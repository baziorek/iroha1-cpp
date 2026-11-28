#pragma once

#include "transaction.pb.h"
#include <endpoint.pb.h>
#include "cryptography/ed25519_sha3_impl/internal/sha3_hash.hpp"


namespace iroha_lib {

using namespace iroha::protocol;
using iroha::protocol::Transaction_Payload_BatchMeta_BatchType;

class TxBatch {

public:
    Transaction_Payload_BatchMeta_BatchType getBatchType(bool atomic) const
    {
        return atomic ? Transaction_Payload_BatchMeta_BatchType_ATOMIC
                      : Transaction_Payload_BatchMeta_BatchType_ORDERED;
    }

    // Sets batch meta (type + reduced hashes of all txs) on every
    // transaction. Must be called before signing - the meta is part of
    // the signed payload.
    TxList batch(std::vector<Transaction>& transactions, bool atomic = true)
    {
        TxList tx_list;

        Transaction::Payload::BatchMeta meta;
        meta.set_type(getBatchType(atomic));
        for (const auto& tx : transactions) {
            *meta.add_reduced_hashes() = iroha::sha3_256(
                        tx.payload().reduced_payload().SerializeAsString())
                    .to_hexstring();
        }

        for (auto& tx : transactions) {
            tx.mutable_payload()->mutable_batch()->CopyFrom(meta);
            *tx_list.add_transactions() = tx;
        }
        return tx_list;
    }
};

}

