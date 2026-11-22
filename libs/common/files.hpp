/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_FILES_HPP
#define IROHA_FILES_HPP

#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <filesystem>
#include "common/result_fwd.hpp"
#include "logger/logger_fwd.hpp"

/**
 * This source file contains common methods related to files
 */
namespace iroha {

  /**
   * Generate a unique path inside the temp directory.
   * Replacement for boost::filesystem::unique_path().
   */
  inline std::filesystem::path unique_path() {
    static std::atomic<std::uint64_t> counter{0};
    const auto tick =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count();
    return std::to_string(tick) + "-" +
           std::to_string(counter.fetch_add(1, std::memory_order_relaxed));
  }

  /**
   * Remove all files and directories inside a folder.
   * Keeps the target folder.
   * @param dir - target folder
   * @param log - a log for local messages
   */
  void remove_dir_contents(const std::filesystem::path &dir,
                           const logger::LoggerPtr &log);

  /**
   * Read file in text mode, and either return its contents as a string
   * or return the error as a string
   * @param path - path to the file
   */
  iroha::expected::Result<std::string, std::string> readTextFile(
      const std::filesystem::path &path);

  /**
   * Read file in binary mode, and either return its contents as a byte vector
   * or return the error as a string
   * @param path - path to the file
   */
  iroha::expected::Result<std::vector<uint8_t>, std::string> readBinaryFile(
      const std::filesystem::path &path);
}  // namespace iroha
#endif  // IROHA_FILES_HPP
