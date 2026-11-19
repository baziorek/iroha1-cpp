/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_VISITOR_APPLY_FOR_ALL_HPP
#define IROHA_VISITOR_APPLY_FOR_ALL_HPP

#include <functional>
#include <string>

namespace shared_model {
  namespace detail {

    /**
     * Class provides generic toString visitor for objects
     */
    class ToStringVisitor {
     public:
      template <typename T>
      std::string operator()(const std::reference_wrapper<const T> &operand)
          const {
        return operand.get().toString();
      }

      template <typename InputType>
      auto operator()(const InputType &operand) const {
        return operand.toString();
      }
    };

  }  // namespace detail
}  // namespace shared_model
#endif  // IROHA_VISITOR_APPLY_FOR_ALL_HPP
