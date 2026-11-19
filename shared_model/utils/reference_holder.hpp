/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_REFERENCE_HOLDER_HPP
#define IROHA_REFERENCE_HOLDER_HPP

#include <optional>
#include <type_traits>

namespace shared_model {
  namespace detail {
    /**
     * Container designed to store reference or value depending on called ctor
     * @tparam T type of stored value
     */
    template <typename T>
    class ReferenceHolder {
     public:
      template <typename V>
      explicit ReferenceHolder(V &&value) {
        using U = std::remove_reference_t<V>;
        constexpr bool is_nonconst_lvalue =
            std::is_lvalue_reference_v<V &&> && !std::is_const_v<U>
            && std::is_same_v<std::remove_cv_t<U>, std::remove_cv_t<T>>;
        if constexpr (is_nonconst_lvalue) {
          ptr_ = std::addressof(value);
        } else {
          owned_.emplace(std::forward<V>(value));
          ptr_ = std::addressof(*owned_);
        }
      }

      ReferenceHolder(const ReferenceHolder &other)
          : owned_(other.owned_), ptr_(nullptr) {
        if (owned_) {
          ptr_ = std::addressof(*owned_);
        } else {
          ptr_ = other.ptr_;
        }
      }

      ReferenceHolder(ReferenceHolder &&other) noexcept
          : owned_(std::move(other.owned_)), ptr_(nullptr) {
        if (owned_) {
          ptr_ = std::addressof(*owned_);
        } else {
          ptr_ = other.ptr_;
        }
      }

      ReferenceHolder &operator=(const ReferenceHolder &other) {
        if (this != &other) {
          owned_ = other.owned_;
          ptr_ = owned_ ? std::addressof(*owned_) : other.ptr_;
        }
        return *this;
      }

      ReferenceHolder &operator=(ReferenceHolder &&other) noexcept {
        if (this != &other) {
          owned_ = std::move(other.owned_);
          ptr_ = owned_ ? std::addressof(*owned_) : other.ptr_;
        }
        return *this;
      }

      using ReferenceType = typename std::add_lvalue_reference_t<T>;
      using PointerType = typename std::add_pointer_t<T>;

      using ConstReferenceType = typename std::add_lvalue_reference_t<const T>;
      using ConstPointerType = typename std::add_pointer_t<const T>;

      ReferenceType operator*() {
        return *ptr();
      }

      PointerType operator->() {
        return ptr();
      }

      PointerType ptr() {
        return ptr_;
      }

      ConstReferenceType operator*() const {
        return *ptr();
      }

      ConstPointerType operator->() const {
        return ptr();
      }

      ConstPointerType ptr() const {
        return ptr_;
      }

     private:
      std::optional<T> owned_;
      T *ptr_ = nullptr;
    };
  }  // namespace detail
}  // namespace shared_model

#endif  // IROHA_REFERENCE_HOLDER_HPP
