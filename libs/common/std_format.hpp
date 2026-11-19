#ifndef IROHA_COMMON_STD_FORMAT_HPP
#define IROHA_COMMON_STD_FORMAT_HPP

#include <format>
#include <string>
#include <type_traits>

/// Specializes std::formatter for a user-defined type T by delegating to its
/// toString() method, mirroring the former generic fmt::formatter.
#define IROHA_STD_FORMATTER_TO_STRING(Type)                                \
  template <>                                                             \
  struct std::formatter<Type> {                                           \
    constexpr auto parse(std::format_parse_context &ctx) {                \
      return ctx.begin();                                                 \
    }                                                                     \
    template <typename FormatContext>                                     \
    auto format(const Type &val, FormatContext &ctx) const {              \
      return std::format_to(ctx.out(), "{}", val.toString());             \
    }                                                                     \
  };

#endif  // IROHA_COMMON_STD_FORMAT_HPP
