/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_UTIL_HPP
#define IROHA_SHARED_MODEL_PROTO_UTIL_HPP

#include <google/protobuf/message.h>
#include <stdexcept>
#include <vector>
#include "cryptography/blob.hpp"

namespace shared_model {
  namespace proto {

    template <typename T>
    crypto::Blob makeBlob(T &&message) {
      crypto::Blob::Bytes data;
      data.resize(message.ByteSizeLong());
      if (not message.SerializeToArray(data.data(), data.size())) {
        throw std::runtime_error(
            "failed to serialize protobuf message to blob");
      }
      return crypto::Blob(std::move(data));
    }

  }  // namespace proto
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_UTIL_HPP
