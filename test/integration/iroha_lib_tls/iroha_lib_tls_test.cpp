/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Integration test for TLS-encrypted transport in iroha-lib.
 *
 * Boots a single-node Iroha instance via IntegrationTestFramework
 * with the Torii TLS endpoint enabled (test/system/tls/correct.*),
 * then runs the TlsExample executable against the TLS port with the
 * node certificate as the root CA. A second case verifies that the
 * example fails when given an invalid root certificate.
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

#include "builders/protobuf/transaction.hpp"
#include "common/files.hpp"
#include "cryptography/blob.hpp"
#include "cryptography/default_hash_provider.hpp"
#include "datetime/time.hpp"
#include "framework/common_constants.hpp"
#include "framework/integration_framework/integration_test_framework.hpp"
#include "interfaces/permissions.hpp"
#include "module/shared_model/builders/protobuf/block.hpp"

#include "iroha_lib_tls_paths.h"

using namespace common_constants;
using namespace shared_model::crypto;
using shared_model::interface::types::PublicKeyHexStringView;

namespace {

const std::string kAdminAccountId = "admin@test";
const std::string kTestDomain = "test";
const std::string kUserRole = "user";

shared_model::proto::Block tlsTestGenesisBlock(
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

struct RunResult {
    bool exited;
    int exit_code;
};

RunResult runTlsExample(
        const std::string& binary_path,
        uint16_t tls_port,
        const std::string& root_cert_path,
        int timeout_seconds)
{
    const std::string command =
            "IROHA_HOST_ADDR=127.0.0.1"
            " IROHA_PORT=" + std::to_string(tls_port) +
            " ADMIN_ACCOUNT_ID=" + kAdminAccountId +
            " ADMIN_PRIVATE_KEY=" + kAdminKeypair.privateKey().hex() +
            " IROHA_TLS_ROOT_CERT=" + root_cert_path +
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

class IrohaLibTlsTest : public ::testing::Test {
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
                    true,
                    boost::none,
                    std::chrono::milliseconds(20000),
                    std::chrono::milliseconds(20000),
                    std::chrono::milliseconds(10000),
                    integration_framework::getDefaultItfLogManager(),
                    (std::filesystem::temp_directory_path()
                     / iroha::unique_path())
                        .string(),
                    (std::filesystem::temp_directory_path()
                     / iroha::unique_path())
                        .string(),
                    std::string(IROHA_TLS_KEY_PATH));
        itf_->setInitialState(
            kAdminKeypair,
            tlsTestGenesisBlock(itf_->getAddress(), kAdminKeypair));

        ASSERT_TRUE(waitForToriiEndpoint(
            static_cast<uint16_t>(itf_->toriiTlsPort()),
            std::chrono::seconds(30)))
            << "TLS torii endpoint did not come up";
    }

 protected:
    std::unique_ptr<integration_framework::IntegrationTestFramework> itf_;
};

}  // namespace

TEST_F(IrohaLibTlsTest, TlsExampleConnectsOverTls)
{
    const auto result = runTlsExample(
        IROHA_EXAMPLE_TLS,
        static_cast<uint16_t>(itf_->toriiTlsPort()),
        std::string(IROHA_TLS_CERT_PATH),
        120);
    ASSERT_TRUE(result.exited) << "example did not exit";
    ASSERT_EQ(result.exit_code, 0)
        << "TlsExample failed with code " << result.exit_code;
}

TEST_F(IrohaLibTlsTest, TlsExampleRejectsWrongRootCert)
{
    // the private key file is not a valid PEM certificate - the TLS
    // handshake must fail and the example must exit with an error
    const auto result = runTlsExample(
        IROHA_EXAMPLE_TLS,
        static_cast<uint16_t>(itf_->toriiTlsPort()),
        std::string(IROHA_TLS_KEY_FILE),
        120);
    ASSERT_TRUE(result.exited) << "example did not exit";
    ASSERT_NE(result.exit_code, 0)
        << "TlsExample unexpectedly succeeded with an invalid root cert";
}
