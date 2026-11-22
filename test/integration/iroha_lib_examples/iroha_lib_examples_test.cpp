/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Integration tests for the iroha-lib examples ported from iroha-python.
 *
 * Each test spins up a real single-node Iroha instance via
 * IntegrationTestFramework with a genesis block that replicates the
 * environment expected by the Python examples (admin@test account,
 * "test" domain with "user" default role that has all permissions),
 * then runs the corresponding example executable as a child process
 * with connection details passed through environment variables.
 */

#include <gtest/gtest.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "builders/protobuf/transaction.hpp"
#include "cryptography/blob.hpp"
#include "cryptography/default_hash_provider.hpp"
#include "datetime/time.hpp"
#include "framework/common_constants.hpp"
#include "framework/integration_framework/integration_test_framework.hpp"
#include "interfaces/permissions.hpp"
#include "module/shared_model/builders/protobuf/block.hpp"

#include "iroha_lib_examples_paths.h"

using namespace common_constants;
using namespace shared_model::crypto;
using shared_model::interface::types::PublicKeyHexStringView;

namespace {

const std::string kAdminAccountId = "admin@test";
const std::string kTestDomain = "test";
const std::string kUserRole = "user";

/**
 * Genesis block replicating the environment the Python examples expect:
 * admin account "admin@test" in domain "test", where the domain's default
 * role "user" is granted all permissions (so every account created in
 * the domain can perform the commands used by the examples).
 */
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

/**
 * Wait until the node's torii endpoint accepts TCP connections.
 */
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

struct RunResult {
    bool exited;
    int exit_code;
};

RunResult runExample(
        const std::string& binary_path,
        uint16_t torii_port,
        int timeout_seconds)
{
    const std::string command =
            "IROHA_HOST_ADDR=127.0.0.1"
            " IROHA_PORT=" + std::to_string(torii_port) +
            " ADMIN_ACCOUNT_ID=" + kAdminAccountId +
            " ADMIN_PRIVATE_KEY=" + kAdminKeypair.privateKey().hex() +
            " timeout " + std::to_string(timeout_seconds) +
            " " + binary_path;

    const int status = std::system(command.c_str());
    RunResult result{false, -1};
    if (WIFEXITED(status)) {
        result.exited = true;
        result.exit_code = WEXITSTATUS(status);
    }
    return result;
}

class IrohaLibExamplesTest : public ::testing::Test {
 protected:
    void SetUp() override
    {
        itf_ = std::make_unique<
            integration_framework::IntegrationTestFramework>(
                    1,
                    iroha::StorageType::kPostgres,
                    boost::none,
                    iroha::StartupWsvDataPolicy::kDrop,
                    true,
                    true);  // mst_support - required by BatchExample
        itf_->setInitialState(
            kAdminKeypair,
            pythonLikeGenesisBlock(itf_->getAddress(), kAdminKeypair));

        ASSERT_TRUE(waitForToriiEndpoint(
            static_cast<uint16_t>(itf_->toriiPort()),
            std::chrono::seconds(30)))
            << "torii endpoint did not come up";
    }

    void runExampleAndExpectSuccess(const char* binary_path)
    {
        const auto result = runExample(
            binary_path,
            static_cast<uint16_t>(itf_->toriiPort()),
            300);
        ASSERT_TRUE(result.exited) << "example did not exit";
        ASSERT_EQ(result.exit_code, 0)
            << "example " << binary_path << " failed with code "
            << result.exit_code;
    }

 protected:
    std::unique_ptr<integration_framework::IntegrationTestFramework> itf_;
};

}  // namespace

TEST_F(IrohaLibExamplesTest, TxExample)
{
    runExampleAndExpectSuccess(IROHA_EXAMPLE_TX);
}

TEST_F(IrohaLibExamplesTest, QueryExample)
{
    runExampleAndExpectSuccess(IROHA_EXAMPLE_QUERY);
}

TEST_F(IrohaLibExamplesTest, TxOrderingExample)
{
    runExampleAndExpectSuccess(IROHA_EXAMPLE_TX_ORDERING);
}

TEST_F(IrohaLibExamplesTest, BlocksQueryExample)
{
    runExampleAndExpectSuccess(IROHA_EXAMPLE_BLOCKS_QUERY);
}

TEST_F(IrohaLibExamplesTest, BatchExample)
{
    runExampleAndExpectSuccess(IROHA_EXAMPLE_BATCH);
}

TEST_F(IrohaLibExamplesTest, InfiniteBlocksStreamExample)
{
    // the example is an infinite stream - it is expected to be killed
    // by `timeout`, which yields exit code 124
    const auto result = runExample(
        IROHA_EXAMPLE_INFINITE_BLOCKS_STREAM,
        static_cast<uint16_t>(itf_->toriiPort()),
        20);
    ASSERT_TRUE(result.exited) << "example did not exit";
    ASSERT_EQ(result.exit_code, 124)
        << "example exited prematurely with code " << result.exit_code;
}
