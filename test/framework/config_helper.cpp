/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "framework/config_helper.hpp"

#include <ciso646>
#include <cstdio>
#include <random>
#include <sstream>



namespace {
  std::string randomHexSuffix() {
    std::random_device rd;
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%08x", rd());
    return buf;
  }
}  // namespace

namespace integration_framework {
  const std::string kDefaultWorkingDatabaseName{"iroha_default"};

  std::string getPostgresCredsOrDefault() {
    static const std::string kDefaultPostgresCreds =
        "host=localhost "
        "port=5432 "
        "user=postgres "
        "password=mysecretpassword";
    return getPostgresCredsFromEnv().value_or(kDefaultPostgresCreds);
  }

  boost::optional<std::string> getPostgresCredsFromEnv() {
    auto pg_host = std::getenv("IROHA_POSTGRES_HOST");
    auto pg_port = std::getenv("IROHA_POSTGRES_PORT");
    auto pg_user = std::getenv("IROHA_POSTGRES_USER");
    auto pg_pass = std::getenv("IROHA_POSTGRES_PASSWORD");

    if (pg_host and pg_port and pg_user and pg_pass) {
      std::stringstream ss;
      ss << "host=" << pg_host << " port=" << pg_port << " user=" << pg_user
         << " password=" << pg_pass;
      return ss.str();
    }
    return {};
  }

  std::string getRandomDbName() {
    return std::string{"test_db_"}  // must begin with a letter or underscore
    + randomHexSuffix();
  }
}  // namespace integration_framework
