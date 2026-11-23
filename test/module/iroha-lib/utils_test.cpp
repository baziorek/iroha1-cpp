/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <chrono>
#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "cryptography/default_hash_provider.hpp"
#include "framework/test_logger.hpp"
#include "model/Tx.hpp"
#include "model/utils/genesis.hpp"
#include "model/utils/key_files.hpp"
#include "model/utils/Utils.h"

namespace {

namespace fs = std::filesystem;

class TempDir {
 public:
  TempDir() {
    path_ = fs::temp_directory_path() /
            ("iroha_lib_utils_test_" +
             std::to_string(
                 std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(path_);
  }
  ~TempDir() { fs::remove_all(path_); }
  const fs::path &path() const { return path_; }

 private:
  fs::path path_;
};

TEST(KeyFiles, CreateAndLoad) {
  TempDir dir;
  auto log = getTestLogger("KeyFiles");

  auto created = iroha_lib::createKeyFiles(
      "admin@test", "", dir.path().string(), log);
  ASSERT_FALSE(iroha::expected::resultToOptionalError(created));

  auto loaded = iroha_lib::loadKeyFiles(
      "admin@test", "", dir.path().string(), log);
  ASSERT_FALSE(iroha::expected::resultToOptionalError(loaded));
  EXPECT_EQ(created.assumeValue().publicKey(),
            loaded.assumeValue().publicKey());
  EXPECT_EQ(created.assumeValue().privateKey().hex(),
            loaded.assumeValue().privateKey().hex());

  EXPECT_TRUE(fs::exists(dir.path() / "admin@test.pub"));
  EXPECT_TRUE(fs::exists(dir.path() / "admin@test.priv"));
}

TEST(KeyFiles, LoadMissingFails) {
  TempDir dir;
  auto log = getTestLogger("KeyFiles");

  auto loaded = iroha_lib::loadKeyFiles(
      "nobody@test", "", dir.path().string(), log);
  EXPECT_TRUE(iroha::expected::resultToOptionalError(loaded).has_value());
}

TEST(KeyFiles, PassPhraseRoundtrip) {
  TempDir dir;
  auto log = getTestLogger("KeyFiles");

  ASSERT_FALSE(iroha::expected::resultToOptionalError(
      iroha_lib::createKeyFiles("admin@test", "secret",
                                dir.path().string(), log)));

  auto ok = iroha_lib::loadKeyFiles(
      "admin@test", "secret", dir.path().string(), log);
  ASSERT_FALSE(iroha::expected::resultToOptionalError(ok));

  auto wrong = iroha_lib::loadKeyFiles(
      "admin@test", "wrong", dir.path().string(), log);
  EXPECT_TRUE(iroha::expected::resultToOptionalError(wrong).has_value());
}

TEST(ReadPeersAddresses, ReadsWhitespaceSeparated) {
  TempDir dir;
  const auto file = dir.path() / "peers.txt";
  {
    std::ofstream out(file);
    out << "127.0.0.1:1337\n"
           "0.0.0.0:1338   10.0.0.1:1339\n"
           "\n"
           "192.168.1.1:1337\n";
  }

  auto peers = iroha_lib::readPeersAddresses(file.string());
  ASSERT_EQ(peers.size(), 4u);
  EXPECT_EQ(peers[0], "127.0.0.1:1337");
  EXPECT_EQ(peers[1], "0.0.0.0:1338");
  EXPECT_EQ(peers[2], "10.0.0.1:1339");
  EXPECT_EQ(peers[3], "192.168.1.1:1337");
}

TEST(ReadPeersAddresses, MissingFileGivesEmpty) {
  TempDir dir;
  auto peers = iroha_lib::readPeersAddresses(
      (dir.path() / "nope.txt").string());
  EXPECT_TRUE(peers.empty());
}

TEST(Genesis, GeneratesSignedBlock) {
  TempDir dir;
  auto log = getTestLogger("Genesis");

  iroha_lib::GenesisConfig config;
  config.peer_addresses = {"127.0.0.1:1337", "127.0.0.1:1338"};
  config.keys_directory = dir.path().string();

  auto block = iroha_lib::generateGenesisBlock(config, log);
  ASSERT_FALSE(iroha::expected::resultToOptionalError(block));

  const auto &transport = block.assumeValue().getTransport();
  const auto &payload = transport.payload();

  EXPECT_EQ(payload.height(), 1u);
  EXPECT_EQ(payload.tx_number(), 1u);
  EXPECT_EQ(payload.transactions_size(), 1);
  EXPECT_EQ(payload.prev_block_hash(),
            shared_model::crypto::DefaultHashProvider::makeHash(
                shared_model::crypto::Blob(""))
                .hex());
  EXPECT_GT(payload.transactions(0).signatures_size(), 0);

  // peers + 3 roles + domain + asset + 2 accounts + 2 append_role
  EXPECT_EQ(payload.transactions(0)
                .payload()
                .reduced_payload()
                .commands_size(),
            11);

  // key files were generated
  EXPECT_TRUE(fs::exists(dir.path() / "node0.pub"));
  EXPECT_TRUE(fs::exists(dir.path() / "node1.pub"));
  EXPECT_TRUE(fs::exists(dir.path() / "admin@test.pub"));
  EXPECT_TRUE(fs::exists(dir.path() / "test@test.pub"));
}

TEST(Genesis, DeterministicStructureAcrossCalls) {
  TempDir dir;
  auto log = getTestLogger("Genesis");

  iroha_lib::GenesisConfig config;
  config.peer_addresses = {"127.0.0.1:1337"};
  config.keys_directory = dir.path().string();

  auto first = iroha_lib::generateGenesisBlock(config, log);
  auto second = iroha_lib::generateGenesisBlock(config, log);
  ASSERT_FALSE(iroha::expected::resultToOptionalError(first));
  ASSERT_FALSE(iroha::expected::resultToOptionalError(second));

  const auto commands_of = [](const shared_model::proto::Block &b) {
    return b.getTransport()
        .payload()
        .transactions(0)
        .payload()
        .reduced_payload()
        .commands_size();
  };
  EXPECT_EQ(commands_of(first.assumeValue()),
            commands_of(second.assumeValue()));
}

TEST(Genesis, WrapsGivenTransactions) {
  auto keypair = generateRandomKeypair();
  iroha_lib::Tx tx("admin@test", keypair, 1, 1);
  tx.createDomain("test", "user");
  auto signed_tx = tx.signAndAddSignature();

  auto block = iroha_lib::generateGenesisBlockFromTransactions({signed_tx});
  ASSERT_FALSE(iroha::expected::resultToOptionalError(block));

  const auto &payload = block.assumeValue().getTransport().payload();
  EXPECT_EQ(payload.height(), 1u);
  EXPECT_EQ(payload.tx_number(), 1u);
  EXPECT_EQ(payload.transactions_size(), 1);
  EXPECT_EQ(payload.transactions(0).payload()
                .reduced_payload()
                .creator_account_id(),
            "admin@test");
}

TEST(Genesis, RejectsEmptyTransactionList) {
  auto block = iroha_lib::generateGenesisBlockFromTransactions({});
  EXPECT_TRUE(iroha::expected::resultToOptionalError(block).has_value());
}

}  // namespace
