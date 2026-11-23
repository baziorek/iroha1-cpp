/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gflags/gflags.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/rapidjson.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#include "backend/protobuf/proto_block_json_converter.hpp"
#include "backend/protobuf/queries/proto_query.hpp"
#include "backend/protobuf/transaction.hpp"
#include "client.hpp"
#include "common/result.hpp"
#include "converters/protobuf/json_proto_converter.hpp"
#include "grpc_response_handler.hpp"
#include "interactive/interactive_cli.hpp"
#include "logger/logger.hpp"
#include "logger/logger_manager.hpp"
#include "model/converters/json_block_factory.hpp"
#include "model/converters/json_query_factory.hpp"
#include "model/converters/pb_query_factory.hpp"
#include "model/converters/pb_transaction_factory.hpp"
#include "model/utils/genesis.hpp"
#include "model/utils/key_files.hpp"

// Account information
DEFINE_bool(new_account,
            false,
            "Generate and save locally new public/private keys");
DEFINE_string(account_name,
              "",
              "Name of the account. Must be unique in iroha network");
DEFINE_string(pass_phrase, "", "Account pass-phrase");
DEFINE_string(key_path, ".", "Path to user keys");

// Iroha peer to connect with
DEFINE_string(peer_ip, "127.0.0.1", "Address of the Iroha node");
DEFINE_int32(torii_port, 50051, "Port of Iroha's Torii");
DEFINE_string(tls_root_cert, "", "Optional path to PEM root certificate; when set, connections use TLS");

// Send already signed and formed transaction to Iroha peer
DEFINE_string(json_transaction, "", "Transaction in json format");
// Send already signed and formed query to Iroha peer
DEFINE_string(json_query, "", "Query in json format");

// Genesis block generator:
DEFINE_bool(genesis_block,
            false,
            "Generate genesis block for new Iroha network");
DEFINE_string(genesis_transaction,
              "",
              "File with transaction in json format for the genesis block");
DEFINE_string(peers_address,
              "",
              "File with peers address for new Iroha network");

// Run iroha-cli in interactive mode
DEFINE_bool(interactive, true, "Run iroha-cli in interactive mode");

using namespace iroha::protocol;
using namespace iroha::model::converters;
using namespace iroha_cli::interactive;
namespace fs = std::filesystem;

iroha::keypair_t *makeOldModel(const shared_model::crypto::Keypair &keypair) {
  return new iroha::keypair_t{
      iroha::pubkey_t::from_hexstring(keypair.publicKey()).assumeValue(),
      iroha::privkey_t::from_string(toBinaryString(keypair.privateKey()))
          .assumeValue()};
}

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  gflags::ShutDownCommandLineFlags();
  auto log_manager = std::make_shared<logger::LoggerManagerTree>(
                         logger::LoggerConfig{logger::LogLevel::kInfo,
                                              logger::getDefaultLogPatterns()})
                         ->getChild("CLI");
  const auto logger = log_manager->getChild("Main")->getLogger();
  const auto response_handler_log_manager =
      log_manager->getChild("ResponseHandler");
  const auto pb_qry_factory_log =
      log_manager->getChild("PbQueryFactory")->getLogger();
  const auto json_qry_factory_log =
      log_manager->getChild("JsonQueryFactory")->getLogger();
  const auto keys_manager_log =
      log_manager->getChild("KeysManager")->getLogger();
  // Generate new genesis block now Iroha network
  if (FLAGS_genesis_block) {
    auto writeGenesisBlock = [&logger](const shared_model::proto::Block &block) {
      // Convert to json
      std::ofstream output_file("genesis.block");
      shared_model::proto::ProtoBlockJsonConverter().serialize(block).match(
          [&logger, &output_file](auto &&json) {
            output_file << std::move(json.value);
            logger->info("File saved to genesis.block");
          },
          [&logger](const auto &error) {
            // should not get here
            logger->error("error while serializing genesis block");
            std::exit(EXIT_FAILURE);
          });
    };

    if (FLAGS_genesis_transaction.empty()) {
      if (FLAGS_peers_address.empty()) {
        logger->error("--peers_address is empty");
        return EXIT_FAILURE;
      }
      iroha_lib::GenesisConfig config;
      config.peer_addresses =
          iroha_lib::readPeersAddresses(FLAGS_peers_address);
      config.keys_directory = FLAGS_key_path;
      auto block = iroha_lib::generateGenesisBlock(config, keys_manager_log);
      if (auto e = iroha::expected::resultToOptionalError(block)) {
        logger->error("Cannot generate genesis block: {}", e.value());
        return EXIT_FAILURE;
      }
      writeGenesisBlock(block.assumeValue());
    } else {
      rapidjson::Document doc;
      std::ifstream file(FLAGS_genesis_transaction);
      rapidjson::IStreamWrapper isw(file);
      doc.ParseStream(isw);
      auto some_tx = JsonTransactionFactory{}.deserialize(doc);
      if (not some_tx) {
        logger->error(
            "Cannot deserialize genesis transaction (problem with file reading "
            "or illformed json?)");
        return EXIT_FAILURE;
      }
      auto tx = PbTransactionFactory().serialize(*some_tx);
      auto block = iroha_lib::generateGenesisBlockFromTransactions({tx});
      if (auto e = iroha::expected::resultToOptionalError(block)) {
        logger->error("Cannot generate genesis block: {}", e.value());
        return EXIT_FAILURE;
      }
      writeGenesisBlock(block.assumeValue());
    }
  }
  // Create new pub/priv key, register in Iroha Network
  else if (FLAGS_new_account) {
    if (FLAGS_account_name.empty()) {
      logger->error("No account name specified");
      return EXIT_FAILURE;
    }
    auto keypair = iroha_lib::createKeyFiles(
        FLAGS_account_name, FLAGS_pass_phrase, "", keys_manager_log);
    if (auto e = iroha::expected::resultToOptionalError(keypair)) {
      logger->error("{}", e.value());
      return EXIT_FAILURE;
    }
    logger->info(
        "Public and private key has been generated in current directory");

  }
  // Send to Iroha Peer json transaction/query
  else if (not FLAGS_json_transaction.empty() or not FLAGS_json_query.empty()) {
    const auto tls = [&]() -> std::optional<iroha_lib::TlsOptions> {
      if (FLAGS_tls_root_cert.empty()) {
        return std::nullopt;
      }
      std::ifstream cert_file(FLAGS_tls_root_cert);
      if (not cert_file.is_open()) {
        logger->error("Cannot open TLS root certificate: {}",
                      FLAGS_tls_root_cert);
        return std::nullopt;
      }
      std::ostringstream pem;
      pem << cert_file.rdbuf();
      iroha_lib::TlsOptions options;
      options.pem_root_certs = pem.str();
      return options;
    }();
    if (FLAGS_tls_root_cert.size() and not tls) {
      return 1;
    }
    iroha_cli::CliClient client(FLAGS_peer_ip, FLAGS_torii_port, tls);
    iroha_cli::GrpcResponseHandler response_handler(
        response_handler_log_manager);
    if (not FLAGS_json_transaction.empty()) {
      logger->info(
          "Send transaction to {}:{} ", FLAGS_peer_ip, FLAGS_torii_port);
      // Read from file
      std::ifstream file(FLAGS_json_transaction);
      std::string str((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
      iroha::model::converters::JsonTransactionFactory serializer;
      auto doc = iroha::model::converters::stringToJson(str);
      if (not doc) {
        logger->error("Json has wrong format.");
        return EXIT_FAILURE;
      }
      auto tx_opt = serializer.deserialize(doc.value());
      if (not tx_opt) {
        logger->error("Json transaction has wrong format.");
        return EXIT_FAILURE;
      } else {
        response_handler.handle(
            client.sendTx(PbTransactionFactory().serialize(*tx_opt)));
      }
    }
    if (not FLAGS_json_query.empty()) {
      logger->info("Send query to {}:{}", FLAGS_peer_ip, FLAGS_torii_port);
      std::ifstream file(FLAGS_json_query);
      std::string str((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
      iroha::model::converters::JsonQueryFactory serializer(
          json_qry_factory_log);
      auto query_opt = serializer.deserialize(std::move(str));
      if (not query_opt) {
        logger->error("Json has wrong format.");
        return EXIT_FAILURE;
      } else {
        auto serialized =
            PbQueryFactory(pb_qry_factory_log).serialize(*query_opt);
        if (serialized) {
          auto response = client.sendQuery(*serialized);
          response_handler.handle(response);
        } else {
          logger->error("Cannot serialize query to protobuf format.");
        }
      }
    }
  }
  // Run iroha-cli in interactive mode
  else if (FLAGS_interactive) {
    if (FLAGS_account_name.empty()) {
      logger->error("Specify your account name");
      return EXIT_FAILURE;
    }
    fs::path path(FLAGS_key_path);
    if (not fs::exists(path)) {
      logger->error("Path {} not found.", path.string());
      return EXIT_FAILURE;
    }
    auto keypair = iroha_lib::loadKeyFiles(FLAGS_account_name,
                                            FLAGS_pass_phrase,
                                            path.string(),
                                            keys_manager_log);
    if (auto e = iroha::expected::resultToOptionalError(keypair)) {
      logger->error(
          "Keypair error: {}.\n"
          "keypair path: '{}', name: {}. Use --key_path with path of your "
          "keypair. \n"
          "Maybe wrong pass phrase (\"{}\")?",
          e.value(),
          path.string(),
          FLAGS_account_name,
          FLAGS_pass_phrase);
      return EXIT_FAILURE;
    }
    // TODO 13/09/17 grimadas: Init counters from Iroha, or read from disk?
    // IR-334
    InteractiveCli interactiveCli(
        FLAGS_account_name,
        FLAGS_peer_ip,
        FLAGS_torii_port,
        0,
        *std::unique_ptr<iroha::keypair_t>(
            makeOldModel(keypair.assumeValue())),
        response_handler_log_manager,
        pb_qry_factory_log,
        json_qry_factory_log,
        log_manager->getChild("InteractiveCli"));
    interactiveCli.run();
  } else {
    logger->error("Invalid flags");
    return EXIT_FAILURE;
  }
  return 0;
}
