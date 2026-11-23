#include "model/utils/key_files.hpp"

#include <boost/optional.hpp>

#include "crypto/keys_manager_impl.hpp"

namespace iroha_lib {

namespace {
boost::optional<std::string> toOptionalPassphrase(
    const std::string &pass_phrase) {
  if (pass_phrase.empty()) {
    return boost::none;
  }
  return pass_phrase;
}

iroha::KeysManagerImpl makeManager(const std::string &name,
                                   const std::string &keys_directory,
                                   const logger::LoggerPtr &log) {
  if (keys_directory.empty()) {
    return iroha::KeysManagerImpl(name, log);
  }
  return iroha::KeysManagerImpl(name, keys_directory, log);
}
}  // namespace

iroha::expected::Result<shared_model::crypto::Keypair, std::string>
createKeyFiles(const std::string &name,
               const std::string &pass_phrase,
               const std::string &keys_directory,
               const logger::LoggerPtr &log) {
  auto manager = makeManager(name, keys_directory, log);
  if (not manager.createKeys(toOptionalPassphrase(pass_phrase))) {
    return iroha::expected::makeError(
        std::string("Keys could not be created"));
  }
  return manager.loadKeys(toOptionalPassphrase(pass_phrase));
}

iroha::expected::Result<shared_model::crypto::Keypair, std::string>
loadKeyFiles(const std::string &name,
             const std::string &pass_phrase,
             const std::string &keys_directory,
             const logger::LoggerPtr &log) {
  auto manager = makeManager(name, keys_directory, log);
  return manager.loadKeys(toOptionalPassphrase(pass_phrase));
}

}  // namespace iroha_lib
