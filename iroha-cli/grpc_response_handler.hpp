/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CLI_GRPC_RESPONSE_HANDLER_HPP
#define IROHA_CLI_GRPC_RESPONSE_HANDLER_HPP

#include "client.hpp"
#include "logger/logger_fwd.hpp"
#include "logger/logger_manager_fwd.hpp"
#include "query_response_handler.hpp"
#include "transaction_response_handler.hpp"

namespace spdlog {
  class logger;
}

namespace iroha_cli {
  class GrpcResponseHandler {
   public:
    explicit GrpcResponseHandler(logger::LoggerManagerTreePtr log_manager);
    /**
     * Handle iroha GRPC transaction submission response
     * (submission status plus the fetched transaction status)
     * @param response
     */
    void handle(CliClient::Response<iroha::protocol::ToriiResponse> response);
    /**
     * Handle Iroha GRPC QueryResponse
     * @param response
     */
    void handle(CliClient::Response<iroha::protocol::QueryResponse> response);

   private:
    TransactionResponseHandler tx_handler_;
    QueryResponseHandler query_handler_;
    void handleGrpcErrors(grpc::StatusCode code);
    logger::LoggerPtr log_;
    std::unordered_map<int, std::string> handler_map_;
  };
}  // namespace iroha_cli

#endif  // IROHA_GRPC_RESPONSE_HANDLER_HPP
