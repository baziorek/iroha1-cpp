// Port of iroha-python examples/infinite-blocks-stream.py:
// wait for new blocks forever - send some commands to the node
// to see blocks coming. Stop with Ctrl+C.
//
// Node address and credentials are taken from environment variables:
//   IROHA_HOST_ADDR, IROHA_PORT, ADMIN_ACCOUNT_ID, ADMIN_PRIVATE_KEY

#include <csignal>
#include <iostream>

#include "common.hpp"
#include "scenario.hpp"


namespace {

using namespace iroha_example;

volatile std::sig_atomic_t g_stop = 0;

void onSignal(int)
{
    g_stop = 1;
}

}  // namespace


int main()
{
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    try {
        Scenario scenario;
        scenario.init();

        std::cout << "The script will wait for new blocks. "
                     "You need to send some commands to the node "
                     "to see any blocks coming" << std::endl;

        const auto blocks_query = iroha_lib::BlocksQuery(
                    scenario.config.admin_account_id,
                    scenario.admin_keypair)
                .signAndAddSignature();
        auto stream = scenario.client->sendBlocksStreamQuery(blocks_query);

        BlockQueryResponse response;
        while (not g_stop && stream->Read(&response)) {
            if (response.has_block_error_response()) {
                std::cerr << "Blocks stream error: "
                          << response.block_error_response().message()
                          << std::endl;
                break;
            }
            const auto& block = response.block_response().block();
            if (block.has_block_v1()) {
                const auto& payload = block.block_v1().payload();
                std::cout << "\nThe next block arrived: height="
                          << payload.height()
                          << ", tx_number=" << payload.tx_number()
                          << std::endl;
            }
        }
        stream->Cancel();
    } catch (const std::exception& e) {
        std::cerr << "[E] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
