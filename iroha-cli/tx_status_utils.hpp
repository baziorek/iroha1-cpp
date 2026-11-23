/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CLI_TX_STATUS_UTILS_HPP
#define IROHA_CLI_TX_STATUS_UTILS_HPP

#include <endpoint.pb.h>
#include <map>
#include <stdexcept>
#include <string>

namespace iroha_cli {
  /**
   * Human-readable description of a transaction status response,
   * shared by the transaction response handler and the interactive
   * status cli.
   */
  inline std::string getTxStatusMessage(
      const iroha::protocol::ToriiResponse &response) {
    static const std::map<iroha::protocol::TxStatus, std::string>
        userMessageMap = {
            {iroha::protocol::TxStatus::STATELESS_VALIDATION_FAILED,
             "Transaction has not passed stateless validation."},
            {iroha::protocol::TxStatus::STATELESS_VALIDATION_SUCCESS,
             "Transaction has successfully passed stateless validation."},
            {iroha::protocol::TxStatus::STATEFUL_VALIDATION_FAILED,
             "Transaction has not passed stateful validation."},
            {iroha::protocol::TxStatus::STATEFUL_VALIDATION_SUCCESS,
             "Transaction has successfully passed stateful validation."},
            {iroha::protocol::TxStatus::REJECTED,
             "Transaction has been rejected."},
            {iroha::protocol::TxStatus::COMMITTED,
             "Transaction was successfully committed."},
            {iroha::protocol::TxStatus::MST_EXPIRED,
             "Transaction has not collected enough signatures in time."},
            {iroha::protocol::TxStatus::NOT_RECEIVED,
             "Transaction was not found in the system."},
            {iroha::protocol::TxStatus::MST_PENDING,
             "Transaction has not collected quorum of signatures."},
            {iroha::protocol::TxStatus::ENOUGH_SIGNATURES_COLLECTED,
             "Transaction has collected all signatures."}};

    const auto status = response.tx_status();
    std::string message;
    try {
      message = userMessageMap.at(status);
      if (not response.err_or_cmd_name().empty()) {
        message += " error '" + response.err_or_cmd_name() + "'";
      }
    } catch (const std::out_of_range &e) {
      message =
          "A problem detected while retrieving transaction status. Please "
          "try again later.";
    }
    return message;
  }
}  // namespace iroha_cli

#endif  // IROHA_CLI_TX_STATUS_UTILS_HPP
