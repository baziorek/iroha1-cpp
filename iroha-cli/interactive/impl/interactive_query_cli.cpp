/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <boost/algorithm/string.hpp>
#include <fstream>

#include "backend/protobuf/queries/proto_query.hpp"

#include "client.hpp"
#include "common/byteutils.hpp"
#include "datetime/time.hpp"
#include "grpc_response_handler.hpp"
#include "interactive/interactive_query_cli.hpp"
#include "logger/logger.hpp"
#include "model/converters/json_query_factory.hpp"
#include "model/converters/pb_query_factory.hpp"
#include "model/queries/get_asset_info.hpp"
#include "model/queries/get_roles.hpp"

namespace iroha_cli {
  namespace interactive {

    void InteractiveQueryCli::create_queries_menu() {
      description_map_ = {
          {GET_ACC, "Get Account Information"},
          {GET_ACC_AST, "Get Account's Assets"},
          {GET_ACC_AST_TX, "Get Account's Asset Transactions"},
          {GET_ACC_TX, "Get Account's Transactions"},
          {GET_TX, "Get Transactions by transactions' hashes"},
          {GET_ACC_SIGN, "Get Account's Signatories"},
          {GET_ROLES, "Get all current roles in the system"},
          {GET_AST_INFO, "Get information about asset"},
          {GET_ROLE_PERM, "Get all permissions related to role"}
          // description_map_
      };

      const auto acc_id = "Requested account Id";
      const auto ast_id = "Requested asset Id";
      const auto role_id = "Requested role name";
      const auto tx_hashes = "Requested tx hashes";

      query_params_map_ = {
          {GET_ACC, makeParamsDescription({acc_id})},
          {GET_ACC_AST, makeParamsDescription({acc_id, ast_id})},
          {GET_ACC_AST_TX, makeParamsDescription({acc_id, ast_id})},
          {GET_ACC_TX, makeParamsDescription({acc_id})},
          {GET_TX, makeParamsDescription({tx_hashes})},
          {GET_ACC_SIGN, makeParamsDescription({acc_id})},
          {GET_ROLES, makeParamsDescription({})},
          {GET_AST_INFO, makeParamsDescription({ast_id})},
          {GET_ROLE_PERM, makeParamsDescription({role_id})}
          // query_params_map_
      };

      query_handlers_ = {
          {GET_ACC, &InteractiveQueryCli::parseGetAccount},
          {GET_ACC_AST, &InteractiveQueryCli::parseGetAccountAssets},
          {GET_ACC_AST_TX,
           &InteractiveQueryCli::parseGetAccountAssetTransactions},
          {GET_ACC_TX, &InteractiveQueryCli::parseGetAccountTransactions},
          {GET_TX, &InteractiveQueryCli::parseGetTransactions},
          {GET_ACC_SIGN, &InteractiveQueryCli::parseGetSignatories},
          {GET_ROLE_PERM, &InteractiveQueryCli::parseGetRolePermissions},
          {GET_ROLES, &InteractiveQueryCli::parseGetRoles},
          {GET_AST_INFO, &InteractiveQueryCli::parseGetAssetInfo}
          // query_handlers_
      };

      menu_points_ =
          formMenu(query_handlers_, query_params_map_, description_map_);
      // Add "go back" option
      addBackOption(menu_points_);
    }

    void InteractiveQueryCli::create_result_menu() {
      result_handlers_ = {{SAVE_CODE, &InteractiveQueryCli::parseSaveFile},
                          {SEND_CODE, &InteractiveQueryCli::parseSendToIroha}};
      result_params_map_ = getCommonParamsMap(default_peer_ip_, default_port_);

      result_points_ = formMenu(
          result_handlers_, result_params_map_, getCommonDescriptionMap());
      addBackOption(result_points_);
    }

    InteractiveQueryCli::InteractiveQueryCli(
        const std::string &account_name,
        const std::string &default_peer_ip,
        int default_port,
        uint64_t query_counter,
        const iroha::keypair_t &keypair,
        logger::LoggerManagerTreePtr response_handler_log_manager,
        logger::LoggerPtr pb_qry_factory_log,
        logger::LoggerPtr json_qry_factory_log,
        logger::LoggerPtr log)
        : current_context_(MAIN),
          creator_(account_name),
          default_peer_ip_(default_peer_ip),
          default_port_(default_port),
          counter_(query_counter),
          keypair_(keypair),
          response_handler_log_manager_(
              std::move(response_handler_log_manager)),
          pb_qry_factory_log_(std::move(pb_qry_factory_log)),
          json_qry_factory_log_(std::move(json_qry_factory_log)),
          log_(std::move(log)) {
      create_queries_menu();
      create_result_menu();
    }

    static void printMenu(const MenuPoints &menu) {
      printMenu("Choose query: ", menu);
    }

    std::shared_ptr<iroha_lib::Query> InteractiveQueryCli::makeQuery() {
      return std::make_shared<iroha_lib::Query>(
          keypair_, counter_, creator_, local_time_);
    }

    void InteractiveQueryCli::run() {
      bool is_parsing = true;
      current_context_ = MAIN;
      printMenu(menu_points_);
      // Creating a new query, increment local counter
      ++counter_;
      // Init timestamp for a new query
      local_time_ = iroha::time::now();

      while (is_parsing) {
        auto line = promptString("> ");
        if (not line) {
          // The promtSting returns error, terminating symbol
          is_parsing = false;
          break;
        }
        switch (current_context_) {
          case MAIN:
            is_parsing = parseQuery(line.value());
            break;
          case RESULT:
            is_parsing = parseResult(line.value());
            break;
          default:
            BOOST_ASSERT_MSG(false, "not implemented");
        }
      }
    }

    bool InteractiveQueryCli::parseQuery(std::string line) {
      if (isBackOption(line)) {
        // Stop parsing
        return false;
      }

      auto res = handleParse<std::shared_ptr<iroha_lib::Query>>(
          this, line, query_handlers_, query_params_map_);
      if (not res) {
        // Continue parsing
        return true;
      }

      query_ = res.value();
      current_context_ = RESULT;
      printMenu("Query is formed. Choose what to do:", result_points_);
      // Continue parsing
      return true;
    }

    std::shared_ptr<iroha_lib::Query> InteractiveQueryCli::parseGetAccount(
        QueryParams params) {
      auto query = makeQuery();
      query->getAccount(params[0]);
      return query;
    }

    std::shared_ptr<iroha_lib::Query>
    InteractiveQueryCli::parseGetAccountAssets(QueryParams params) {
      // NOTE: the second parameter (asset id) is kept for menu
      // compatibility; GetAccountAssets query does not use it, same as
      // the previous implementation.
      auto query = makeQuery();
      query->getAccountAssets(params[0]);
      return query;
    }

    std::shared_ptr<iroha_lib::Query>
    InteractiveQueryCli::parseGetAccountAssetTransactions(QueryParams params) {
      auto query = makeQuery();
      query->getAccountAssetTransactions(params[0], params[1]);
      return query;
    }

    std::shared_ptr<iroha_lib::Query>
    InteractiveQueryCli::parseGetAccountTransactions(QueryParams params) {
      auto query = makeQuery();
      query->getAccountTransactions(params[0]);
      return query;
    }

    std::shared_ptr<iroha_lib::Query>
    InteractiveQueryCli::parseGetTransactions(QueryParams params) {
      // Parser definition: hash1 hash2 ...
      // NOTE: lib QueryGenerator expects raw (non-hex) hash strings and
      // hex-encodes them itself.
      std::vector<std::string> tx_hashes;
      std::for_each(
          params.begin(), params.end(), [&tx_hashes](auto const &hex_hash) {
            if (auto raw = iroha::hexstringToBytestring(hex_hash)) {
              tx_hashes.push_back(*raw);
            }
          });
      auto query = makeQuery();
      query->getTransactions(creator_, tx_hashes);
      return query;
    }

    std::shared_ptr<iroha_lib::Query>
    InteractiveQueryCli::parseGetSignatories(QueryParams params) {
      auto query = makeQuery();
      query->getSignatories(params[0]);
      return query;
    }

    std::shared_ptr<iroha_lib::Query> InteractiveQueryCli::parseGetAssetInfo(
        QueryParams params) {
      auto query = makeQuery();
      query->getAssetInfo(creator_, params[0]);
      return query;
    }

    std::shared_ptr<iroha_lib::Query> InteractiveQueryCli::parseGetRoles(
        QueryParams params) {
      auto query = makeQuery();
      query->getRoles(creator_);
      return query;
    }

    std::shared_ptr<iroha_lib::Query>
    InteractiveQueryCli::parseGetRolePermissions(QueryParams params) {
      auto query = makeQuery();
      query->getRolePermissions(creator_, params[0]);
      return query;
    }

    bool InteractiveQueryCli::parseResult(std::string line) {
      if (isBackOption(line)) {
        // Give up the last query and start a new one
        current_context_ = MAIN;
        printEnd();
        printMenu(menu_points_);
        // Continue parsing
        return true;
      }

      auto res = handleParse<bool>(
          this, line, result_handlers_, result_params_map_);

      return res.get_value_or(true);
    }

    bool InteractiveQueryCli::parseSendToIroha(QueryParams params) {
      auto address =
          parseIrohaPeerParams(params, default_peer_ip_, default_port_);
      if (not address) {
        return true;
      }

      const auto signed_query = query_->signAndAddSignature();

      CliClient client(
          address.value().first, address.value().second);
      GrpcResponseHandler{response_handler_log_manager_}.handle(
          client.sendQuery(signed_query));
      printEnd();
      // Stop parsing
      return false;
    }

    bool InteractiveQueryCli::parseSaveFile(QueryParams params) {
      const auto signed_query = query_->signAndAddSignature();

      auto path = params[0];
      std::ofstream output_file(path);
      if (not output_file) {
        std::cout << "Cannot create file" << std::endl;
        // Continue parsing
        return true;
      }

      // Convert proto query back to the model one for JSON output
      auto model_query =
          iroha::model::converters::PbQueryFactory(pb_qry_factory_log_)
              .deserialize(signed_query);
      if (not model_query) {
        std::cout << "Cannot serialize query" << std::endl;
        return true;
      }

      iroha::model::converters::JsonQueryFactory json_factory(
          json_qry_factory_log_);
      auto json_string = json_factory.serialize(*model_query);
      output_file << json_string;
      std::cout << "Successfully saved!" << std::endl;
      printEnd();
      // Stop parsing
      return false;
    }

  }  // namespace interactive
}  // namespace iroha_cli
