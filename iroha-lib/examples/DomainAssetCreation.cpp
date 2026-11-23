#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <cassert>
#include <gflags/gflags.h>
#include "model/Tx.hpp"
#include "model/Query.hpp"
#include "model/utils/Utils.h"


/// Command line options:
DEFINE_string(admin_account_name, "admin@test", "set the admin account name. The account will be used to create domain and asset");
DEFINE_string(key_path, ".", "set the key path. Here should be private and public key pair for admin");
DEFINE_string(peer_ip, "127.0.0.1", "set the peer IP address. It is address of Iroha node");
DEFINE_uint32(torii_port, 50051u, "set the torii port. Port of iroha node to send commands and queries.");
DEFINE_string(user_default_role, "user", "set the user default role for newly created domain");
DEFINE_string(asset_full_name, "assetnamesamplev4#domainsamplev4", "set the asset full name (format asset_name#domain)");
DEFINE_string(tls_root_cert, "", "optional path to PEM root certificate; when set, connections use TLS");


iroha_lib::Query generateQueryBase(const std::string& account_name, const std::string& key_path);
iroha::protocol::Query generateGetAccountAssetsQuery(const std::string& account_name, const std::string& key_path);
iroha::protocol::Query generateGetAccountTransactionsQuery(const std::string& account_name, const std::string& key_path);
iroha::protocol::Query generateGetAccountQuery(const std::string& account_name, const std::string& key_path);

template<typename Tx>
void sendTransaction(Tx& tx, const std::string& peer_ip, const uint16_t torii_port,
                     const std::optional<iroha_lib::TlsOptions>& tls);
std::optional<iroha_lib::TlsOptions> makeTlsOptions(const std::string& cert_path);

iroha::protocol::Transaction generateTransactionWhichCreatesDomainAndAsset(
        const std::string& account_name,
        const std::string& key_path,
        const std::string& domain_id,
        const std::string& user_default_role,
        const std::string& asset_name);
iroha::protocol::Transaction generateTransactionWhichAddsAssetQuantiti(
        const std::string& account_name,
        const std::string& key_path,
        const std::string& assetIdWithDomain,
        const std::string& assetAmount);

void printAccountAssets(const std::string& account_name, const std::string& key_path, const std::string& peer_ip, const uint16_t torii_port,
                        const std::optional<iroha_lib::TlsOptions>& tls);
void printAccount(const std::string& account_name, const std::string& key_path, const std::string& peer_ip, const uint16_t torii_port,
                  const std::optional<iroha_lib::TlsOptions>& tls);

void printErrorResponse(const QueryResponse& response);

void run(const std::string& adminAccountName, const std::string& key_path,
         const std::string& peer_ip, uint16_t torii_port,
         const std::string& user_default_role,
         const std::string& assetFullName,
         const std::optional<iroha_lib::TlsOptions>& tls);


int main(int argc, char* argv[]) try {
    gflags::SetUsageMessage("Usage: " + std::string(argv[0]));
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    run(FLAGS_admin_account_name,
        FLAGS_key_path,
        FLAGS_peer_ip,
        FLAGS_torii_port,
        FLAGS_user_default_role,
        FLAGS_asset_full_name,
        makeTlsOptions(FLAGS_tls_root_cert));

    gflags::ShutDownCommandLineFlags();
} catch (const std::exception& e) {
    std::cerr << std::format("Exception from {}: '{}'\n", __FUNCTION__, e.what());
}

void run(const std::string& adminAccountName, const std::string& key_path,
         const std::string& peer_ip, uint16_t torii_port,
         const std::string& user_default_role,
         const std::string& assetFullName,
         const std::optional<iroha_lib::TlsOptions>& tls)
{
    const auto [assetName, assetDomain] = splitAssetFullName(assetFullName);

    const auto tx2CreateDomainAndAsset = generateTransactionWhichCreatesDomainAndAsset(
                adminAccountName,
                key_path,
                assetDomain,
                user_default_role,
                assetName);

    const auto tx2AddAssetQuantity = generateTransactionWhichAddsAssetQuantiti(
                adminAccountName,
                key_path,
                assetFullName,
                "100");

    sendTransaction(tx2CreateDomainAndAsset, peer_ip, torii_port, tls);
    sendTransaction(tx2AddAssetQuantity, peer_ip, torii_port, tls);

    /// querying:
    printAccountAssets(adminAccountName, key_path, peer_ip, torii_port, tls);
    printAccount(adminAccountName, key_path, peer_ip, torii_port, tls);
}

iroha::protocol::Transaction generateTransactionWhichCreatesDomainAndAsset(
        const std::string& account_name,
        const std::string& key_path,
        const std::string& domain_id,
        const std::string& user_default_role,
        const std::string& asset_name)
{
    static auto log_manager = std::make_shared<logger::LoggerManagerTree>(
                logger::LoggerConfig{logger::LogLevel::kInfo,
                                     logger::getDefaultLogPatterns()})->getChild("CLI");
    const auto keypair = generateKeypair(
                account_name,
                key_path,
                log_manager);

    return iroha_lib::Tx(
                account_name,
                keypair)
            .createDomain(
                domain_id,
                user_default_role)
            .createAsset(
                asset_name,
                domain_id,
                0)
            .signAndAddSignature();
}

iroha::protocol::Transaction generateTransactionWhichAddsAssetQuantiti(
        const std::string& account_name,
        const std::string& key_path,
        const std::string& assetIdWithDomain,
        const std::string& assetAmount)
{
    static auto log_manager = std::make_shared<logger::LoggerManagerTree>(
                logger::LoggerConfig{logger::LogLevel::kInfo,
                                     logger::getDefaultLogPatterns()})->getChild("CLI");
    const auto keypair = generateKeypair(
                account_name,
                key_path,
                log_manager);

    return iroha_lib::Tx(
                account_name,
                keypair)
            .addAssetQuantity(assetIdWithDomain, assetAmount, "")
            .signAndAddSignature();
}

std::optional<iroha_lib::TlsOptions> makeTlsOptions(const std::string& cert_path)
{
    if (cert_path.empty()) {
        return std::nullopt;
    }
    std::ifstream file(cert_path);
    if (not file.is_open()) {
        throw std::runtime_error("Cannot open TLS root certificate: " + cert_path);
    }
    std::ostringstream pem;
    pem << file.rdbuf();
    iroha_lib::TlsOptions tls;
    tls.pem_root_certs = pem.str();
    return tls;
}

template<typename Tx>
void sendTransaction(
        Tx& tx,
        const std::string& peer_ip,
        const uint16_t torii_port,
        const std::optional<iroha_lib::TlsOptions>& tls)
{
    iroha_lib::GrpcClient(
                peer_ip,
                torii_port,
                tls)
            .send(tx);

    printTransactionStatus(
                peer_ip,
                torii_port,
                getTransactionHash(tx),
                tls);
}

void printAccountAssets(const std::string& account_name,
                        const std::string& key_path,
                        const std::string& peer_ip,
                        const uint16_t torii_port,
                        const std::optional<iroha_lib::TlsOptions>& tls)
{
    std::cout << std::format("----------->{}-----------\n", __FUNCTION__);

    const auto query_proto = generateGetAccountAssetsQuery(
                account_name,
                key_path);
    const QueryResponse response = iroha_lib::GrpcClient(
                peer_ip,
                torii_port,
                tls)
            .send(query_proto);
    const auto payload = query_proto.payload();
    assert(payload.get_account_assets().account_id() == account_name);
    assert(payload.has_get_account_assets());

    if (response.has_error_response())
    {
        printErrorResponse(response);
        return;
    }

    assert(response.has_account_assets_response());
    const auto accountAssetsResponce = response.account_assets_response();
    for (const auto& r : accountAssetsResponce.account_assets())
    {
        std::cout << std::format("\tasset: {} {}\n", r.asset_id(), r.balance());
    }

    std::cout << std::format("-----------<{}-----------\n",  __FUNCTION__);
}

iroha::protocol::Query generateGetAccountAssetsQuery(const std::string& account_name,
        const std::string& key_path)
{
    return generateQueryBase(account_name, key_path)
            .getAccountAssets(account_name)
            .signAndAddSignature();
}

iroha_lib::Query generateQueryBase(
        const std::string& account_name,
        const std::string& key_path)
{
    static auto log_manager = std::make_shared<logger::LoggerManagerTree>(
                logger::LoggerConfig{logger::LogLevel::kInfo,
                                     logger::getDefaultLogPatterns()})->getChild("CLI");

    const auto keypair = generateKeypair(
                account_name,
                key_path,
                log_manager);
    return iroha_lib::Query(keypair);
}

iroha::protocol::Query generateGetAccountTransactionsQuery(
        const std::string& account_name,
        const std::string& key_path)
{
    return generateQueryBase(account_name, key_path)
            .getAccountTransactions(account_name)
            .signAndAddSignature();
}

iroha::protocol::Query generateGetAccountQuery(
        const std::string& account_name,
        const std::string& key_path)
{
    return generateQueryBase(account_name, key_path)
            .getAccount(account_name)
            .signAndAddSignature();
}

void printErrorResponse(const QueryResponse& response)
{
    const auto errorResponse = response.error_response();
    std::cerr << std::format("{}: {}", static_cast<int>(errorResponse.error_code()), errorResponse.message()) << std::endl;
}

void printAccount(const std::string& account_name,
                  const std::string& key_path,
                  const std::string& peer_ip,
                  const uint16_t torii_port,
                  const std::optional<iroha_lib::TlsOptions>& tls)
{
    std::cout << std::format("----------->{}-----------\n", __FUNCTION__);

    const auto query_proto = generateGetAccountQuery(
                account_name,
                key_path);
    const QueryResponse response = iroha_lib::GrpcClient(
                peer_ip,
                torii_port,
                tls)
            .send(query_proto);
    const auto payload = query_proto.payload();
    assert(payload.get_account().account_id() == account_name);
    assert(payload.has_get_account());

    if (response.has_error_response())
    {
        printErrorResponse(response);
        return;
    }

    assert(response.has_account_response());
    const auto account = response.account_response().account();
    std::cout << std::format("account_id={},\n"
               "domain_id={}\n"
               "quorum={}\n"
               "json_data={}\n",
               account.account_id(), account.domain_id(), account.quorum(), account.json_data());

    std::cout << std::format("-----------<{}-----------\n", __FUNCTION__);
}
