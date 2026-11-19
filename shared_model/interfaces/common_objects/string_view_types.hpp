/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_STRING_VIEW_TYPES_HPP
#define IROHA_SHARED_MODEL_STRING_VIEW_TYPES_HPP

#include <string_view>

#include <boost/serialization/strong_typedef.hpp>
#include "interfaces/common_objects/byte_range.hpp"

namespace shared_model {
  namespace interface {
    namespace types {
      BOOST_STRONG_TYPEDEF(std::string_view, SignedHexStringView)
      BOOST_STRONG_TYPEDEF(ByteRange, SignatureByteRangeView)

      BOOST_STRONG_TYPEDEF(std::string_view, PublicKeyHexStringView)
      BOOST_STRONG_TYPEDEF(ByteRange, PublicKeyByteRangeView)

      BOOST_STRONG_TYPEDEF(std::string_view, EvmCalleeHexStringView)
      BOOST_STRONG_TYPEDEF(std::string_view, EvmCodeHexStringView)

      // C++20: boost::totally_ordered2 only provides operator==(base, D),
      // whose body delegates to the missing operator==(D, base) and recurses
      // infinitely. Provide the forward comparison explicitly.
      inline bool operator==(const SignedHexStringView &lhs,
                             const std::string_view &rhs) {
        return lhs.t == rhs;
      }
      inline bool operator==(const SignatureByteRangeView &lhs,
                             const ByteRange &rhs) {
        return lhs.t == rhs;
      }
      inline bool operator==(const PublicKeyHexStringView &lhs,
                             const std::string_view &rhs) {
        return lhs.t == rhs;
      }
      inline bool operator==(const PublicKeyByteRangeView &lhs,
                             const ByteRange &rhs) {
        return lhs.t == rhs;
      }
      inline bool operator==(const EvmCalleeHexStringView &lhs,
                             const std::string_view &rhs) {
        return lhs.t == rhs;
      }
      inline bool operator==(const EvmCodeHexStringView &lhs,
                             const std::string_view &rhs) {
        return lhs.t == rhs;
      }
    }  // namespace types
  }    // namespace interface
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_STRING_VIEW_TYPES_HPP
