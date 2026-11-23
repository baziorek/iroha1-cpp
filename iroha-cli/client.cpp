/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "client.hpp"

#include <chrono>
#include <thread>
#include <unordered_set>

#include "model/converters/pb_common.hpp"

namespace iroha_cli {

  CliClient::CliClient(std::string target_ip, int port)
      : client_(std::move(target_ip), port) {}

  CliClient::Response<iroha::protocol::ToriiResponse> CliClient::sendTx(
      const iroha::protocol::Transaction &tx) {
    CliClient::Response<iroha::protocol::ToriiResponse> response;
    response.status = client_.send(tx);
    if (not response.status.ok()) {
      return response;
    }

    // Fetch the real transaction status. The status endpoint reports
    // intermediate states too (stateless/stateful validation success,
    // MST pending, ...), so poll until a final status is reached or
    // the timeout expires.
    static const std::unordered_set<iroha::protocol::TxStatus> final_statuses = {
        iroha::protocol::TxStatus::COMMITTED,
        iroha::protocol::TxStatus::REJECTED,
        iroha::protocol::TxStatus::MST_EXPIRED,
        iroha::protocol::TxStatus::STATELESS_VALIDATION_FAILED,
        iroha::protocol::TxStatus::STATEFUL_VALIDATION_FAILED};

    const auto tx_hash = iroha::hash(tx).to_hexstring();
    for (int attempt = 0; attempt < 25; ++attempt) {
      const auto status = client_.getTxStatus(tx_hash, &response.answer);
      if (status.ok()
          && final_statuses.count(response.answer.tx_status()) != 0) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return response;
  }

  CliClient::Response<iroha::protocol::QueryResponse> CliClient::sendQuery(
      const iroha::protocol::Query &query) {
    CliClient::Response<iroha::protocol::QueryResponse> response;
    response.status = client_.sendQuery(query, &response.answer);
    return response;
  }

  CliClient::Response<iroha::protocol::ToriiResponse> CliClient::getTxStatus(
      const std::string &tx_hash) {
    CliClient::Response<iroha::protocol::ToriiResponse> response;
    response.status = client_.getTxStatus(tx_hash, &response.answer);
    return response;
  }

}  // namespace iroha_cli
