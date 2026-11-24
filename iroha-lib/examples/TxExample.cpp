/// @file TxExample.cpp
/// @brief The main transaction example: creates a domain and an asset,
/// tops up the admin balance, registers an account, transfers coins and
/// queries account/asset state back. Requires a running Iroha node.
///
/// Connection settings come from environment variables (see common.hpp):
/// IROHA_HOST_ADDR, IROHA_PORT, ADMIN_ACCOUNT_ID, ADMIN_PRIVATE_KEY.

#include <iostream>

#include "scenario.hpp"


const std::string kDetailKey = "age";
const std::string kDetailValue = "18";


int main()
{
    try {
        iroha_example::Scenario scenario;
        scenario.init();

        const auto user_id = scenario.userAccountId();
        const auto asset_id = scenario.assetId();

        scenario.createDomainAndAsset(scenario.user_domain,
                                      scenario.asset_short_id);
        scenario.addCoinToAdmin(asset_id);
        scenario.createAccount(scenario.user_account_name,
                               scenario.user_domain);
        scenario.transferCoin(scenario.config.admin_account_id,
                              user_id, asset_id);
        scenario.userGrantsToAdminSetAccountDetailPermission(user_id);
        scenario.setAccountDetail(user_id, kDetailKey, kDetailValue);

        const auto asset_info = scenario.getAssetInfo(asset_id);
        iroha_example::printErrorIfAny(asset_info, "GetAssetInfo");
        if (asset_info.has_asset_response()) {
            const auto& data = asset_info.asset_response().asset();
            std::cout << "Asset id = " << data.asset_id()
                      << ", precision = " << data.precision() << std::endl;
        }

        const auto account_assets = scenario.getAccountAssets(user_id);
        iroha_example::printErrorIfAny(account_assets, "GetAccountAssets");
        if (account_assets.has_account_assets_response()) {
            for (const auto& asset :
                 account_assets.account_assets_response().account_assets()) {
                std::cout << "Asset id = " << asset.asset_id()
                          << ", balance = " << asset.balance() << std::endl;
            }
        }

        const auto account_detail = scenario.getAccountDetail(user_id);
        iroha_example::printErrorIfAny(account_detail, "GetAccountDetail");
        if (account_detail.has_account_detail_response()) {
            std::cout << "Account id = " << user_id
                      << ", details = "
                      << account_detail.account_detail_response().detail()
                      << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[E] " << e.what() << std::endl;
        return 1;
    }
}
