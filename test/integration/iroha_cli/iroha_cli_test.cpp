/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Integration tests for iroha-cli noninteractive modes.
 *
 * JsonTransactionAndQuery boots a real single-node Iroha instance via
 * IntegrationTestFramework (same genesis as the iroha-lib examples tests)
 * and runs `iroha-cli --json_transaction/--json_query` against it.
 * The remaining tests exercise modes that do not need a node:
 * --new_account and --genesis_block.
 */

#include <gtest/gtest.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "builders/protobuf/transaction.hpp"
#include "common/result.hpp"
#include "cryptography/blob.hpp"
#include "cryptography/default_hash_provider.hpp"
#include "datetime/time.hpp"
#include "framework/common_constants.hpp"
#include "framework/integration_framework/integration_test_framework.hpp"
#include "framework/test_logger.hpp"
#include "interfaces/permissions.hpp"
#include "model/Query.hpp"
#include "model/Tx.hpp"
#include "model/converters/json_common.hpp"
#include "model/converters/json_query_factory.hpp"
#include "model/converters/json_transaction_factory.hpp"
#include "model/converters/pb_query_factory.hpp"
#include "model/converters/pb_transaction_factory.hpp"
#include "module/shared_model/builders/protobuf/block.hpp"

#include "iroha_cli_paths.h"

using namespace common_constants;
using namespace shared_model::crypto;
using shared_model::interface::types::PublicKeyHexStringView;

namespace fs = std::filesystem;

namespace {

const std::string kAdminAccountId = "admin@test";
const std::string kTestDomain = "test";
const std::string kUserRole = "user";

/** Same genesis block as in the iroha-lib examples tests. */
shared_model::proto::Block pythonLikeGenesisBlock(
        const std::string& peer_address,
        const Keypair& key)
{
    shared_model::interface::RolePermissionSet all_perms{};
    for (size_t i = 0; i < all_perms.size(); ++i) {
        all_perms.set(
            static_cast<shared_model::interface::permissions::Role>(i));
    }

    auto genesis_tx = shared_model::proto::TransactionBuilder()
            .creatorAccountId(kAdminAccountId)
            .createdTime(iroha::time::now())
            .addPeer(peer_address, PublicKeyHexStringView{key.publicKey()})
            .createRole(kUserRole, all_perms)
            .createDomain(kTestDomain, kUserRole)
            .createAccount("admin",
                           kTestDomain,
                           PublicKeyHexStringView{key.publicKey()})
            .quorum(1)
            .build()
            .signAndAddSignature(key)
            .finish();

    return shared_model::proto::BlockBuilder()
            .transactions(
                std::vector<shared_model::proto::Transaction>{genesis_tx})
            .height(1)
            .prevHash(DefaultHashProvider::makeHash(Blob("")))
            .createdTime(iroha::time::now())
            .build()
            .signAndAddSignature(key)
            .finish();
}

bool waitForToriiEndpoint(uint16_t port, std::chrono::seconds timeout)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd >= 0) {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            const int rc = ::connect(fd, reinterpret_cast<sockaddr*>(&addr),
                                     sizeof(addr));
            ::close(fd);
            if (rc == 0) {
                return true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return false;
}

/**
 * Run iroha-cli with the given extra arguments inside `work_dir`,
 * capturing combined stdout/stderr into a file.
 */
struct CliRunResult {
    bool exited;
    int exit_code;
    std::string output;
};

CliRunResult runCli(const std::string& work_dir,
                    const std::vector<std::string>& args,
                    int timeout_seconds = 60,
                    const std::string& stdin_content = "")
{
    const auto uniq =
        std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count());
    const fs::path out_path =
        fs::temp_directory_path() / ("iroha_cli_test_out_" + uniq);
    const fs::path in_path =
        fs::temp_directory_path() / ("iroha_cli_test_in_" + uniq);
    if (not stdin_content.empty()) {
        std::ofstream in(in_path);
        in << stdin_content;
    }

    std::ostringstream command;
    command << "cd " << work_dir << " && timeout " << timeout_seconds
            << " " << IROHA_CLI_BIN;
    for (const auto& arg : args) {
        command << " '" << arg << "'";
    }
    command << " > " << out_path.string() << " 2>&1";
    if (not stdin_content.empty()) {
        command << " < " << in_path.string();
    }

    const int status = std::system(command.str().c_str());
    CliRunResult result{false, -1, {}};
    if (WIFEXITED(status)) {
        result.exited = true;
        result.exit_code = WEXITSTATUS(status);
    }
    std::ifstream out(out_path);
    std::ostringstream ss;
    ss << out.rdbuf();
    result.output = ss.str();
    fs::remove(out_path);
    if (not stdin_content.empty()) {
        fs::remove(in_path);
    }
    return result;
}

class TempWorkDir {
 public:
    TempWorkDir() {
        path_ = fs::temp_directory_path() /
            ("iroha_cli_test_" +
             std::to_string(
                 std::chrono::steady_clock::now().time_since_epoch().count()));
        fs::create_directories(path_);
    }
    ~TempWorkDir() { fs::remove_all(path_); }
    const fs::path& path() const { return path_; }

 private:
    fs::path path_;
};

/** Convert shared_model keypair to the old-model keypair used by the lib. */
iroha::keypair_t toOldModelKeypair(const Keypair& keypair) {
    return {iroha::pubkey_t::from_hexstring(keypair.publicKey()).assumeValue(),
            iroha::privkey_t::from_string(
                shared_model::crypto::toBinaryString(keypair.privateKey()))
                .assumeValue()};
}

/** Write a signed JSON transaction file built with iroha-lib Tx. */
fs::path writeSignedTxJson(const fs::path& dir, const std::string& name) {
    const auto old_keypair = toOldModelKeypair(kAdminKeypair);
    iroha_lib::Tx tx(kAdminAccountId, old_keypair, iroha::time::now(), 1);
    tx.createDomain("cli-domain", kUserRole);
    const auto proto_tx = tx.signAndAddSignature();

    auto model_tx =
        iroha::model::converters::PbTransactionFactory::deserialize(proto_tx);
    EXPECT_TRUE(static_cast<bool>(model_tx));

    const auto doc =
        iroha::model::converters::JsonTransactionFactory().serialize(*model_tx);
    const auto json =
        iroha::model::converters::jsonToString(doc);

    const auto path = dir / name;
    std::ofstream file(path);
    file << json;
    return path;
}

/** Write a signed JSON query file built with iroha-lib Query. */
fs::path writeSignedQueryJson(const fs::path& dir, const std::string& name) {
    const auto old_keypair = toOldModelKeypair(kAdminKeypair);
    iroha_lib::Query query(old_keypair, 1, kAdminAccountId, iroha::time::now());
    query.getAccount(kAdminAccountId);
    const auto proto_query = query.signAndAddSignature();

    auto model_query =
        iroha::model::converters::PbQueryFactory(getTestLogger("PbQueryFactory"))
            .deserialize(proto_query);
    EXPECT_TRUE(static_cast<bool>(model_query));

    const auto json =
        iroha::model::converters::JsonQueryFactory(
            getTestLogger("JsonQueryFactory"))
            .serialize(*model_query);

    const auto path = dir / name;
    std::ofstream file(path);
    file << json;
    return path;
}

class IrohaCliNoNodeTest : public ::testing::Test {};

TEST_F(IrohaCliNoNodeTest, NewAccount)
{
    TempWorkDir dir;
    const auto result = runCli(dir.path().string(),
        {"--new_account",
         "--account_name=cli_test_acc",
         "--interactive=false"});
    ASSERT_TRUE(result.exited);
    ASSERT_EQ(result.exit_code, 0) << result.output;
    EXPECT_TRUE(fs::exists(dir.path() / "cli_test_acc.pub"));
    EXPECT_TRUE(fs::exists(dir.path() / "cli_test_acc.priv"));

    // Second run overwrites the existing keys (KeysManagerImpl::createKeys
    // semantics, same as the original CLI)
    const auto second = runCli(dir.path().string(),
        {"--new_account",
         "--account_name=cli_test_acc",
         "--interactive=false"});
    ASSERT_TRUE(second.exited);
    ASSERT_EQ(second.exit_code, 0) << second.output;
    EXPECT_TRUE(fs::exists(dir.path() / "cli_test_acc.pub"));
    EXPECT_TRUE(fs::exists(dir.path() / "cli_test_acc.priv"));
}

TEST_F(IrohaCliNoNodeTest, GenesisBlockFromPeers)
{
    TempWorkDir dir;
    const auto peers_path = dir.path() / "peers.txt";
    {
        std::ofstream peers(peers_path);
        peers << "127.0.0.1:50051 127.0.0.2:50052\n";
    }

    const auto result = runCli(dir.path().string(),
        {"--genesis_block",
         "--peers_address=" + peers_path.string(),
         "--interactive=false"});
    ASSERT_TRUE(result.exited);
    ASSERT_EQ(result.exit_code, 0) << result.output;

    const auto genesis_path = dir.path() / "genesis.block";
    ASSERT_TRUE(fs::exists(genesis_path));
    std::ifstream genesis(genesis_path);
    std::ostringstream content;
    content << genesis.rdbuf();
    EXPECT_NE(content.str().find("\"transactions\""), std::string::npos);
}

TEST_F(IrohaCliNoNodeTest, GenesisBlockFromTransaction)
{
    TempWorkDir dir;
    const auto tx_path = writeSignedTxJson(dir.path(), "tx.json");

    const auto result = runCli(dir.path().string(),
        {"--genesis_block",
         "--genesis_transaction=" + tx_path.string(),
         "--interactive=false"});
    ASSERT_TRUE(result.exited);
    ASSERT_EQ(result.exit_code, 0) << result.output;
    EXPECT_TRUE(fs::exists(dir.path() / "genesis.block"));
}

class IrohaCliTest : public ::testing::Test {
 protected:
    void SetUp() override
    {
        itf_ = std::make_unique<
            integration_framework::IntegrationTestFramework>(
                    10,
                    iroha::StorageType::kPostgres,
                    boost::none,
                    iroha::StartupWsvDataPolicy::kDrop,
                    true,
                    false);
        itf_->setInitialState(
            kAdminKeypair,
            pythonLikeGenesisBlock(itf_->getAddress(), kAdminKeypair));

        ASSERT_TRUE(waitForToriiEndpoint(
            static_cast<uint16_t>(itf_->toriiPort()),
            std::chrono::seconds(30)))
            << "torii endpoint did not come up";
    }

 protected:
    std::unique_ptr<integration_framework::IntegrationTestFramework> itf_;
};

TEST_F(IrohaCliTest, JsonTransactionAndQuery)
{
    TempWorkDir dir;
    const auto tx_path = writeSignedTxJson(dir.path(), "tx.json");
    const auto query_path = writeSignedQueryJson(dir.path(), "query.json");
    const auto port = std::to_string(itf_->toriiPort());

    const auto tx_result = runCli(dir.path().string(),
        {"--peer_ip=127.0.0.1",
         "--torii_port=" + port,
         "--json_transaction=" + tx_path.string(),
         "--interactive=false"},
        120);
    ASSERT_TRUE(tx_result.exited);
    ASSERT_EQ(tx_result.exit_code, 0) << tx_result.output;
    EXPECT_NE(tx_result.output.find("successfully committed"),
              std::string::npos)
        << "CLI output:\n" << tx_result.output;

    const auto query_result = runCli(dir.path().string(),
        {"--peer_ip=127.0.0.1",
         "--torii_port=" + port,
         "--json_query=" + query_path.string(),
         "--interactive=false"},
        120);
    ASSERT_TRUE(query_result.exited);
    ASSERT_EQ(query_result.exit_code, 0) << query_result.output;
    EXPECT_NE(query_result.output.find(kAdminAccountId), std::string::npos)
        << query_result.output;
}

TEST_F(IrohaCliTest, InteractiveTransactionAndQuery)
{
    TempWorkDir dir;

    // Write admin key files in the KeysManager format (hex pub/priv)
    {
        std::ofstream pub(dir.path() / "admin@test.pub");
        pub << kAdminKeypair.publicKey();
        std::ofstream priv(dir.path() / "admin@test.priv");
        priv << kAdminKeypair.privateKey().hex();
    }

    const auto port = std::to_string(itf_->toriiPort());
    const std::string script =
        // form and send a transaction (create domain)
        "tx\n"
        "crt_dmn cli-domain2 user\n"
        "send\n"
        "127.0.0.1\n" + port + "\n"
        // form and send a query (get account)
        "qry\n"
        "get_acc admin@test\n"
        "send\n"
        "127.0.0.1\n" + port + "\n";

    const auto result = runCli(dir.path().string(),
        {"--interactive",
         "--account_name=admin@test",
         "--key_path=" + dir.path().string(),
         "--peer_ip=127.0.0.1",
         "--torii_port=" + port},
        120,
        script);
    ASSERT_TRUE(result.exited) << result.output;
    ASSERT_EQ(result.exit_code, 0) << result.output;
    EXPECT_NE(result.output.find("successfully committed"),
              std::string::npos)
        << "CLI output:\n" << result.output;
    EXPECT_NE(result.output.find(kAdminAccountId), std::string::npos)
        << "CLI output:\n" << result.output;
}

}  // namespace
