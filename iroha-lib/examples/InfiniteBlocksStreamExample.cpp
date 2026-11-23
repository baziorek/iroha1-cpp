// Port of iroha-python examples/infinite-blocks-stream.py:
// wait for new blocks forever - send some commands to the node
// to see blocks coming. Stop with Ctrl+C.
//
// Node address and credentials are taken from environment variables:
//   IROHA_HOST_ADDR, IROHA_PORT, ADMIN_ACCOUNT_ID, ADMIN_PRIVATE_KEY

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

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
    // only SIGINT (Ctrl+C) is handled gracefully; SIGTERM is left at
    // its default disposition so that `timeout`/`kill` can terminate
    // the process even while it is blocked in stream->Read()
    std::signal(SIGINT, onSignal);

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

        // stream->Read() cannot be interrupted by a signal handler, so a
        // watcher thread cancels the stream once SIGINT was received
        std::atomic<bool> watcher_done{false};
        std::thread watcher([&] {
            while (not g_stop and not watcher_done) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            if (g_stop) {
                stream->Cancel();
            }
        });

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
        watcher_done = true;
        watcher.join();
        stream->Cancel();
    } catch (const std::exception& e) {
        std::cerr << "[E] " << e.what() << std::endl;
        return 1;
    }
}
