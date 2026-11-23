/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CLI_CLIENT_HPP
#define IROHA_CLI_CLIENT_HPP

#include <string>

#include "endpoint.grpc.pb.h"
#include "grpc++/grpc++.h"
#include "grpc_client/GrpcClient.hpp"

namespace iroha_cli {

  /**
   * Thin wrapper over iroha_lib::GrpcClient used by iroha-cli.
   * Responses carry the gRPC status plus the torii/query answer.
   */
  class CliClient {
   public:
    template <typename T>
    struct Response {
      grpc::Status status;
      T answer;
    };

    CliClient(std::string target_ip, int port);

    /**
     * Send Transaction to Iroha Peer, i.e. target_ip:port, then fetch
     * its status from the torii endpoint.
     * @param tx signed transaction in protobuf transport format
     * @return gRPC status of the submission and the transaction status
     */
    CliClient::Response<iroha::protocol::ToriiResponse> sendTx(
        const iroha::protocol::Transaction &tx);

    /**
     * Send Query to Iroha Peer, i.e. target_ip:port
     * @param query signed query in protobuf transport format
     * @return gRPC status and the query response
     */
    CliClient::Response<iroha::protocol::QueryResponse> sendQuery(
        const iroha::protocol::Query &query);

    CliClient::Response<iroha::protocol::ToriiResponse> getTxStatus(
        const std::string &tx_hash);

   private:
    iroha_lib::GrpcClient client_;
  };
}  // namespace iroha_cli

#endif  // IROHA_CLI_CLIENT_HPP
