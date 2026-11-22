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

namespace iroha {
  /// std::vformat with forwarding: binds args to lvalues so that
  /// std::make_format_args accepts temporaries as well.
  template <typename... Args>
  inline std::string vformat(std::string_view format, Args &&...args) {
    return std::vformat(format, std::make_format_args(args...));
  }
}  // namespace iroha

#endif  // IROHA_COMMON_STD_FORMAT_HPP
