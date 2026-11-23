/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Unit tests for the iroha-lib model builders: Tx, Query and TxBatch.
 * They verify the protobuf structure produced by the builders without
 * requiring a running node.
 */

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "cryptography/ed25519_sha3_impl/internal/sha3_hash.hpp"
#include "model/Query.hpp"
#include "model/Tx.hpp"
#include "model/TxBatch.hpp"
#include "model/utils/Utils.h"

namespace {

using namespace iroha::protocol;
using namespace iroha_lib;

const std::string kAccountId = "admin@test";
const std::string kPubKey =
    "52a1d6f4a4b5f6d1f1e0aabbccddeeff00112233445566778899aabbccddeeff";

class TxModelTest : public ::testing::Test {
 protected:
  void SetUp() override { keypair_ = generateRandomKeypair(); }

  Tx makeTx() const {
    return Tx(kAccountId, keypair_, 1234567890, 2);
  }

  iroha::keypair_t keypair_;
};

TEST_F(TxModelTest, PayloadFieldsAreSet) {
  const auto tx = makeTx().getTransaction();
  const auto &payload = tx.payload().reduced_payload();

  EXPECT_EQ(payload.creator_account_id(), kAccountId);
  EXPECT_EQ(payload.created_time(), 1234567890u);
  EXPECT_EQ(payload.quorum(), 2u);
  EXPECT_EQ(payload.commands_size(), 0);
}

TEST_F(TxModelTest, CommandsProduceExpectedCases) {
  Tx tx = makeTx();
  tx.createDomain("test", "user")
      .createAsset("coin", "test", 2)
      .createAccount("alice", "test", kPubKey)
      .transferAsset("admin@test", "alice@test", "coin#test", "tip", "1.00")
      .addAssetQuantity("coin#test", "100", "genesis")
      .subtractAssetQuantity("coin#test", "1", "fee")
      .grantPermission("alice@test", can_set_my_account_detail)
      .revokePermission("alice@test", can_transfer_my_assets)
      .setAccountDetail("alice@test", "age", "30")
      .compareAndSetAccountDetail("alice@test", "age", "31", "30", false)
      .setAccountQuorum("alice@test", 2)
      .addSignatory("alice@test", kPubKey)
      .removeSignatory("alice@test", kPubKey)
      .appendRole("alice@test", "user")
      .detachRole("alice@test", "user")
      .createRole("role1",
                  {can_append_role, can_create_role})
      .addPeer("127.0.0.1:1337", kPubKey)
      .removePeer(kPubKey);

  const auto &commands =
      tx.getTransaction().payload().reduced_payload().commands();
  ASSERT_EQ(commands.size(), 18u);

  EXPECT_EQ(commands.Get(0).command_case(), Command::kCreateDomain);
  EXPECT_EQ(commands.Get(1).command_case(), Command::kCreateAsset);
  EXPECT_EQ(commands.Get(2).command_case(), Command::kCreateAccount);
  EXPECT_EQ(commands.Get(3).command_case(), Command::kTransferAsset);
  EXPECT_EQ(commands.Get(4).command_case(), Command::kAddAssetQuantity);
  EXPECT_EQ(commands.Get(5).command_case(), Command::kSubtractAssetQuantity);
  EXPECT_EQ(commands.Get(6).command_case(), Command::kGrantPermission);
  EXPECT_EQ(commands.Get(7).command_case(), Command::kRevokePermission);
  EXPECT_EQ(commands.Get(8).command_case(), Command::kSetAccountDetail);
  EXPECT_EQ(commands.Get(9).command_case(),
            Command::kCompareAndSetAccountDetail);
  EXPECT_EQ(commands.Get(10).command_case(), Command::kSetAccountQuorum);
  EXPECT_EQ(commands.Get(11).command_case(), Command::kAddSignatory);
  EXPECT_EQ(commands.Get(12).command_case(), Command::kRemoveSignatory);
  EXPECT_EQ(commands.Get(13).command_case(), Command::kAppendRole);
  EXPECT_EQ(commands.Get(14).command_case(), Command::kDetachRole);
  EXPECT_EQ(commands.Get(15).command_case(), Command::kCreateRole);
  EXPECT_EQ(commands.Get(16).command_case(), Command::kAddPeer);
  EXPECT_EQ(commands.Get(17).command_case(), Command::kRemovePeer);
}

TEST_F(TxModelTest, CommandPayloadsArePopulated) {
  Tx tx = makeTx();
  tx.transferAsset("admin@test", "alice@test", "coin#test", "tip", "1.00");

  const auto &commands =
      tx.getTransaction().payload().reduced_payload().commands();
  ASSERT_EQ(commands.size(), 1u);
  const auto &transfer = commands.Get(0).transfer_asset();
  EXPECT_EQ(transfer.src_account_id(), "admin@test");
  EXPECT_EQ(transfer.dest_account_id(), "alice@test");
  EXPECT_EQ(transfer.asset_id(), "coin#test");
  EXPECT_EQ(transfer.amount(), "1.00");
}

TEST_F(TxModelTest, SignAndAddSignatureAddsSignature) {
  Tx tx = makeTx();
  tx.createDomain("test", "user");

  const auto signed_tx = tx.signAndAddSignature();
  ASSERT_EQ(signed_tx.signatures_size(), 1);
  EXPECT_EQ(signed_tx.signatures(0).public_key(),
            keypair_.pubkey.to_hexstring());
  EXPECT_FALSE(signed_tx.signatures(0).signature().empty());
}

TEST_F(TxModelTest, UnsignedTransactionHasNoSignatures) {
  const auto tx = makeTx().getTransaction();
  EXPECT_EQ(tx.signatures_size(), 0);
}

class QueryModelTest : public ::testing::Test {
 protected:
  void SetUp() override { keypair_ = generateRandomKeypair(); }

  iroha_lib::Query makeQuery(const std::string& creator = {}) const {
    return iroha_lib::Query(keypair_, 42, creator, 1234567890);
  }

  iroha::keypair_t keypair_;
};

TEST_F(QueryModelTest, GetAccountSetsQueryType) {
  const auto &query =
      makeQuery().getAccount("alice@test").signAndAddSignature();

  EXPECT_EQ(query.payload().query_case(), iroha::protocol::Query_Payload::kGetAccount);
  EXPECT_EQ(query.payload().get_account().account_id(),
            "alice@test");
  EXPECT_EQ(query.payload().meta().query_counter(), 42u);
  EXPECT_EQ(query.payload().meta().created_time(), 1234567890u);
}

TEST_F(QueryModelTest, QueryTypesProduceExpectedCases) {
  const auto &q = makeQuery()
                       .getAccountAssets("alice@test")
                       .signAndAddSignature();
  EXPECT_EQ(q.payload().query_case(), iroha::protocol::Query_Payload::kGetAccountAssets);

  const auto &q2 = makeQuery()
                        .getPeers("alice@test")
                        .signAndAddSignature();
  EXPECT_EQ(q2.payload().query_case(), iroha::protocol::Query_Payload::kGetPeers);

  const auto &q3 = makeQuery()
                        .getPendingTransactions("alice@test", 5)
                        .signAndAddSignature();
  EXPECT_EQ(q3.payload().query_case(),
            iroha::protocol::Query_Payload::kGetPendingTransactions);

  const auto &q4 = makeQuery()
                        .getSignatories("alice@test")
                        .signAndAddSignature();
  EXPECT_EQ(q4.payload().query_case(), iroha::protocol::Query_Payload::kGetSignatories);
}

TEST_F(QueryModelTest, SignAndAddSignatureSetsSignatureAndCreator) {
  const auto &signed_query =
      makeQuery(kAccountId).getAccount("alice@test").signAndAddSignature();

  EXPECT_EQ(signed_query.signature().public_key(),
            keypair_.pubkey.to_hexstring());
  EXPECT_FALSE(signed_query.signature().signature().empty());
  EXPECT_EQ(signed_query.payload().meta().creator_account_id(), kAccountId);
}

TEST_F(QueryModelTest, DefaultCreatorIsQueriedAccount) {
  // QueryGenerator sets meta.creator_account_id to the queried account
  // when the caller does not provide an explicit creator
  const auto &signed_query =
      makeQuery().getAccount("alice@test").signAndAddSignature();

  EXPECT_EQ(signed_query.payload().meta().creator_account_id(),
            "alice@test");
}

class TxBatchModelTest : public ::testing::Test {
 protected:
  void SetUp() override { keypair_ = generateRandomKeypair(); }

  Transaction makeSignedTx(const std::string& creator, int seed) {
    Tx tx(creator, keypair_, 1000 + seed, 1);
    tx.transferAsset(creator, "bob@test", "coin#test", "tip",
                     std::to_string(seed));
    return tx.signAndAddSignature();
  }

  iroha::keypair_t keypair_;
};

TEST_F(TxBatchModelTest, AtomicBatchSetsMetaOnEveryTx) {
  std::vector<Transaction> txs{
      makeSignedTx(kAccountId, 1), makeSignedTx(kAccountId, 2)};

  TxBatch batch;
  const auto tx_list = batch.batch(txs, /*atomic=*/true);

  ASSERT_EQ(tx_list.transactions_size(), 2);
  for (const auto &tx : tx_list.transactions()) {
    ASSERT_TRUE(tx.payload().has_batch());
    EXPECT_EQ(tx.payload().batch().type(),
              Transaction_Payload_BatchMeta_BatchType_ATOMIC);
    EXPECT_EQ(tx.payload().batch().reduced_hashes_size(), 2);
  }
}

TEST_F(TxBatchModelTest, ReducedHashesMatchTransactionContents) {
  std::vector<Transaction> txs{
      makeSignedTx(kAccountId, 1), makeSignedTx(kAccountId, 2)};

  TxBatch batch;
  const auto tx_list = batch.batch(txs, /*atomic=*/true);

  const auto expected_hash = [](const Transaction &tx) {
    return iroha::sha3_256(tx.payload().reduced_payload()
                               .SerializeAsString())
        .to_hexstring();
  };

  for (int i = 0; i < tx_list.transactions_size(); ++i) {
    EXPECT_EQ(tx_list.transactions(i).payload().batch().reduced_hashes(i),
              expected_hash(txs[i]));
  }
}

TEST_F(TxBatchModelTest, OrderedBatchSetsOrderedMetaType) {
  std::vector<Transaction> txs{makeSignedTx(kAccountId, 1)};

  TxBatch batch;
  const auto tx_list = batch.batch(txs, /*atomic=*/false);

  ASSERT_EQ(tx_list.transactions_size(), 1);
  ASSERT_TRUE(tx_list.transactions(0).payload().has_batch());
  EXPECT_EQ(tx_list.transactions(0).payload().batch().type(),
            Transaction_Payload_BatchMeta_BatchType_ORDERED);
}

}  // namespace
