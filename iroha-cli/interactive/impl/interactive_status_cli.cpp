/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interactive/interactive_status_cli.hpp"

#include <boost/assert.hpp>
#include <fstream>

#include "client.hpp"
#include "common/byteutils.hpp"
#include "tx_status_utils.hpp"

namespace iroha_cli {
  namespace interactive {

    InteractiveStatusCli::InteractiveStatusCli(
        const std::string &default_peer_ip,
        int default_port)
        : default_peer_ip_(default_peer_ip),
          default_port_(default_port) {
      createActionsMenu();
      createResultMenu();
    }

    void InteractiveStatusCli::createActionsMenu() {
      descriptionMap_ = {{GET_TX_INFO, "Get status of transaction"}};
      const auto tx_id = "Requested tx hash";

      requestParamsDescriptions_ = {
          {GET_TX_INFO, makeParamsDescription({tx_id})}};
      actionHandlers_ = {{GET_TX_INFO, &InteractiveStatusCli::parseGetHash}};

      menuPoints_ = formMenu(
          actionHandlers_, requestParamsDescriptions_, descriptionMap_);
      addBackOption(menuPoints_);
    }

    void InteractiveStatusCli::createResultMenu() {
      resultHandlers_ = {{SEND_CODE, &InteractiveStatusCli::parseSendToIroha},
                         {SAVE_CODE, &InteractiveStatusCli::parseSaveFile}};
      resultParamsDescriptions_ =
          getCommonParamsMap(default_peer_ip_, default_port_);

      resultPoints_ = formMenu(resultHandlers_,
                               resultParamsDescriptions_,
                               getCommonDescriptionMap());
      addBackOption(resultPoints_);
    }

    void InteractiveStatusCli::run() {
      bool isParsing = true;
      currentContext_ = MAIN;
      printMenu("Choose action: ", menuPoints_);
      while (isParsing) {
        auto line = promptString("> ");
        if (not line) {
          // line has terminating symbol
          isParsing = false;
          break;
        }
        switch (currentContext_) {
          case MAIN:
            isParsing = parseAction(line.value());
            break;
          case RESULT:
            isParsing = parseResult(line.value());
            break;
          default:
            // shouldn't get here
            BOOST_ASSERT_MSG(false, "not implemented");
            break;
        }
      }
    }

    bool InteractiveStatusCli::parseAction(std::string &line) {
      if (isBackOption(line)) {
        return false;
      }

      auto res = handleParse<std::string>(
          this, line, actionHandlers_, requestParamsDescriptions_);
      if (not res) {
        // Continue parsing
        return true;
      }

      txHash_ = res.value();
      currentContext_ = RESULT;
      printMenu("Tx hash is saved. Choose what to do:", resultPoints_);
      // Continue parsing
      return true;
    }

    bool InteractiveStatusCli::parseResult(std::string &line) {
      if (isBackOption(line)) {
        // Give up the last query and start a new one
        currentContext_ = MAIN;
        printEnd();
        printMenu("Choose action: ", menuPoints_);
        // Continue parsing
        return true;
      }

      auto res = handleParse<bool>(
          this, line, resultHandlers_, resultParamsDescriptions_);

      return res.value_or(true);
    }

    bool InteractiveStatusCli::parseSendToIroha(ActionParams line) {
      auto address =
          parseIrohaPeerParams(line, default_peer_ip_, default_port_);
      if (not address) {
        return true;
      }

      iroha::protocol::ToriiResponse answer;
      if (iroha::hexstringToBytestring(txHash_)) {
        auto response = CliClient(address.value().first,
                                  address.value().second)
                            .getTxStatus(txHash_);
        if (response.status.ok()) {
          answer = response.answer;
        }
      }

      std::cout << getTxStatusMessage(answer) << std::endl;

      printEnd();
      return false;
    }

    bool InteractiveStatusCli::parseSaveFile(ActionParams line) {
      auto path = line[0];
      std::ofstream output_file(path);
      if (not output_file) {
        std::cout << "Cannot create file" << std::endl;
        // Continue parsing
        return true;
      }

      // Save the transaction hash together with the fetched status
      auto response = CliClient(default_peer_ip_, default_port_)
                          .getTxStatus(txHash_);
      const auto message = response.status.ok()
          ? getTxStatusMessage(response.answer)
          : std::string("Failed to retrieve transaction status");

      output_file << "tx_hash: " << txHash_ << "\n"
                  << "status: " << message << "\n";
      std::cout << "Successfully saved!" << std::endl;
      printEnd();
      return false;
    }

    std::string InteractiveStatusCli::parseGetHash(ActionParams params) {
      return params[0];
    }
  }  // namespace interactive
}  // namespace iroha_cli
