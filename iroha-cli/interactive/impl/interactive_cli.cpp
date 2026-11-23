/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interactive/interactive_cli.hpp"

#include <utility>

#include "logger/logger_manager.hpp"

namespace iroha_cli {
  namespace interactive {

    void InteractiveCli::assign_main_handlers() {
      addCliCommand(menu_points_,
                    main_handler_map_,
                    TX_CODE,
                    "New transaction",
                    &InteractiveCli::startTx);
      addCliCommand(menu_points_,
                    main_handler_map_,
                    QRY_CODE,
                    "New query",
                    &InteractiveCli::startQuery);
      addCliCommand(menu_points_,
                    main_handler_map_,
                    ST_CODE,
                    "New transaction status request",
                    &InteractiveCli::startTxStatusRequest);
    }

    InteractiveCli::InteractiveCli(
        const std::string &account_name,
        const std::string &default_peer_ip,
        int default_port,
        uint64_t qry_counter,
        const iroha::keypair_t &keypair,
        logger::LoggerManagerTreePtr response_handler_log_manager,
        logger::LoggerPtr pb_qry_factory_log,
        logger::LoggerPtr json_qry_factory_log,
        logger::LoggerManagerTreePtr log_manager,
        const std::optional<iroha_lib::TlsOptions> &tls)
        : creator_(account_name),
          tx_cli_(creator_,
                  default_peer_ip,
                  default_port,
                  keypair,
                  response_handler_log_manager,
                  log_manager->getChild("Transaction")->getLogger(),
                  tls),
          query_cli_(creator_,
                     default_peer_ip,
                     default_port,
                     qry_counter,
                     keypair,
                     std::move(response_handler_log_manager),
                     pb_qry_factory_log,
                     std::move(json_qry_factory_log),
                     log_manager->getChild("Query")->getLogger(),
                     tls),
          statusCli_(default_peer_ip, default_port, tls) {
      assign_main_handlers();
    }

    void InteractiveCli::parseMain(std::string line) {
      auto raw_command = parser::parseFirstCommand(std::move(line));
      if (not raw_command) {
        handleEmptyCommand();
        return;
      }
      auto command_name = *raw_command;

      auto val = findInHandlerMap(command_name, main_handler_map_);
      if (val) {
        (this->*val.value())();
      }
    }

    void InteractiveCli::startQuery() {
      query_cli_.run();
    }

    void InteractiveCli::startTx() {
      tx_cli_.run();
    }

    void InteractiveCli::startTxStatusRequest() {
      statusCli_.run();
    }

    void InteractiveCli::run() {
      std::cout << "Welcome to Iroha-Cli. " << std::endl;
      // Parsing cycle
      while (true) {
        printMenu("Choose what to do:", menu_points_);
        auto line = promptString("> ");
        if (not line) {
          // Line contains terminating symbol
          break;
        }
        parseMain(line.value());
      }
    }

  }  // namespace interactive
}  // namespace iroha_cli
