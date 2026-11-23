// Port of iroha-python examples/query-transactions.py:
// same command scenario as tx-example.py, plus GetAccountTransactions
// queries - simple, filtered by tx_time range and by tx_height range.
//
// Node address and credentials are taken from environment variables:
//   IROHA_HOST_ADDR, IROHA_PORT, ADMIN_ACCOUNT_ID, ADMIN_PRIVATE_KEY

#include <iostream>

#include "scenario.hpp"


namespace {
using namespace iroha_example;

void printTransactionsPage(const QueryResponse& response)
{
    if (response.has_error_response()) {
        std::cerr << "GetAccountTransactions error: "
                  << response.error_response().message() << std::endl;
        return;
    }
    if (not response.has_transactions_page_response()) {
        std::cerr << "Unexpected response type" << std::endl;
        return;
    }
    const auto& page = response.transactions_page_response();
    std::cout << "all_transactions_size = " << page.all_transactions_size()
              << ", returned = " << page.transactions_size() << std::endl;
    for (const auto& tx : page.transactions()) {
        std::cout << "  " << txSummary(tx) << std::endl;
    }
}
}  // namespace


int main()
{
    try {
        Scenario scenario;
        scenario.init();

        scenario.createDomainAndAsset("domain", "coin");
        const auto created_time = scenario.addCoinToAdmin("coin#domain");
        scenario.createAccount("userone", "domain");
        scenario.transferCoin(scenario.config.admin_account_id,
                              "userone@domain", "coin#domain");
        scenario.userGrantsToAdminSetAccountDetailPermission("userone@domain");
        scenario.setAccountDetail("userone@domain", "age", "18");

        std::cout << "--- query_transactions_simple (page_size=3) ---"
                  << std::endl;
        printTransactionsPage(
                    scenario.getAccountTransactions(
                        scenario.config.admin_account_id, 3));

        // query for txs in measured time range (+-1ms around created_time)
        std::cout << "--- transactions from time interval query ---"
                  << std::endl;
        printTransactionsPage(
                    scenario.getAccountTransactions(
                        scenario.config.admin_account_id,
                        3,
                        {},
                        timestampFromMillis(created_time - 1),
                        timestampFromMillis(created_time + 1)));

        // query for txs in given height range
        std::cout << "--- transactions from height range query ---"
                  << std::endl;
        printTransactionsPage(
                    scenario.getAccountTransactions(
                        scenario.config.admin_account_id,
                        3,
                        {},
                        {},
                        {},
                        2,
                        3));

        std::cout << "done" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[E] " << e.what() << std::endl;
        return 1;
    }
}
