/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SOCI_STD_OPTIONAL_HPP
#define IROHA_SOCI_STD_OPTIONAL_HPP

#include <soci/std-optional.h>

namespace soci {

  template <>
  struct type_conversion<std::nullopt_t> {
    typedef int base_type;

    static void from_base(base_type const & /*in*/,
                          indicator /*ind*/,
                          std::nullopt_t & /*out*/) {}

    static void to_base(std::nullopt_t const & /*in*/,
                        base_type & /*out*/,
                        indicator &ind) {
      ind = i_null;
    }
  };

}  // namespace soci

#endif  // IROHA_SOCI_STD_OPTIONAL_HPP
