#include "model/utils/genesis.hpp"

#include <fstream>
#include <iterator>
#include <unordered_set>

#include <boost/optional.hpp>

#include "backend/protobuf/block.hpp"
#include "backend/protobuf/permissions.hpp"
#include "backend/protobuf/transaction.hpp"
#include "common/result.hpp"
#include "crypto/keypair.hpp"
#include "crypto/keys_manager_impl.hpp"
#include "cryptography/blob.hpp"
#include "cryptography/default_hash_provider.hpp"
#include "datetime/time.hpp"
#include "../Tx.hpp"
#include "model/converters/pb_common.hpp"

namespace iroha_lib {

namespace {

using shared_model::interface::permissions::Role;
using shared_model::interface::RolePermissionSet;

RolePermissionSet adminPermissions() {
    RolePermissionSet perms{};
    for (auto p : {Role::kCreateDomain,
                   Role::kCreateAccount,
                   Role::kAddPeer,
                   Role::kSetQuorum,
                   Role::kAddSignatory,
                   Role::kRemoveSignatory,
                   Role::kGetAllAccounts,
                   Role::kGetAllSignatories,
                   Role::kGetAllAccAst,
                   Role::kGetAllAccDetail,
                   Role::kGetAllAccTxs,
                   Role::kGetAllAccAstTxs,
                   Role::kGetAllTxs,
                   Role::kGetRoles,
                   Role::kReadAssets,
                   Role::kGetBlocks,
                   Role::kGetAllEngineReceipts}) {
        perms.set(p);
    }
    return perms;
}

RolePermissionSet userPermissions() {
    RolePermissionSet perms{};
    for (auto p : {Role::kReceive,
                   Role::kTransfer,
                   Role::kGetMyAccount,
                   Role::kGetMySignatories,
                   Role::kGetMyAccAst,
                   Role::kGetMyAccDetail,
                   Role::kGetMyAccTxs,
                   Role::kGetMyAccAstTxs,
                   Role::kGetMyTxs,
                   Role::kGetMyEngineReceipts,
                   Role::kSetMyQuorum,
                   Role::kAddMySignatory,
                   Role::kRemoveMySignatory,
                   Role::kTransferMyAssets,
                   Role::kSetMyAccountDetail}) {
        perms.set(p);
    }
    return perms;
}

RolePermissionSet moneyCreatorPermissions() {
    // NOTE: replicates the effective permission set of the original
    // CommandGenerator::generateCreateAssetCreatorRole (the read_self
    // insert there is an empty range and adds nothing)
    RolePermissionSet perms{};
    for (auto p : {Role::kReceive,
                   Role::kTransfer,
                   Role::kCreateAsset,
                   Role::kAddAssetQty}) {
        perms.set(p);
    }
    return perms;
}

std::unordered_set<iroha::protocol::RolePermission>
toProtoPermissions(const RolePermissionSet &perms) {
    std::unordered_set<iroha::protocol::RolePermission> result;
    for (size_t i = 0; i < perms.size(); ++i) {
        auto role = static_cast<Role>(i);
        if (perms.isSet(role)) {
            result.insert(shared_model::proto::permissions::toTransport(
                role));
        }
    }
    return result;
}

/**
 * Create key files (overwriting existing ones, like the original
 * implementation) and load the keypair.
 */
iroha::expected::Result<shared_model::crypto::Keypair, std::string>
loadOrCreateKeypair(const std::string &name,
                    const std::string &directory,
                    const logger::LoggerPtr &log) {
    iroha::KeysManagerImpl manager(name, directory, log);
    manager.createKeys(boost::none);
    return manager.loadKeys(boost::none);
}

iroha::keypair_t toOldKeypair(
    const shared_model::crypto::Keypair &keypair) {
    return iroha::keypair_t(
        iroha::pubkey_t::from_hexstring(keypair.publicKey()).assumeValue(),
        iroha::privkey_t::from_string(
            toBinaryString(keypair.privateKey()))
            .assumeValue());
}

std::string accountId(const std::string &name, const std::string &domain) {
    return name + "@" + domain;
}

/**
 * Build an unsigned genesis block (height 1, zero prev hash),
 * like the old BlockGenerator::generateGenesisBlock did.
 */
shared_model::proto::Block makeGenesisBlock(
    std::vector<iroha::protocol::Transaction> transactions,
    uint64_t created_time) {
    iroha::protocol::Block_v1 block_v1;
    auto *payload = block_v1.mutable_payload();
    payload->set_height(1);
    payload->set_tx_number(transactions.size());
    payload->set_prev_block_hash(
        shared_model::crypto::DefaultHashProvider::makeHash(
            shared_model::crypto::Blob(""))
            .hex());
    payload->set_created_time(created_time);
    for (auto &tx : transactions) {
        *payload->add_transactions() = std::move(tx);
    }
    return shared_model::proto::Block(std::move(block_v1));
}

}  // namespace

std::vector<std::string> readPeersAddresses(const std::string &path) {
    std::ifstream file(path);
    return {std::istream_iterator<std::string>(file),
            std::istream_iterator<std::string>()};
}

iroha::expected::Result<shared_model::proto::Block, std::string>
generateGenesisBlock(const GenesisConfig &config,
                     const logger::LoggerPtr &log) {
    const auto admin_id = accountId(config.admin_name, config.domain);
    const auto user_id = accountId(config.user_name, config.domain);

    auto admin_keypair_res =
        loadOrCreateKeypair(admin_id, config.keys_directory, log);
    if (auto error = iroha::expected::resultToOptionalError(admin_keypair_res)) {
        return iroha::expected::makeError(error.value());
    }
    auto admin_keypair = admin_keypair_res.assumeValue();

    auto user_keypair_res =
        loadOrCreateKeypair(user_id, config.keys_directory, log);
    if (auto error = iroha::expected::resultToOptionalError(user_keypair_res)) {
        return iroha::expected::makeError(error.value());
    }
    auto user_keypair = user_keypair_res.assumeValue();

    Tx tx(admin_id,
          toOldKeypair(admin_keypair),
          iroha::time::now(),
          1);

    for (size_t i = 0; i < config.peer_addresses.size(); ++i) {
        auto peer_key_res = loadOrCreateKeypair(
            "node" + std::to_string(i), config.keys_directory, log);
        if (auto error =
                iroha::expected::resultToOptionalError(peer_key_res)) {
            return iroha::expected::makeError(error.value());
        }
        tx.addPeer(config.peer_addresses[i],
                   peer_key_res.assumeValue().publicKey());
    }

    tx.createRole("admin", toProtoPermissions(adminPermissions()));
    tx.createRole(config.default_role, toProtoPermissions(userPermissions()));
    tx.createRole("money_creator",
                  toProtoPermissions(moneyCreatorPermissions()));
    tx.createDomain(config.domain, config.default_role);
    tx.createAsset(config.asset_name, config.domain, config.asset_precision);
    tx.createAccount(config.admin_name,
                     config.domain,
                     admin_keypair.publicKey());
    tx.createAccount(config.user_name,
                     config.domain,
                     user_keypair.publicKey());
    tx.appendRole(admin_id, "admin");
    tx.appendRole(admin_id, "money_creator");

    auto signed_tx = tx.signAndAddSignature();
    return makeGenesisBlock({std::move(signed_tx)}, iroha::time::now());
}

iroha::expected::Result<shared_model::proto::Block, std::string>
generateGenesisBlockFromTransactions(
    const std::vector<iroha::protocol::Transaction> &transactions) {
    if (transactions.empty()) {
        return iroha::expected::makeError(
            std::string("genesis block requires at least one transaction"));
    }
    return makeGenesisBlock(transactions, iroha::time::now());
}

}  // namespace iroha_lib
