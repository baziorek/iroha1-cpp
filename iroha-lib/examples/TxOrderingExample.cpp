// Port of iroha-python examples/tx-ordering.py:
// pagination with ordering - GetAccountAssetTransactions in ascending
// and descending order.
//
// Node address and credentials are taken from environment variables:
//   IROHA_HOST_ADDR, IROHA_PORT, ADMIN_ACCOUNT_ID, ADMIN_PRIVATE_KEY

#include <chrono>
#include <iostream>
#include <thread>

#include "scenario.hpp"


namespace {

using namespace iroha_example;

const std::string kUserAccount = "aa@test";
const std::string kUserPrivateKey =
        "1234567890123456789012345678901234567890123456789012345678901234";
const std::string kAssetId = "lemurcoin#test";

void createAccount(Scenario& scenario,
                   const iroha::keypair_t& user_keypair);

void createAsset(Scenario& scenario);

void addCoinToAdmin(Scenario& scenario);

void performTransferTransaction(
        Scenario& scenario,
        const std::string& description,
        int amount);

Ordering makeOrdering(Direction direction);

void getAccountAssetTransactions(
        Scenario& scenario,
        const iroha::keypair_t& user_keypair,
        Direction direction);

void printParagraph(const std::string& text);
}  // namespace


int main()
{
    try {
        const auto user_keypair =
                generateKeypairFromPrivateKey(kUserPrivateKey);

        Scenario scenario;
        scenario.init();

        printParagraph("Preparing");
        createAccount(scenario, user_keypair);
        createAsset(scenario);
        addCoinToAdmin(scenario);

        printParagraph("Creating transactions");
        for (int i = 1; i <= 10; ++i) {
            performTransferTransaction(
                        scenario, "tx " + std::to_string(i), i);
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));

        printParagraph("Querying as usual");
        getAccountAssetTransactions(scenario, user_keypair, Direction::kAscending);

        printParagraph("Querying as with reversed order");
        getAccountAssetTransactions(scenario, user_keypair, Direction::kDescending);
    } catch (const std::exception& e) {
        std::cerr << "[E] " << e.what() << std::endl;
        return 1;
    }
}


namespace {

void createAccount(Scenario& scenario,
                   const iroha::keypair_t& user_keypair)
{
    auto tx = makeTx(scenario.config.admin_account_id,
                     scenario.admin_keypair)
            .createAccount("aa", "test",
                           user_keypair.pubkey.to_hexstring())
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*scenario.client, tx);
}

void createAsset(Scenario& scenario)
{
    auto tx = makeTx(scenario.config.admin_account_id,
                     scenario.admin_keypair)
            .createAsset("lemurcoin", "test", 2)
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*scenario.client, tx);
}

void addCoinToAdmin(Scenario& scenario)
{
    auto tx = makeTx(scenario.config.admin_account_id,
                     scenario.admin_keypair)
            .addAssetQuantity(kAssetId, "10000", "")
            .signAndAddSignature();
    sendTransactionAndPrintStatus(*scenario.client, tx);
}

void performTransferTransaction(
        Scenario& scenario,
        const std::string& description,
        int amount)
{
    auto tx = makeTx(scenario.config.admin_account_id,
                     scenario.admin_keypair)
            .transferAsset(scenario.config.admin_account_id,
                           kUserAccount,
                           kAssetId,
                           description,
                           std::to_string(amount))
            .signAndAddSignature();
    std::cout << "------------> Transaction: \"" << description
              << "\" ------------>" << std::endl;
    sendTransactionAndPrintStatus(*scenario.client, tx);
}

Ordering makeOrdering(Direction direction)
{
    Ordering ordering;
    auto* field_ordering = ordering.add_sequence();
    field_ordering->set_field(Field::kCreatedTime);
    field_ordering->set_direction(direction);
    return ordering;
}

void getAccountAssetTransactions(
        Scenario& scenario,
        const iroha::keypair_t& user_keypair,
        Direction direction)
{
    const auto response = scenario.getAccountAssetTransactions(
                kUserAccount,
                kAssetId,
                user_keypair,
                3,
                makeOrdering(direction));

    if (response.has_error_response()) {
        std::cerr << "GetAccountAssetTransactions error: "
                  << response.error_response().message() << std::endl;
        return;
    }
    if (not response.has_transactions_page_response()) {
        std::cerr << "Unexpected response type" << std::endl;
        return;
    }

    for (const auto& tx : response.transactions_page_response().transactions()) {
        const auto& payload = tx.payload().reduced_payload();
        for (const auto& command : payload.commands()) {
            if (command.has_transfer_asset()) {
                const auto& transfer = command.transfer_asset();
                std::cout << "Transaction: " << transfer.src_account_id()
                          << " -> " << transfer.dest_account_id() << ", "
                          << transfer.asset_id() << "*" << transfer.amount()
                          << ": \"" << transfer.description() << "\". "
                          << "Created: " << payload.created_time() << std::endl;
            }
        }
    }
}

void printParagraph(const std::string& text)
{
    std::cout << "---------- " << text << " ----------" << std::endl;
}
}  // namespace
