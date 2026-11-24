#pragma once

#include <filesystem>
#include <iostream>

#include "crypto/keys_manager_impl.hpp"
#include "logger/logger_manager.hpp"
#include "model/converters/pb_common.hpp"
#include "grpc_client/GrpcClient.hpp"


using namespace iroha::protocol;
namespace fs = std::filesystem;


void verifyPath(
        const fs::path& path,
        const logger::LoggerPtr& logger);

void verifyKepair(
        const iroha::expected::Result<shared_model::crypto::Keypair, std::string>& keypair,
        const logger::LoggerPtr& logger,
        const fs::path& path,
        const std::string& account_name);

iroha::keypair_t generateKeypair(
        const std::string& account_name,
        const std::string& key_path,
        const logger::LoggerManagerTreePtr& log_manager);

iroha::keypair_t generateKeypairFromPrivateKey(
        const std::string& private_key_hex);

iroha::keypair_t generateRandomKeypair();

std::string getPublicKeyHexFromPrivateKey(
        const std::string& private_key_hex);

void addSignature(
        iroha::protocol::Transaction& tx,
        const iroha::keypair_t& keypair);

const std::string getTransactionHash(const Transaction& tx);

void printTransactionStatus(
        const std::string& peer_ip,
        uint16_t torii_port,
        const std::string& tx_hash,
        const std::optional<iroha_lib::TlsOptions>& tls = std::nullopt);

void printTransactionStatuses(
        const std::string& peer_ip,
        uint16_t torii_port,
        const std::vector<Transaction>& transactions,
        const std::optional<iroha_lib::TlsOptions>& tls = std::nullopt);

std::string string_to_hex(const std::string& in);

/// Separator between the asset name and the domain in an asset id
/// ("name#domain").
inline constexpr char kAssetIdSeparator = '#';
/// Separator between the account name and the domain in an account id
/// ("name@domain").
inline constexpr char kAccountIdSeparator = '@';

std::pair<std::string,std::string> splitAssetFullName(const std::string& assetFullName);

/// Splits an account id ("name@domain") into {name, domain}.
std::pair<std::string,std::string> splitAccountId(const std::string& accountId);

/// Combines an asset name and a domain into an asset id ("name#domain").
std::string combineAssetId(const std::string& assetName, const std::string& domain);

/// Combines an account name and a domain into an account id ("name@domain").
std::string combineAccountId(const std::string& accountName, const std::string& domain);
