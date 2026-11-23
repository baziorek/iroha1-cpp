/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "transaction_response_handler.hpp"
#include "logger/logger.hpp"
#include "tx_status_utils.hpp"

namespace iroha_cli {

  void TransactionResponseHandler::handle(
      const iroha::protocol::ToriiResponse &response) const {
    log_->info("{}", getTxStatusMessage(response));
  }
  TransactionResponseHandler::TransactionResponseHandler(logger::LoggerPtr log)
      : log_(std::move(log)) {}

}  // namespace iroha_cli
