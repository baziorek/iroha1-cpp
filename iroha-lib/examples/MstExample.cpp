// Port of iroha-python examples/mst-example.py:
// multi-signature transaction (MST) - a "group" account with three
// signatories (admin, alice, bob) and quorum 2. Alice initiates a
// transfer from the group account; the transaction stays pending until
// bob countersigns it.
//
// Node address and credentials are taken from environment variables:
//   IROHA_HOST_ADDR, IROHA_PORT, ADMIN_ACCOUNT_ID, ADMIN_PRIVATE_KEY
//
// NOTE: MST must be enabled in the Iroha config (mst_support), same as
// for the Python example.

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "common.hpp"


namespace {

using namespace iroha_example;
using namespace iroha_lib;

Config config;
GrpcClient* client = nullptr;
iroha::keypair_t admin_keypair;

const std::string kGroupAccount = "group@test";
const std::string kAliceAccount = "alice@test";
const std::string kBobAccount = "bob@test";
const std::string kReceiverAccount = "receiver@test";
const std::string kDomain = "test";
const std::string kCoin = "coin#test";

// test-only keys, same as in the Python example
const std::string kGroupPrivateKey =
    "f101537e319568c765b2cc89698325604991dca57b9716b58016b253506caba1";
const std::string kAlicePrivateKey =
    "f101537e319568c765b2cc89698325604991dca57b9716b58016b253506caba2";
const std::string kBobPrivateKey =
    "f101537e319568c765b2cc89698325604991dca57b9716b58016b253506caba3";
const std::string kReceiverPrivateKey =
    "f101537e319568c765b2cc89698325604991dca57b9716b58016b253506caba4";

void createUserAccounts()
{
    auto tx = makeTx(config.admin_account_id, admin_keypair)
            .createAccount("alice", kDomain,
                           getPublicKeyHexFromPrivateKey(kAlicePrivateKey))
            .createAccount("bob", kDomain,
                           getPublicKeyHexFromPrivateKey(kBobPrivateKey))
            .createAccount("group", kDomain,
                           getPublicKeyHexFromPrivateKey(kGroupPrivateKey))
            .createAccount("receiver", kDomain,
                           getPublicKeyHexFromPrivateKey(kReceiverPrivateKey))
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*client, tx);
}

void setupGroupAccount()
{
    // the transaction creator is the group account itself and it is
    // signed by the group's own key (the only signatory at this point)
    const auto group_keypair = generateKeypairFromPrivateKey(kGroupPrivateKey);
    auto tx = makeTx(kGroupAccount, group_keypair)
            .addSignatory(kGroupAccount, admin_keypair.pubkey.to_hexstring())
            .addSignatory(kGroupAccount,
                          getPublicKeyHexFromPrivateKey(kAlicePrivateKey))
            .addSignatory(kGroupAccount,
                          getPublicKeyHexFromPrivateKey(kBobPrivateKey))
            .grantPermission(config.admin_account_id,
                             GrantablePermission::can_set_my_quorum)
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*client, tx);
}

void mintAsset()
{
    // the Python example assumes coin#test already exists in the
    // development environment; here we create it explicitly so the
    // example also works on a fresh chain
    auto tx = makeTx(config.admin_account_id, admin_keypair)
            .createAsset("coin", kDomain, 2)
            .addAssetQuantity(kCoin, "1000.00", "")
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*client, tx);
}

void transferCoinFromAdmin(
        const std::string& dest_id,
        const std::string& asset_id,
        const std::string& amount)
{
    auto tx = makeTx(config.admin_account_id, admin_keypair)
            .transferAsset(config.admin_account_id, dest_id, asset_id,
                           "init top up", amount)
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*client, tx);
}

void transferCoinFromGroup(
        const std::string& dest_id,
        const std::string& asset_id,
        const std::string& amount,
        const iroha::keypair_t& creator_keypair)
{
    // the transaction creator is the group account, but it is signed by
    // one of its signatories; quorum 2 keeps it pending until a second
    // signatory countersigns
    auto tx = makeTx(kGroupAccount, creator_keypair, 2)
            .transferAsset(kGroupAccount, dest_id, asset_id,
                           "transfer", amount)
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*client, tx);
}

std::vector<Transaction> fetchPendingTransactions(
        const iroha::keypair_t& keypair)
{
    // creator of the query is the group account, signed by a key that
    // is one of its signatories (same as Iroha(group).query() signed
    // with the countersigning key in the Python example)
    const auto query = iroha_lib::Query(keypair, nextQueryCounter())
            .getPendingTransactions(kGroupAccount)
            .signAndAddSignature();

    const auto response = client->send(query);
    if (response.has_error_response()) {
        throw std::runtime_error(
                    "GetPendingTransactions error: "
                    + response.error_response().message());
    }

    std::vector<Transaction> transactions;
    for (const auto& tx : response.transactions_response().transactions()) {
        transactions.push_back(tx);
    }
    return transactions;
}

void printAccountAssets(const std::string& account_id)
{
    const auto query = iroha_lib::Query(
                admin_keypair, nextQueryCounter(), config.admin_account_id)
            .getAccountAssets(account_id)
            .signAndAddSignature();
    const auto response = client->send(query);
    if (response.has_error_response()) {
        throw std::runtime_error(
                    "GetAccountAssets error: "
                    + response.error_response().message());
    }

    std::cout << "Account = " << account_id << std::endl;
    for (const auto& asset : response.account_assets_response().account_assets()) {
        std::cout << "Asset id = " << asset.asset_id()
                  << ", balance = " << asset.balance() << std::endl;
    }
}

std::string getBalance(const std::string& account_id, const std::string& asset_id)
{
    const auto query = iroha_lib::Query(
                admin_keypair, nextQueryCounter(), config.admin_account_id)
            .getAccountAssets(account_id)
            .signAndAddSignature();
    const auto response = client->send(query);
    if (response.has_error_response()) {
        throw std::runtime_error(
                    "GetAccountAssets error: "
                    + response.error_response().message());
    }
    for (const auto& asset : response.account_assets_response().account_assets()) {
        if (asset.asset_id() == asset_id) {
            return asset.balance();
        }
    }
    return "";
}

void signPendingTransactions(const iroha::keypair_t& keypair)
{
    auto transactions = fetchPendingTransactions(keypair);
    std::cout << "pending transactions: " << transactions.size() << std::endl;

    for (auto& tx : transactions) {
        const auto& creator =
                tx.payload().reduced_payload().creator_account_id();
        std::cout << "creator: " << creator << std::endl;
        if (creator == kGroupAccount) {
            // signatures are cleared so that the transaction reaches the
            // MST engine with only the new signature; the pending
            // transaction storage keeps the signatures collected so far
            tx.mutable_signatures()->Clear();
            addSignature(tx, keypair);
            sendTransactionAndPrintStatus(*client, tx);
        }
    }
}

void expectBalance(
        const std::string& account_id,
        const std::string& asset_id,
        const std::string& expected_balance)
{
    const auto balance = getBalance(account_id, asset_id);
    if (balance != expected_balance) {
        throw std::runtime_error(
                    "unexpected balance of " + account_id + " in " + asset_id
                    + ": got '" + balance
                    + "', expected '" + expected_balance + "'");
    }
}

}  // namespace


int main()
{
    std::cout << "\nPLEASE ENSURE THAT MST IS ENABLED IN IROHA CONFIG\n\n";

    try {
        config = loadConfig();
        admin_keypair = generateKeypairFromPrivateKey(config.admin_private_key);

        const auto alice_keypair = generateKeypairFromPrivateKey(kAlicePrivateKey);
        const auto bob_keypair = generateKeypairFromPrivateKey(kBobPrivateKey);

        auto grpc_client_holder = makeClient(config);
        GrpcClient& grpc_client = *grpc_client_holder;
        client = &grpc_client;

        std::cout << "1. creating accounts" << std::endl;
        createUserAccounts();
        setupGroupAccount();

        std::cout << "2. mint coins and give to group account" << std::endl;
        mintAsset();
        transferCoinFromAdmin(kGroupAccount, kCoin, "42.00");

        std::cout << "3. alice initiates transfer 14.0 from group to receiver"
                  << std::endl;
        transferCoinFromGroup(kReceiverAccount, kCoin, "14.00", alice_keypair);

        std::cout << "4. bob countersigns transfer" << std::endl;
        signPendingTransactions(bob_keypair);

        printAccountAssets(kReceiverAccount);
        printAccountAssets(kGroupAccount);
        expectBalance(kReceiverAccount, kCoin, "14.00");

        std::cout << "5. bob initiates transfer 7.0 from group to receiver"
                  << std::endl;
        transferCoinFromGroup(kReceiverAccount, kCoin, "7.00", bob_keypair);

        std::cout << "6. alice countersigns transfer" << std::endl;
        signPendingTransactions(alice_keypair);

        printAccountAssets(kReceiverAccount);
        printAccountAssets(kGroupAccount);
        expectBalance(kReceiverAccount, kCoin, "21.00");
        expectBalance(kGroupAccount, kCoin, "21.00");
    } catch (const std::exception& e) {
        std::cerr << "[E] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
