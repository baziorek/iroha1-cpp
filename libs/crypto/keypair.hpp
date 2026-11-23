/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CRYPTO_KEYPAIR_HPP
#define IROHA_CRYPTO_KEYPAIR_HPP

#include "common/blob.hpp"

namespace iroha {

  constexpr unsigned ed25519SignatureLengthBytes = 64;
  constexpr unsigned keyLenght = 32;

  using sig_t = blob_t<ed25519SignatureLengthBytes>;
  using pubkey_t = blob_t<keyLenght>;
  using privkey_t = blob_t<keyLenght>;

  struct keypair_t {
    keypair_t() = default;

    keypair_t(pubkey_t pubkey, privkey_t privkey)
        : pubkey(pubkey), privkey(privkey) {}

    pubkey_t pubkey;
    privkey_t privkey;
  };
}  // namespace iroha

#endif  // IROHA_CRYPTO_KEYPAIR_HPP
