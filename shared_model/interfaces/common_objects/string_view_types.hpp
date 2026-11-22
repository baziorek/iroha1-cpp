/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_STRING_VIEW_TYPES_HPP
#define IROHA_SHARED_MODEL_STRING_VIEW_TYPES_HPP

#include <format>
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

namespace std {
  /// std::format support for the strong string_view typedefs (they are not
  /// formattable through their string_view conversion alone).
#define IROHA_STD_FORMATTER_STRING_VIEW_TYPEDEF(Type)                        \
  template <>                                                               \
  struct formatter<Type> {                                                  \
    constexpr auto parse(format_parse_context &ctx) {                       \
      return ctx.begin();                                                   \
    }                                                                       \
    template <typename FormatContext>                                       \
    auto format(const Type &val, FormatContext &ctx) const {                \
      return format_to(ctx.out(), "{}", val.t);                             \
    }                                                                       \
  };

  IROHA_STD_FORMATTER_STRING_VIEW_TYPEDEF(
      shared_model::interface::types::SignedHexStringView)
  IROHA_STD_FORMATTER_STRING_VIEW_TYPEDEF(
      shared_model::interface::types::PublicKeyHexStringView)
  IROHA_STD_FORMATTER_STRING_VIEW_TYPEDEF(
      shared_model::interface::types::EvmCalleeHexStringView)
  IROHA_STD_FORMATTER_STRING_VIEW_TYPEDEF(
      shared_model::interface::types::EvmCodeHexStringView)

#undef IROHA_STD_FORMATTER_STRING_VIEW_TYPEDEF
}  // namespace std

#endif  // IROHA_SHARED_MODEL_STRING_VIEW_TYPES_HPP
