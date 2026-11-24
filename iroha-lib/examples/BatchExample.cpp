/// @file BatchExample.cpp
/// @brief Atomic exchange batch (MST batches): alice and bob, each with
/// quorum 2, swap assets atomically — alice proposes the batch, bob
/// accepts it (and a second exchange is declined on purpose to show
/// batch failure). MST must be enabled in the Iroha config.
/// Requires a running Iroha node.
///
/// Connection settings come from environment variables (see common.hpp):
/// IROHA_HOST_ADDR, IROHA_PORT, ADMIN_ACCOUNT_ID, ADMIN_PRIVATE_KEY.

#include <iostream>
#include <stdexcept>
#include <vector>

#include "common.hpp"

using namespace iroha_example;
using namespace iroha_lib;

Config config;
GrpcClient* client = nullptr;
iroha::keypair_t admin_keypair;
std::vector<iroha::keypair_t> alice_keys;
std::vector<iroha::keypair_t> bob_keys;

const std::string kDomain = "test";
const std::string kAliceName = "alice";
const std::string kBobName = "bob";
const std::string kAliceAccount = kAliceName + "@" + kDomain;
const std::string kBobAccount = kBobName + "@" + kDomain;
const std::string kBitcoinName = "bitcoin";
const std::string kDogeName = "dogecoin";
const std::string kBitcoin = kBitcoinName + "#" + kDomain;
const std::string kDoge = kDogeName + "#" + kDomain;

// test-only keys, same as in the Python example
const std::string kAlicePrivateKey1 =
    "f101537e319568c765b2cc89698325604991dca57b9716b58016b253506caba1";
const std::string kAlicePrivateKey2 =
    "f101537e319568c765b2cc89698325604991dca57b9716b58016b253506caba2";
const std::string kBobPrivateKey1 =
    "f101537e319568c765b2cc89698325604991dca57b9716b58016b253506caba3";
const std::string kBobPrivateKey2 =
    "f101537e319568c765b2cc89698325604991dca57b9716b58016b253506caba4";


void prepareUsers();

void addKeyAndSetQuorum(
        const std::string& account_id,
        const iroha::keypair_t& account_key,
        const iroha::keypair_t& key_to_add);

void addKeysAndSetQuorum();

std::vector<Transaction> fetchPendingTransactions(
        const iroha::keypair_t& keypair,
        const std::string& account_id);

void aliceCreatesExchangeBatch();

void bobAcceptsExchangeRequest();

void bobDeclinesExchangeRequest();

void checkNoPendingTxs(
        const std::string& account_id,
        const iroha::keypair_t& keypair);


int main()
{
    std::cout << "\nPLEASE ENSURE THAT MST IS ENABLED IN IROHA CONFIG\n\n";

    try {
        config = loadConfig();
        admin_keypair = generateKeypairFromPrivateKey(config.admin_private_key);

        alice_keys = {
            generateKeypairFromPrivateKey(kAlicePrivateKey1),
            generateKeypairFromPrivateKey(kAlicePrivateKey2)};
        bob_keys = {
            generateKeypairFromPrivateKey(kBobPrivateKey1),
            generateKeypairFromPrivateKey(kBobPrivateKey2)};

        auto grpc_client_holder = makeClient(config);
        GrpcClient& grpc_client = *grpc_client_holder;
        client = &grpc_client;

        prepareUsers();
        addKeysAndSetQuorum();

        aliceCreatesExchangeBatch();
        bobAcceptsExchangeRequest();
        checkNoPendingTxs(kBobAccount, bob_keys[0]);

        aliceCreatesExchangeBatch();
        try {
            bobDeclinesExchangeRequest();  // the batch is expected to fail
        } catch (const std::runtime_error&) {
        }
        checkNoPendingTxs(kBobAccount, bob_keys[0]);
    } catch (const std::exception& e) {
        std::cerr << "[E] " << e.what() << std::endl;
        return 1;
    }
}


void prepareUsers()
{
    auto tx = makeTx(config.admin_account_id, admin_keypair)
            .createAsset(kBitcoinName, kDomain, 2)
            .createAsset(kDogeName, kDomain, 2)
            .addAssetQuantity(kBitcoin, "100000", "")
            .addAssetQuantity(kDoge, "20000", "")
            .createAccount(kAliceName, kDomain,
                           alice_keys[0].pubkey.to_hexstring())
            .createAccount(kBobName, kDomain,
                           bob_keys[0].pubkey.to_hexstring())
            .transferAsset(config.admin_account_id, kAliceAccount, kBitcoin,
                           "init top up", "100000")
            .transferAsset(config.admin_account_id, kBobAccount, kDoge,
                           "init doge", "20000")
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*client, tx);
}

void addKeyAndSetQuorum(
        const std::string& account_id,
        const iroha::keypair_t& account_key,
        const iroha::keypair_t& key_to_add)
{
    auto tx = makeTx(account_id, account_key)
            .addSignatory(account_id, key_to_add.pubkey.to_hexstring())
            .setAccountQuorum(account_id, 2)
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*client, tx);
}

void addKeysAndSetQuorum()
{
    addKeyAndSetQuorum(kAliceAccount, alice_keys[0], alice_keys[1]);
    addKeyAndSetQuorum(kBobAccount, bob_keys[0], bob_keys[1]);
}

std::vector<Transaction> fetchPendingTransactions(
        const iroha::keypair_t& keypair,
        const std::string& account_id)
{
    const auto query = iroha_lib::Query(keypair, nextQueryCounter())
            .getPendingTransactions(account_id)
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

void aliceCreatesExchangeBatch()
{
    // unsigned transactions - signatures are added after the batch meta
    iroha_lib::Tx alice_tx_builder = makeTx(kAliceAccount, alice_keys[0], 2);
    alice_tx_builder.transferAsset(kAliceAccount, kBobAccount, kBitcoin,
                                   "", "1");
    iroha_lib::Tx bob_tx_builder = makeTx(kBobAccount, bob_keys[0]);
    bob_tx_builder.transferAsset(kBobAccount, kAliceAccount, kDoge,
                                 "", "2");

    std::vector<Transaction> transactions{
        alice_tx_builder.getTransaction(),
        bob_tx_builder.getTransaction()};

    // atomic batch meta must be set before signing (it is part of the payload)
    TxBatch tx_batch;
    tx_batch.batch(transactions, true);

    // alice signs with all her keys (quorum = 2); bob's tx is left unsigned
    // - it will be signed by bob when he accepts the exchange
    for (const auto& key : alice_keys) {
        addSignature(transactions[0], key);
    }

    sendBatchAndPrintStatus(*client, transactions);
}

void bobAcceptsExchangeRequest()
{
    auto transactions = fetchPendingTransactions(bob_keys[0], kBobAccount);

    for (auto& tx : transactions) {
        if (tx.payload().reduced_payload().creator_account_id() == kAliceAccount) {
            // signatures of the initiator are cleared so that the batch
            // reaches the MST engine with only bob's signatures
            tx.mutable_signatures()->Clear();
        } else {
            for (const auto& key : bob_keys) {
                addSignature(tx, key);
            }
        }
    }

    sendBatchAndPrintStatus(*client, transactions);
}

void bobDeclinesExchangeRequest()
{
    std::cout << "\n"
              << "IT IS EXPECTED HERE THAT THE BATCH WILL FAIL STATEFUL VALIDATION"
              << "\n" << std::endl;

    auto transactions = fetchPendingTransactions(bob_keys[0], kBobAccount);

    for (auto& tx : transactions) {
        if (tx.payload().reduced_payload().creator_account_id() == kAliceAccount) {
            tx.mutable_signatures()->Clear();
        } else {
            // intentionally alice's keys are used to fail bob's txs;
            // zeroed private keys would be acceptable as well
            for (const auto& key : alice_keys) {
                addSignature(tx, key);
            }
        }
    }

    sendBatchAndPrintStatus(*client, transactions);
}

void checkNoPendingTxs(
        const std::string& account_id,
        const iroha::keypair_t& keypair)
{
    std::cout << " ~~~ No pending txs expected:" << std::endl;
    try {
        const auto query = iroha_lib::Query(keypair, nextQueryCounter())
                .getPendingTransactions(account_id)
                .signAndAddSignature();
        const auto response = client->send(query);
        std::cout << response.DebugString() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "query failed: " << e.what() << std::endl;
    }
    std::cout << " ~~~" << std::endl;
}
