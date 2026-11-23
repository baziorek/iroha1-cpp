#pragma once

#include <string>
#include <vector>

#include "backend/protobuf/block.hpp"
#include "common/result.hpp"
#include "logger/logger_fwd.hpp"
#include "transaction.pb.h"

namespace iroha_lib {

/**
 * Configuration for sample genesis block generation
 * (equivalent of `iroha-cli --genesis_block`).
 */
struct GenesisConfig {
    /// peer addresses ("ip:port"), one per network node;
    /// a fresh keypair is generated for each of them
    std::vector<std::string> peer_addresses;

    /// directory where generated key files are stored
    /// (node0..nodeN for peers, admin@test, test@test for accounts)
    std::string keys_directory = ".";

    std::string domain = "test";
    std::string default_role = "user";
    std::string asset_name = "coin";
    uint32_t asset_precision = 2;

    /// account names used for the two sample accounts
    std::string admin_name = "admin";
    std::string user_name = "test";
};

/**
 * Read whitespace-separated peer addresses from a file
 * (equivalent of `iroha-cli --peers_address`).
 */
std::vector<std::string> readPeersAddresses(const std::string &path);

/**
 * Generate a genesis block (height 1) replicating what
 * `iroha-cli --genesis_block` produces: peer registrations,
 * admin/user/money_creator roles, sample domain, asset and accounts.
 *
 * Key files for peers and accounts are (re)generated in keys_directory
 * on each call, like the original implementation.
 *
 * @return proto genesis block ready to be serialized to JSON
 *         (the block itself is unsigned, like the original output)
 */
iroha::expected::Result<shared_model::proto::Block, std::string>
generateGenesisBlock(const GenesisConfig &config,
                     const logger::LoggerPtr &log);

/**
 * Wrap already-signed transactions into a genesis block
 * (equivalent of `iroha-cli --genesis_block --genesis_transaction`).
 * The transactions are expected to contain their signatures.
 */
iroha::expected::Result<shared_model::proto::Block, std::string>
generateGenesisBlockFromTransactions(
    const std::vector<iroha::protocol::Transaction> &transactions);

}  // namespace iroha_lib
