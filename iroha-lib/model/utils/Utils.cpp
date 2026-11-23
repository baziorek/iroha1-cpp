#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cstdint>

#include "crypto/keys_manager_impl.hpp"
#include "cryptography/ed25519_sha3_impl/internal/ed25519_impl.hpp"
#include "logger/logger_manager.hpp"
#include "model/converters/pb_common.hpp"
#include "grpc_client/GrpcClient.hpp"
#include "Utils.h"
#include "common/std_format.hpp"


namespace fs = std::filesystem;


void verifyPath(
        const fs::path& path,
        const logger::LoggerPtr& logger)
{
    if (not fs::exists(path)) {
        const auto error_message = "Path " + path.string() + " not found.";
        logger->error(error_message);
        throw error_message;
    }
}


void verifyKepair(
        const iroha::expected::Result<shared_model::crypto::Keypair, std::string>& keypair,
        const logger::LoggerPtr& logger,
        const fs::path& path,
        const std::string& account_name)
{
    if (auto error = iroha::expected::resultToOptionalError(keypair)) {
        const auto error_message = std::format(
                    "Keypair error= {}."
                    "\nKeypair path= {}, name= {}.\n",
                    error.value(),
                    path.string(),
                    account_name);
        logger->error(error_message);
        throw error_message;
    }
}


iroha::keypair_t generateKeypair(
        const std::string& account_name,
        const std::string& key_path,
        const logger::LoggerManagerTreePtr& log_manager)
{
    const auto logger = log_manager->getChild("Main")->getLogger();
    const auto keys_manager_log = log_manager->getChild("KeysManager")->getLogger();
    fs::path path(key_path);

    verifyPath(
                path,
                logger);

    iroha::KeysManagerImpl manager(
                (path / account_name)
                .string(),
                keys_manager_log);
    auto keypair = manager.loadKeys(boost::none);

    verifyKepair(
                keypair,
                logger,
                path,
                account_name);
    return iroha::keypair_t(
                iroha::pubkey_t::from_hexstring(keypair.assumeValue().publicKey()).assumeValue(),
                iroha::privkey_t::from_string(toBinaryString(keypair.assumeValue().privateKey())).assumeValue());
}


iroha::keypair_t generateKeypairFromPrivateKey(
        const std::string& private_key_hex)
{
    auto privkey = iroha::privkey_t::from_hexstring(private_key_hex);
    if (iroha::expected::hasError(privkey)) {
        throw std::invalid_argument(
            "Invalid private key hex: " + privkey.assumeError());
    }
    return iroha::create_keypair(privkey.assumeValue());
}


iroha::keypair_t generateRandomKeypair()
{
    return iroha::create_keypair();
}


std::string getPublicKeyHexFromPrivateKey(
        const std::string& private_key_hex)
{
    return generateKeypairFromPrivateKey(private_key_hex).pubkey.to_hexstring();
}


void addSignature(
        iroha::protocol::Transaction& tx,
        const iroha::keypair_t& keypair)
{
    const auto signature = iroha::sign(
                iroha::hash(tx).to_string(),
                keypair.pubkey,
                keypair.privkey);

    auto proto_signature = tx.add_signatures();
    proto_signature->set_public_key(keypair.pubkey.to_hexstring());
    proto_signature->set_signature(signature.to_hexstring());
}


const std::string getTransactionHash(const Transaction& tx)
{
    return iroha::hash(tx).to_hexstring();
}


void printTransactionStatus(
        const std::string& peer_ip,
        uint16_t torii_port,
        const std::string& tx_hash,
        const std::optional<iroha_lib::TlsOptions>& tls)
{
    const auto status = iroha_lib::GrpcClient(
                peer_ip,
                torii_port,
                tls)
            .getTxStatus(tx_hash);
    std::cout << "Tx hash=" << tx_hash
              << "  Status name=" << TxStatus_Name(status.tx_status())
              << "  Status code=" << status.tx_status()
              << "  Error code=" << status.error_code()
              << std::endl;
}


void printTransactionStatuses(
        const std::string& peer_ip,
        uint16_t torii_port,
        const std::vector<Transaction>& transactions,
        const std::optional<iroha_lib::TlsOptions>& tls)
{
    for (const auto& tx: transactions) {
        printTransactionStatus(
                    peer_ip,
                    torii_port,
                    getTransactionHash(tx),
                    tls);
    }
}


std::string string_to_hex(const std::string& in)
{
    std::stringstream ss;

    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < in.length(); ++i) {
        ss << std::setw(2) << static_cast<std::uint32_t>(static_cast<std::uint8_t>(in[i]));
    }
    return ss.str();
}

std::pair<std::string,std::string> splitAssetFullName(const std::string& assetFullName)
{
    constexpr const char nameDomainSeparator = '#';
    const auto separatorPosition = assetFullName.find(nameDomainSeparator);
    const std::string assetName = assetFullName.substr(0, separatorPosition);
    const std::string assetDomain = assetFullName.substr(separatorPosition+1);
    return {assetName, assetDomain};
}
