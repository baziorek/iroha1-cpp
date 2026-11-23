#pragma once

#include <string>

#include "common/result.hpp"
#include "cryptography/keypair.hpp"
#include "logger/logger_fwd.hpp"

namespace iroha_lib {

/**
 * Generate a new keypair and store it as key files on disk
 * (<directory>/<name>.pub and <directory>/<name>.priv).
 * Existing files are overwritten, like KeysManagerImpl::createKeys.
 *
 * @param name account id / file name used as the file prefix
 *        (e.g. "admin@test")
 * @param pass_phrase optional pass phrase for key encryption
 *        (empty string means unencrypted keys)
 * @param keys_directory directory for the key files
 *        (empty means current directory)
 * @param log logger
 * @return generated keypair or error message
 */
iroha::expected::Result<shared_model::crypto::Keypair, std::string>
createKeyFiles(const std::string &name,
               const std::string &pass_phrase,
               const std::string &keys_directory,
               const logger::LoggerPtr &log);

/**
 * Load an existing keypair from key files on disk.
 *
 * @param name same identifier as used in createKeyFiles
 * @param pass_phrase optional pass phrase used during creation
 * @param keys_directory directory with the key files
 *        (empty means current directory)
 * @param log logger
 * @return loaded keypair or error message
 */
iroha::expected::Result<shared_model::crypto::Keypair, std::string>
loadKeyFiles(const std::string &name,
             const std::string &pass_phrase,
             const std::string &keys_directory,
             const logger::LoggerPtr &log);

}  // namespace iroha_lib
