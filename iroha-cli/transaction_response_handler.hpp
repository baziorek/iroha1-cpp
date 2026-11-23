/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CLI_TRANSACTION_RESPONSE_HANDLER_HPP
#define IROHA_CLI_TRANSACTION_RESPONSE_HANDLER_HPP

#include <endpoint.pb.h>

#include "logger/logger_fwd.hpp"

namespace spdlog {
  class logger;
}

namespace iroha_cli {

  class TransactionResponseHandler {
   public:
    explicit TransactionResponseHandler(logger::LoggerPtr log);
    /**
     * Handle transaction status response from Iroha
     * @param response torii response with the transaction status
     */
    void handle(const iroha::protocol::ToriiResponse &response) const;

   private:
    logger::LoggerPtr log_;
  };
}  // namespace iroha_cli

#endif  // IROHA_CLI_TRANSACTION_RESPONSE_HANDLER_HPP
