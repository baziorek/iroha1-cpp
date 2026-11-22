/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_VISITOR_HPP
#define IROHA_VISITOR_HPP

#include <functional>  // for std::reference_wrapper
#include <memory>      // for std::addressof
#include <type_traits>  // for std::decay
#include <utility>      // for std::forward
#include <variant>      // for std::visit

namespace iroha {

  namespace detail {
    template <typename T>
    struct is_reference_wrapper : std::false_type {};

    template <typename T>
    struct is_reference_wrapper<std::reference_wrapper<T>>
        : std::true_type {};

    template <typename T>
    inline constexpr bool is_reference_wrapper_v =
        is_reference_wrapper<std::remove_cv_t<std::remove_reference_t<T>>>::
            value;

    /**
     * @brief Visitor adapter which unwraps std::reference_wrapper
     * alternatives before invoking the wrapped visitor, so visitors observe
     * the referred objects (like boost::variant of references did) instead
     * of the wrapper itself.
     */
    template <typename Visitor>
    struct unwrap_reference_wrapper_visitor {
      Visitor visitor;

      template <typename T>
      decltype(auto) operator()(std::reference_wrapper<T> wrapped) {
        return visitor(wrapped.get());
      }

      template <typename T>
      requires(not is_reference_wrapper_v<T>)
      decltype(auto) operator()(T &&value) {
        return visitor(std::forward<T>(value));
      }
    };
  }  // namespace detail

  /**
   * @brief std::visit which additionally unwraps std::reference_wrapper
   * variant alternatives before calling the visitor.
   */
  template <typename Visitor, typename... Variants>
  constexpr decltype(auto) visit(Visitor &&visitor, Variants &&...variants) {
    return std::visit(
        detail::unwrap_reference_wrapper_visitor<
            std::remove_reference_t<Visitor>>{std::forward<Visitor>(visitor)},
        std::forward<Variants>(variants)...);
  }

  /**
   * @brief Pointer-based accessor for variants holding
   * std::reference_wrapper<const T> alternatives. Returns a pointer to the
   * referred object, or nullptr if the variant does not hold T.
   */
  template <typename T, typename Variant>
  constexpr const T *get_wrapped(const Variant *variant) noexcept {
    if (auto wrapped =
            std::get_if<std::reference_wrapper<const T>>(variant)) {
      return std::addressof(wrapped->get());
    }
    return nullptr;
  }

  template <typename... Lambdas>
  struct lambda_visitor;

  template <typename Lambda, typename... Lambdas>
  struct lambda_visitor<Lambda, Lambdas...>
      : public Lambda, public lambda_visitor<Lambdas...> {
    using Lambda::operator();
    using lambda_visitor<Lambdas...>::operator();

    lambda_visitor(Lambda lambda, Lambdas... lambdas)
        : Lambda(lambda), lambda_visitor<Lambdas...>(lambdas...) {}
  };

  template <typename Lambda>
  struct lambda_visitor<Lambda> : public Lambda {
    using Lambda::operator();

    lambda_visitor(Lambda lambda) : Lambda(lambda) {}
  };

  /**
   * @brief Convenient in-place compile-time visitor creation, from a set of
   * lambdas
   *
   * @code
   * make_visitor([](int a){ return 1; },
   *              [](std::string b) { return 2; });
   * @nocode
   *
   * is essentially the same as
   *
   * @code
   * struct visitor {
   *   int operator()(int a) { return 1; }
   *   int operator()(std::string b) { return 2; }
   * }
   * @nocode
   *
   * @param lambdas
   * @return visitor
   */
  template <class... Fs>
  constexpr auto make_visitor(Fs &&... fs) {
    using visitor_type = lambda_visitor<std::decay_t<Fs>...>;
    return visitor_type(std::forward<Fs>(fs)...);
  }

  /**
   * @brief Inplace visitor for std::variant.
   * @code
   *   std::variant<int, std::string> value = "1234";
   *   ...
   *   visit_in_place(value,
   *                  [](int v) { std::cout << "(int)" << v; },
   *                  [](std::string v) { std::cout << "(string)" << v;}
   *                  );
   * @nocode
   *
   * @param variant
   * @param lambdas
   * @param lambdas
   */
  template <typename TVariant, typename... TVisitors>
  constexpr decltype(auto) visit_in_place(TVariant &&variant,
                                          TVisitors &&... visitors) {
    return iroha::visit(make_visitor(std::forward<TVisitors>(visitors)...),
                        std::forward<TVariant>(variant));
  }

  /// apply Matcher to optional T
  template <typename T, typename Matcher>
  constexpr decltype(auto) match(T &&t, Matcher &&m) {
    return std::forward<T>(t) ? std::forward<Matcher>(m)(*std::forward<T>(t))
                              : std::forward<Matcher>(m)();
  }

  /// construct visitor from Fs and apply it to optional T
  template <typename T, typename... Fs>
  constexpr decltype(auto) match_in_place(T &&t, Fs &&... fs) {
    return match(std::forward<T>(t), make_visitor(std::forward<Fs>(fs)...));
  }
}  // namespace iroha

#endif  // IROHA_VISITOR_HPP
