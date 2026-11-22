// Port of iroha-python examples/tx-example.py:
// CreateDomain+CreateAsset, AddAssetQuantity, CreateAccount, TransferAsset,
// GrantPermission, SetAccountDetail + GetAssetInfo/GetAccountAssets/
// GetAccountDetail queries.
//
// Node address and credentials are taken from environment variables:
//   IROHA_HOST_ADDR, IROHA_PORT, ADMIN_ACCOUNT_ID, ADMIN_PRIVATE_KEY
// (same names and defaults as the Python examples).

#include <iostream>

#include "scenario.hpp"


int main()
{
    try {
        iroha_example::Scenario scenario;
        scenario.init();

        scenario.createDomainAndAsset("domain", "coin");
        scenario.addCoinToAdmin("coin#domain");
        scenario.createAccount("userone", "domain");
        scenario.transferCoin(scenario.config.admin_account_id,
                              "userone@domain", "coin#domain");
        scenario.userGrantsToAdminSetAccountDetailPermission("userone@domain");
        scenario.setAccountDetail("userone@domain", "age", "18");

        const auto asset_info = scenario.getAssetInfo("coin#domain");
        iroha_example::printErrorIfAny(asset_info, "GetAssetInfo");
        if (asset_info.has_asset_response()) {
            const auto& data = asset_info.asset_response().asset();
            std::cout << "Asset id = " << data.asset_id()
                      << ", precision = " << data.precision() << std::endl;
        }

        const auto account_assets = scenario.getAccountAssets("userone@domain");
        iroha_example::printErrorIfAny(account_assets, "GetAccountAssets");
        if (account_assets.has_account_assets_response()) {
            for (const auto& asset :
                 account_assets.account_assets_response().account_assets()) {
                std::cout << "Asset id = " << asset.asset_id()
                          << ", balance = " << asset.balance() << std::endl;
            }
        }

        const auto account_detail = scenario.getAccountDetail("userone@domain");
        iroha_example::printErrorIfAny(account_detail, "GetAccountDetail");
        if (account_detail.has_account_detail_response()) {
            std::cout << "Account id = userone@domain"
                      << ", details = "
                      << account_detail.account_detail_response().detail()
                      << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[E] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
