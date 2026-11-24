/// @file QueryExample.cpp
/// @brief Runs the same command scenario as TxExample, then demonstrates
/// GetAccountTransactions queries: unfiltered, filtered by transaction
/// time range and by block height range. Requires a running Iroha node.
///
/// Connection settings come from environment variables (see common.hpp):
/// IROHA_HOST_ADDR, IROHA_PORT, ADMIN_ACCOUNT_ID, ADMIN_PRIVATE_KEY.

#include <iostream>

#include "scenario.hpp"


using iroha_example::Scenario;
using iroha_example::timestampFromMillis;
using iroha_example::txSummary;
using iroha_lib::QueryResponse;

const std::string kDetailKey = "age";
const std::string kDetailValue = "18";
const uint32_t kPageSize = 3;


void printTransactionsPage(const QueryResponse& response);


int main()
{
    try {
        Scenario scenario;
        scenario.init();

        const auto user_id = scenario.userAccountId();
        const auto asset_id = scenario.assetId();

        scenario.createDomainAndAsset(scenario.user_domain,
                                      scenario.asset_short_id);
        const auto created_time = scenario.addCoinToAdmin(asset_id);
        scenario.createAccount(scenario.user_account_name,
                               scenario.user_domain);
        scenario.transferCoin(scenario.config.admin_account_id,
                              user_id, asset_id);
        scenario.userGrantsToAdminSetAccountDetailPermission(user_id);
        scenario.setAccountDetail(user_id, kDetailKey, kDetailValue);

        std::cout << "--- query_transactions_simple (page_size="
                  << kPageSize << ") ---" << std::endl;
        printTransactionsPage(
                    scenario.getAccountTransactions(
                        scenario.config.admin_account_id, kPageSize));

        // query for txs in measured time range (+-1ms around created_time)
        std::cout << "--- transactions from time interval query ---"
                  << std::endl;
        printTransactionsPage(
                    scenario.getAccountTransactions(
                        scenario.config.admin_account_id,
                        kPageSize,
                        {},
                        timestampFromMillis(created_time - 1),
                        timestampFromMillis(created_time + 1)));

        // query for txs in given height range
        std::cout << "--- transactions from height range query ---"
                  << std::endl;
        printTransactionsPage(
                    scenario.getAccountTransactions(
                        scenario.config.admin_account_id,
                        kPageSize,
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
