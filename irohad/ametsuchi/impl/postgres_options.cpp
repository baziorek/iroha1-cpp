/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/impl/postgres_options.hpp"

#include <boost/algorithm/string.hpp>
#include <cctype>
#include <limits>
#include <regex>

#include "common/std_format.hpp"
#include "logger/logger.hpp"

using namespace iroha::ametsuchi;

namespace {
  const std::string kPreparedBlockPrefix{"prepared_block_"};

  boost::optional<std::string> extractOptionalField(
      const std::string &connection_string, const std::string &field_name) {
    const std::regex field_regex(
        std::format(R"(\b{}=([^ ]+)\b)", field_name));
    std::smatch m;
    if (not std::regex_search(connection_string, m, field_regex)) {
      return boost::none;
    }
    return std::string{m[1]};
  }

  std::string extractField(const std::string &connection_string,
                           const std::string &field_name) {
    auto opt_value = extractOptionalField(connection_string, field_name);
    if (not opt_value) {
      throw(std::runtime_error(
          std::format("missing {} field in PostgresSQL connection string",
                      field_name)));
    }
    return std::move(opt_value).value();
  }

  static uint16_t getPort(const std::string &s) {
    auto number = std::stol(s);
    if (number <= 0 or number > std::numeric_limits<uint16_t>::max()) {
      throw(std::runtime_error(
          std::format("port number {} is out of range", s)));
    }
    return static_cast<uint16_t>(number);
  }
}  // namespace

PostgresOptions::PostgresOptions(const std::string &pg_opt,
                                 std::string default_dbname,
                                 logger::LoggerPtr log)
    : PostgresOptions(
        extractField(pg_opt, "host"),
        getPort(extractField(pg_opt, "port")),
        extractField(pg_opt, "user"),
        extractField(pg_opt, "password"),
        extractOptionalField(pg_opt, "dbname").value_or(default_dbname),
        extractOptionalField(pg_opt, "maintenance_dbname").value_or("postgres"),
        std::move(log)) {}

PostgresOptions::PostgresOptions(const std::string &host,
                                 uint16_t port,
                                 const std::string &user,
                                 const std::string &password,
                                 const std::string &working_dbname,
                                 const std::string &maintenance_dbname,
                                 logger::LoggerPtr log)
    : host_(host),
      port_(port),
      user_(user),
      password_(password),
      working_dbname_(working_dbname),
      maintenance_dbname_(maintenance_dbname),
      prepared_block_name_(kPreparedBlockPrefix + working_dbname_) {
  if (working_dbname_ == maintenance_dbname_) {
    log->warn(
        "Working database has the same name with maintenance database: '{}'. "
        "This will cause failures.",
        working_dbname_);
  }
}

std::string PostgresOptions::connectionStringWithoutDbName() const {
  return std::format("host={} port={} user={} password={}", host_, port_,
                     user_, password_);
}

std::string PostgresOptions::workingConnectionString() const {
  return getConnectionStringWithDbName(working_dbname_);
}

std::string PostgresOptions::maintenanceConnectionString() const {
  return getConnectionStringWithDbName(maintenance_dbname_);
}

std::string PostgresOptions::getConnectionStringWithDbName(
    const std::string &dbname) const {
  return connectionStringWithoutDbName() + " dbname=" + dbname;
}

std::string PostgresOptions::workingDbName() const {
  return working_dbname_;
}

std::string PostgresOptions::maintenanceDbName() const {
  return maintenance_dbname_;
}

const std::string &PostgresOptions::preparedBlockName() const {
  return prepared_block_name_;
}
