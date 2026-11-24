/// @file PermissionsExample.cpp
/// @brief Role-based permissions: a freshly created user (default "user"
/// role) tries to create an asset and the transaction fails stateful
/// validation with "No such permissions". The admin then creates a role
/// with the can_create_asset permission, appends the user to it and the
/// very same command succeeds. The expected failure is inspected
/// programmatically via the transaction status error code, not just
/// printed. Requires a running Iroha node whose admin account is allowed
/// to create roles (can_create_role / can_append_role or root).
///
/// Connection settings come from environment variables (see common.hpp):
/// IROHA_HOST_ADDR, IROHA_PORT, ADMIN_ACCOUNT_ID, ADMIN_PRIVATE_KEY.

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include "common.hpp"

using namespace iroha_example;
using namespace iroha_lib;

const std::string kDomain = "test";
const std::string kUserName = "permuser";
const std::string kUserAccount = combineAccountId(kUserName, kDomain);
const std::string kAssetName = "userasset";
const std::string kAssetId = combineAssetId(kAssetName, kDomain);
const std::string kRoleName = "asset_creator";

// proto command name reported in ToriiResponse::err_or_cmd_name
const std::string kCreateAssetCommandName = "CreateAsset";

Config config;
GrpcClient* client = nullptr;
std::unique_ptr<GrpcClient> client_holder;
iroha::keypair_t admin_keypair;
iroha::keypair_t user_keypair;

void createUserAccount();

/// Sends a transaction and returns the first status reporting a
/// validation failure (STATELESS/STATEFUL_VALIDATION_FAILED or
/// REJECTED); if the transaction commits, returns the last status seen.
ToriiResponse sendTransactionAndGetFinalStatus(
        const Transaction& tx);

Transaction makeCreateAssetTransaction();

void checkPermissionDenied(const ToriiResponse& status);

void grantAssetCreationRole();


int main()
{
    try {
        config = loadConfig();
        admin_keypair = generateKeypairFromPrivateKey(config.admin_private_key);
        user_keypair = generateRandomKeypair();

        client_holder = makeClient(config);
        client = client_holder.get();

        std::cout << "1. admin creates user account " << kUserAccount
                  << std::endl;
        createUserAccount();

        std::cout << "2. user tries to create asset " << kAssetId
                  << " (expected to fail)" << std::endl;
        const auto denied =
                sendTransactionAndGetFinalStatus(makeCreateAssetTransaction());
        checkPermissionDenied(denied);

        std::cout << "3. admin creates role \"" << kRoleName
                  << "\" and appends user to it" << std::endl;
        grantAssetCreationRole();

        std::cout << "4. user retries createAsset (expected to succeed)"
                  << std::endl;
        sendTransactionAndPrintStatus(*client, makeCreateAssetTransaction());
    } catch (const std::exception& e) {
        std::cerr << "[E] " << e.what() << std::endl;
        return 1;
    }
}


void createUserAccount()
{
    auto tx = makeTx(config.admin_account_id, admin_keypair)
            .createAccount(kUserName, kDomain,
                           user_keypair.pubkey.to_hexstring())
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*client, tx);
}

ToriiResponse sendTransactionAndGetFinalStatus(
        const Transaction& tx)
{
    const auto hash = getTransactionHash(tx);
    std::cout << "Transaction \"" << joinCommands(tx) << "\","
              << " hash = " << hash
              << ", creator = " << tx.payload().reduced_payload().creator_account_id()
              << std::endl;

    client->send(tx);

    auto stream = client->sendStatusStream(hash);
    ToriiResponse status;
    ToriiResponse last;
    std::optional<ToriiResponse> failure;
    int index = 0;
    while (stream->Read(&status)) {
        printTxStatus(status, index++);
        last = status;
        if (isFailureStatus(status.tx_status()) && not failure) {
            failure = status;
        }
    }
    if (failure) {
        return *failure;
    }
    if (last.tx_hash().empty()) {
        throw std::runtime_error("no status received for tx " + hash);
    }
    return last;
}

Transaction makeCreateAssetTransaction()
{
    return makeTx(kUserAccount, user_keypair)
            .createAsset(kAssetName, kDomain, 2)
            .signAndAddSignature();
}

void checkPermissionDenied(const ToriiResponse& status)
{
    if (status.tx_status() != TxStatus::STATEFUL_VALIDATION_FAILED) {
        throw std::runtime_error(
                    "expected STATEFUL_VALIDATION_FAILED, got "
                    + std::string(TxStatus_Name(status.tx_status())));
    }
    if (status.err_or_cmd_name() != kCreateAssetCommandName) {
        throw std::runtime_error(
                    "expected failing command '" + kCreateAssetCommandName
                    + "', got '" + status.err_or_cmd_name() + "'");
    }
    if (status.error_code() != command_error::kNoPermissions) {
        throw std::runtime_error(
                    "expected error_code " + std::to_string(command_error::kNoPermissions)
                    + " (No such permissions), got "
                    + std::to_string(status.error_code())
                    + " (" + errorCodeToHumanReadable(
                        status.err_or_cmd_name(), status.error_code()) + ")");
    }
    std::cout << "as expected: \"" << status.err_or_cmd_name()
              << "\" failed with error_code=" << status.error_code()
              << " (" << errorCodeToHumanReadable(
                     status.err_or_cmd_name(), status.error_code())
              << ")" << std::endl;
}

void grantAssetCreationRole()
{
    auto tx = makeTx(config.admin_account_id, admin_keypair)
            .createRole(kRoleName,
                        {RolePermission::can_create_asset})
            .appendRole(kUserAccount, kRoleName)
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*client, tx);
}
