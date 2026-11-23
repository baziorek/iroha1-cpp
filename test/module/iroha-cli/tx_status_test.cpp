/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>

#include "tx_status_utils.hpp"

namespace {

TEST(TxStatusMessage, AllKnownStatusesHaveMessage) {
  const std::vector<iroha::protocol::TxStatus> statuses = {
      iroha::protocol::TxStatus::STATELESS_VALIDATION_FAILED,
      iroha::protocol::TxStatus::STATELESS_VALIDATION_SUCCESS,
      iroha::protocol::TxStatus::STATEFUL_VALIDATION_FAILED,
      iroha::protocol::TxStatus::STATEFUL_VALIDATION_SUCCESS,
      iroha::protocol::TxStatus::REJECTED,
      iroha::protocol::TxStatus::COMMITTED,
      iroha::protocol::TxStatus::MST_EXPIRED,
      iroha::protocol::TxStatus::NOT_RECEIVED,
      iroha::protocol::TxStatus::MST_PENDING,
      iroha::protocol::TxStatus::ENOUGH_SIGNATURES_COLLECTED};

  for (const auto status : statuses) {
    iroha::protocol::ToriiResponse response;
    response.set_tx_status(status);
    const auto message = iroha_cli::getTxStatusMessage(response);
    EXPECT_FALSE(message.empty()) << "empty message for status "
                                  << static_cast<int>(status);
    EXPECT_EQ(message.find("try again later"), std::string::npos)
        << "known status " << static_cast<int>(status)
        << " hit the fallback message: " << message;
  }
}

TEST(TxStatusMessage, CommittedMessage) {
  iroha::protocol::ToriiResponse response;
  response.set_tx_status(iroha::protocol::TxStatus::COMMITTED);
  EXPECT_EQ(iroha_cli::getTxStatusMessage(response),
            "Transaction was successfully committed.");
}

TEST(TxStatusMessage, ErrorNameIsAppended) {
  iroha::protocol::ToriiResponse response;
  response.set_tx_status(
      iroha::protocol::TxStatus::STATELESS_VALIDATION_FAILED);
  response.set_err_or_cmd_name("error_name");
  const auto message = iroha_cli::getTxStatusMessage(response);
  EXPECT_NE(message.find("error_name"), std::string::npos);
}

TEST(TxStatusMessage, UnknownStatusFallsBack) {
  iroha::protocol::ToriiResponse response;
  response.set_tx_status(
      static_cast<iroha::protocol::TxStatus>(100500));
  const auto message = iroha_cli::getTxStatusMessage(response);
  EXPECT_NE(message.find("try again later"), std::string::npos);
}

}  // namespace
