#include "BlocksQuery.hpp"

#include "cryptography/ed25519_sha3_impl/internal/ed25519_impl.hpp"
#include "cryptography/ed25519_sha3_impl/internal/sha3_hash.hpp"


namespace iroha_lib {

BlocksQuery::BlocksQuery(
        const std::string& account_id,
        const iroha::keypair_t& keypair,
        uint64_t counter,
        uint64_t created_time) noexcept
    : counter_(counter),
      created_time_(created_time),
      keypair_(keypair)
{
    auto* meta = protobuf_blocks_query_.mutable_meta();
    meta->set_creator_account_id(account_id);
    meta->set_query_counter(counter_);
    meta->set_created_time(created_time_);
}

const iroha::protocol::BlocksQuery BlocksQuery::signAndAddSignature()
{
    // BlocksQuery is signed over its meta payload only (see
    // shared_model::proto::BlocksQuery::payload()).
    const auto hash = iroha::sha3_256(
                protobuf_blocks_query_.meta().SerializeAsString());
    const auto signature = iroha::sign(
                hash.to_string(),
                keypair_.pubkey,
                keypair_.privkey);

    auto* sig = protobuf_blocks_query_.mutable_signature();
    sig->set_signature(signature.to_hexstring());
    sig->set_public_key(keypair_.pubkey.to_hexstring());
    return protobuf_blocks_query_;
}

}
