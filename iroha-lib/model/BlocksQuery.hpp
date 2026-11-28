#pragma once

#include <chrono>
#include <cstdint>
#include <string>

#include "crypto/keypair.hpp"
#include "queries.pb.h"


namespace iroha_lib {

class BlocksQuery {
public:
    BlocksQuery(const std::string& account_id,
                const iroha::keypair_t& keypair,
                uint64_t counter = 1u,
                uint64_t created_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) noexcept;

    const iroha::protocol::BlocksQuery signAndAddSignature();

private:
    uint64_t counter_;
    uint64_t created_time_;
    iroha::protocol::BlocksQuery protobuf_blocks_query_;
    iroha::keypair_t keypair_;
};

}

