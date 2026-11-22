/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_GTEST_BOOST_OPTIONAL_PRINTER_HPP
#define IROHA_GTEST_BOOST_OPTIONAL_PRINTER_HPP

#include <boost/optional.hpp>

#include <ostream>

namespace boost {
  /// gtest printer for boost::optional. boost::optional's own operator<<
  /// static_asserts unless <boost/optional/optional_io.hpp> is included, and
  /// optional_io requires the contained type to be streamable, which is not
  /// the case for most types used in tests. This overload is found by ADL
  /// and only reports whether the optional is engaged.
  template <typename T>
  inline void PrintTo(const optional<T> &value, std::ostream *os) {
    *os << (value ? "engaged" : "boost::none");
  }
}  // namespace boost

#endif  // IROHA_GTEST_BOOST_OPTIONAL_PRINTER_HPP
