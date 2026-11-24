#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "queries.pb.h"
#include "common.hpp"


namespace iroha_example {

using namespace iroha_lib;

/// Optional parameters of a GetAccountTransactions query; unset fields
/// mean "no bound" on that side of the range.
struct AccountTransactionsParams {
    uint32_t page_size = 10;
    std::optional<std::string> first_tx_hash;
    std::optional<google::protobuf::Timestamp> first_tx_time;
    std::optional<google::protobuf::Timestamp> last_tx_time;
    std::optional<uint64_t> first_tx_height;
    std::optional<uint64_t> last_tx_height;
    std::optional<Ordering> ordering;
};

// Shared scenario ported from iroha-python tx-example.py /
// query-transactions.py: domain+asset creation, account creation,
// transfers, permissions and account detail queries.
struct Scenario {
    Config config;
    std::unique_ptr<GrpcClient> client;
    iroha::keypair_t admin_keypair;
    iroha::keypair_t user_keypair;
    std::string user_account_name = "userone";
    std::string user_domain = "domain";
    std::string asset_short_id = "coin";

    std::string userAccountId() const
    {
        return combineAccountId(user_account_name, user_domain);
    }

    std::string assetId() const
    {
        return combineAssetId(asset_short_id, user_domain);
    }

    void init()
    {
        config = loadConfig();
        admin_keypair = generateKeypairFromPrivateKey(config.admin_private_key);
        user_keypair = generateRandomKeypair();
        client = makeClient(config);
    }

    void createDomainAndAsset(
            const std::string& domain,
            const std::string& asset,
            uint32_t precision = 2,
            const std::string& default_role = "user")
    {
        auto tx = makeTx(config.admin_account_id, admin_keypair)
                .createDomain(domain, default_role)
                .createAsset(asset, domain, precision)
                .signAndAddSignature();
        sendTransactionAndPrintStatus(*client, tx);
    }

    uint64_t addCoinToAdmin(
            const std::string& asset,
            const std::string& amount = "1000.00")
    {
        auto tx = makeTx(config.admin_account_id, admin_keypair)
                .addAssetQuantity(asset, amount)
                .signAndAddSignature();
        const auto created_time =
                tx.payload().reduced_payload().created_time();
        sendTransactionAndPrintStatus(*client, tx);
        return created_time;
    }

    void createAccount(
            const std::string& account_name,
            const std::string& domain)
    {
        auto tx = makeTx(config.admin_account_id, admin_keypair)
                .createAccount(account_name, domain,
                               user_keypair.pubkey.to_hexstring())
                .signAndAddSignature();
        sendTransactionAndPrintStatus(*client, tx);
    }

    void createAccountWithKey(
            const std::string& account_name,
            const std::string& domain,
            const iroha::keypair_t& keypair)
    {
        auto tx = makeTx(config.admin_account_id, admin_keypair)
                .createAccount(account_name, domain,
                               keypair.pubkey.to_hexstring())
                .signAndAddSignature();
        sendTransactionAndPrintStatus(*client, tx);
    }

    void transferCoin(
            const std::string& source_account,
            const std::string& destination_account,
            const std::string& asset_id,
            const std::string& amount = "2.00")
    {
        auto tx = makeTx(config.admin_account_id, admin_keypair)
                .transferAsset(source_account,
                               destination_account,
                               asset_id,
                               "init top up",
                               amount)
                .signAndAddSignature();
        sendTransactionAndPrintStatus(*client, tx);
    }

    void userGrantsToAdminSetAccountDetailPermission(
            const std::string& account_id)
    {
        auto tx = makeTx(account_id, user_keypair)
                .grantPermission(config.admin_account_id,
                                 GrantablePermission::can_set_my_account_detail)
                .signAndAddSignature();
        sendTransactionAndPrintStatus(*client, tx);
    }

    void setAccountDetail(
            const std::string& account_id,
            const std::string& key,
            const std::string& value)
    {
        auto tx = makeTx(config.admin_account_id, admin_keypair)
                .setAccountDetail(account_id, key, value)
                .signAndAddSignature();
        sendTransactionAndPrintStatus(*client, tx);
    }

    void addAssetQuantity(
            const std::string& asset_id,
            const std::string& amount)
    {
        auto tx = makeTx(config.admin_account_id, admin_keypair)
                .addAssetQuantity(asset_id, amount)
                .signAndAddSignature();
        sendTransactionAndPrintStatus(*client, tx);
    }

    void createAsset(
            const std::string& asset_name,
            const std::string& domain_id,
            uint32_t precision = 2)
    {
        auto tx = makeTx(config.admin_account_id, admin_keypair)
                .createAsset(asset_name, domain_id, precision)
                .signAndAddSignature();
        sendTransactionAndPrintStatus(*client, tx);
    }

    QueryResponse getAssetInfo(const std::string& asset_id)
    {
        const auto query = iroha_lib::Query(
                    admin_keypair, nextQueryCounter(), config.admin_account_id)
                .getAssetInfo(config.admin_account_id, asset_id)
                .signAndAddSignature();
        return client->send(query);
    }

    QueryResponse getAccountAssets(const std::string& account_id)
    {
        const auto query = iroha_lib::Query(
                    admin_keypair, nextQueryCounter(), config.admin_account_id)
                .getAccountAssets(account_id)
                .signAndAddSignature();
        return client->send(query);
    }

    QueryResponse getAccountDetail(const std::string& account_id)
    {
        const auto query = iroha_lib::Query(
                    admin_keypair, nextQueryCounter(), config.admin_account_id)
                .getAccountDetail(account_id)
                .signAndAddSignature();
        return client->send(query);
    }

    QueryResponse getAccountTransactions(
            const std::string& account_id,
            uint32_t page_size = 10,
            const std::optional<std::string>& first_tx_hash = {},
            const std::optional<google::protobuf::Timestamp>& first_tx_time = {},
            const std::optional<google::protobuf::Timestamp>& last_tx_time = {},
            const std::optional<uint64_t> first_tx_height = {},
            const std::optional<uint64_t> last_tx_height = {},
            const std::optional<Ordering>& ordering = {})
    {
        return getAccountTransactions(account_id,
                AccountTransactionsParams{
                    page_size, first_tx_hash, first_tx_time, last_tx_time,
                    first_tx_height, last_tx_height, ordering});
    }

    QueryResponse getAccountTransactions(
            const std::string& account_id,
            const AccountTransactionsParams& params)
    {
        const auto query = iroha_lib::Query(
                    admin_keypair, nextQueryCounter(), config.admin_account_id)
                .getAccountTransactions(account_id,
                                        params.first_tx_hash,
                                        params.first_tx_time,
                                        params.last_tx_time,
                                        params.first_tx_height,
                                        params.last_tx_height,
                                        params.page_size,
                                        params.ordering)
                .signAndAddSignature();
        return client->send(query);
    }

    QueryResponse getAccountAssetTransactions(
            const std::string& account_id,
            const std::string& asset_id,
            const iroha::keypair_t& keypair,
            uint32_t page_size = 10,
            const std::optional<Ordering>& ordering = {})
    {
        const auto query = iroha_lib::Query(keypair, nextQueryCounter())
                .getAccountAssetTransactions(account_id,
                                             asset_id,
                                             {},
                                             {},
                                             {},
                                             {},
                                             {},
                                             page_size,
                                             ordering)
                .signAndAddSignature();
        return client->send(query);
    }
};

inline google::protobuf::Timestamp timestampFromMillis(uint64_t millis)
{
    google::protobuf::Timestamp ts;
    ts.set_seconds(static_cast<int64_t>(millis / 1000));
    ts.set_nanos(static_cast<int32_t>((millis % 1000) * 1000000));
    return ts;
}

inline std::string txSummary(const Transaction& tx)
{
    return "creator=" + tx.payload().reduced_payload().creator_account_id()
        + ", commands=[" + joinCommands(tx) + "]"
        + ", created_time=" + std::to_string(tx.payload().reduced_payload().created_time());
}

inline void printErrorIfAny(const QueryResponse& response, const char* what)
{
    if (response.has_error_response()) {
        std::cerr << what << " error: "
                  << response.error_response().message() << std::endl;
    }
}
}  // namespace iroha_example
