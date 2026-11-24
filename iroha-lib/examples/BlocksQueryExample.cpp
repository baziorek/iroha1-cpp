/// @file BlocksQueryExample.cpp
/// @brief Subscribes to the blocks stream (BlocksQuery) and then sends
/// transactions, printing the blocks they get committed in. Requires a
/// running Iroha node.
///
/// Connection settings come from environment variables (see common.hpp):
/// IROHA_HOST_ADDR, IROHA_PORT, ADMIN_ACCOUNT_ID, ADMIN_PRIVATE_KEY.

#include <chrono>
#include <iostream>
#include <thread>

#include "common.hpp"
#include "scenario.hpp"

using namespace iroha_example;
using namespace iroha_lib;

const std::string kUserName1 = "blockuser1";
const std::string kUserName2 = "blockuser2";


void sendCreateAccountTransaction(
        Scenario& scenario,
        const iroha::keypair_t& user_keypair,
        const std::string& account_name);

void printBlock(const BlockQueryResponse& response);


int main()
{
    try {
        Scenario scenario;
        scenario.init();

        const auto blocks_query = iroha_lib::BlocksQuery(
                    scenario.config.admin_account_id,
                    scenario.admin_keypair)
                .signAndAddSignature();
        auto stream = scenario.client->sendBlocksStreamQuery(blocks_query);

        // give the node a moment to subscribe to the block stream
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        const auto user_keypair = generateRandomKeypair();

        sendCreateAccountTransaction(scenario, user_keypair, kUserName1);

        BlockQueryResponse response;
        if (stream->Read(&response)) {
            printBlock(response);
        }

        sendCreateAccountTransaction(scenario, user_keypair, kUserName2);

        if (stream->Read(&response)) {
            printBlock(response);
        }

        stream->Cancel();
    } catch (const std::exception& e) {
        std::cerr << "[E] " << e.what() << std::endl;
        return 1;
    }
}


void sendCreateAccountTransaction(
        Scenario& scenario,
        const iroha::keypair_t& user_keypair,
        const std::string& account_name)
{
    const auto domain = splitAccountId(
                scenario.config.admin_account_id).second;
    auto tx = makeTx(scenario.config.admin_account_id,
                     scenario.admin_keypair)
            .createAccount(account_name, domain,
                           user_keypair.pubkey.to_hexstring())
            .signAndAddSignature();
    scenario.client->send(tx);
    std::cout << "tx \"CreateAccount\" is sent" << std::endl;
}

void printBlock(const BlockQueryResponse& response)
{
    if (response.has_block_error_response()) {
        std::cerr << "Blocks stream error: "
                  << response.block_error_response().message() << std::endl;
        return;
    }
    const auto& block = response.block_response().block();
    if (block.has_block_v1()) {
        const auto& payload = block.block_v1().payload();
        std::cout << "\nNext block: height=" << payload.height()
                  << ", tx_number=" << payload.tx_number()
                  << ", created_time=" << payload.created_time()
                  << std::endl;
    } else {
        std::cout << "\nNext block: " << block.DebugString() << std::endl;
    }
}
