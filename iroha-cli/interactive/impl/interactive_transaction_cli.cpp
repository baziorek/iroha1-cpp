/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interactive/interactive_transaction_cli.hpp"

#include <fstream>
#include <unordered_set>
#include <utility>

#include "client.hpp"
#include "datetime/time.hpp"
#include "grpc_response_handler.hpp"
#include "logger/logger.hpp"
#include "model/converters/json_common.hpp"
#include "model/converters/json_transaction_factory.hpp"
#include "model/converters/pb_common.hpp"
#include "model/converters/pb_transaction_factory.hpp"
#include "parser/parser.hpp"  // for parser::ParseValue

namespace iroha_cli {
  namespace interactive {
    namespace {
      using iroha::protocol::GrantablePermission;
      using iroha::protocol::RolePermission;

      std::shared_ptr<TxCommandApplier> makeApplier(
          TxCommandApplier applier) {
        return std::make_shared<TxCommandApplier>(std::move(applier));
      }

      /**
       * Map of permission names accepted by grant_perm/revoke_perm
       * commands to protobuf GrantablePermission values.
       */
      const std::unordered_map<std::string, GrantablePermission> &
      grantablePermissions() {
        static const std::unordered_map<std::string, GrantablePermission>
            permissions = {
                {"can_add_my_signatory",
                 GrantablePermission::can_add_my_signatory},
                {"can_remove_my_signatory",
                 GrantablePermission::can_remove_my_signatory},
                {"can_set_my_quorum",
                 GrantablePermission::can_set_my_quorum},
                {"can_set_my_account_detail",
                 GrantablePermission::can_set_my_account_detail},
                {"can_set_detail",
                 GrantablePermission::can_set_my_account_detail},
                {"can_transfer_my_assets",
                 GrantablePermission::can_transfer_my_assets},
                {"can_transfer",
                 GrantablePermission::can_transfer_my_assets},
            };
        return permissions;
      }

      boost::optional<GrantablePermission> parseGrantablePermission(
          const std::string &name) {
        auto it = grantablePermissions().find(name);
        if (it == grantablePermissions().end()) {
          return boost::none;
        }
        return it->second;
      }
    }  // namespace

    void InteractiveTransactionCli::createCommandMenu() {
      commands_description_map_ = {
          {ADD_ASSET_QTY, "Add Asset Quantity"},
          {ADD_PEER, "Add Peer to Iroha Network"},
          {ADD_SIGN, "Add Signatory to Account"},
          {CREATE_ACC, "Create Account"},
          {CREATE_DOMAIN, "Create Domain"},
          {CREATE_ASSET, "Create Asset"},
          {REMOVE_SIGN, "Remove Signatory"},
          {SET_QUO, "Set Account Quorum"},
          {SUB_ASSET_QTY, "Subtract Assets Quantity"},
          {TRAN_ASSET, "Transfer Assets"},
          {CREATE_ROLE, "Create new role"},
          {APPEND_ROLE, "Add new role to account"},
          {DETACH_ROLE, "Detach role from account"},
          {GRANT_PERM, "Grant permission over your account"},
          {REVOKE_PERM, "Revoke permission from account"},
          {SET_ACC_KV, "Set account key/value detail"}
          // commands_description_map_
      };

      const auto acc_id = "Account Id";
      const auto ast_id = "Asset Id";
      const auto dom_id = "Domain Id";
      const auto add_amount_str = "Amount to add, e.g 123.456";
      const auto sub_amount_str = "Amount to subtract, e.g 123.456";
      const auto transfer_amount_str = "Amount to transfer, e.g 123.456";
      const auto peer_id = "Full address of a peer";
      const auto pub_key = "Public Key";
      const auto acc_name = "Account Name";
      const auto ast_name = "Asset name";
      const auto ast_precision = "Asset precision";
      const auto quorum = "Quorum";
      const auto role = "Role name";
      const auto perm = "Permission name";

      const auto can_read_self = "Can read all information about their account";
      const auto can_edit_self = "Can change their quorum/signatory";
      const auto can_read_all = "Can read all other accounts";
      const auto can_transfer_receive = "Can receive/transfer assets";
      const auto can_asset_creator = "Can create/add new assets";
      const auto can_roles = "Can create/append roles";
      const auto can_create_domain = "Can create domain";
      const auto can_create_account = "Can create account";

      command_params_map_ = {
          {ADD_ASSET_QTY, makeParamsDescription({ast_id, add_amount_str})},
          {ADD_PEER, makeParamsDescription({peer_id, pub_key})},
          {ADD_SIGN, makeParamsDescription({acc_id, pub_key})},
          {CREATE_ACC, makeParamsDescription({acc_name, dom_id, pub_key})},
          {CREATE_DOMAIN,
           makeParamsDescription({dom_id, std::string("Default ") + role})},
          {CREATE_ASSET,
           makeParamsDescription({ast_name, dom_id, ast_precision})},
          {REMOVE_SIGN, makeParamsDescription({acc_id, pub_key})},
          {SET_QUO, makeParamsDescription({acc_id, quorum})},
          {SUB_ASSET_QTY, makeParamsDescription({ast_id, sub_amount_str})},
          {TRAN_ASSET,
           makeParamsDescription({std::string("Src") + acc_id,
                                  std::string("Dest") + acc_id,
                                  ast_id,
                                  transfer_amount_str})},
          {CREATE_ROLE,
           makeParamsDescription({role,
                                  can_read_self,
                                  can_edit_self,
                                  can_read_all,
                                  can_transfer_receive,
                                  can_asset_creator,
                                  can_create_domain,
                                  can_roles,
                                  can_create_account})},
          {APPEND_ROLE, makeParamsDescription({acc_id, role})},
          {DETACH_ROLE, makeParamsDescription({acc_id, role})},
          {GRANT_PERM, makeParamsDescription({acc_id, perm})},
          {REVOKE_PERM, makeParamsDescription({acc_id, perm})},
          {SET_ACC_KV, makeParamsDescription({acc_id, "key", "value"})}
          // command parameters descriptions
      };

      command_handlers_ = {
          {ADD_ASSET_QTY, &InteractiveTransactionCli::parseAddAssetQuantity},
          {ADD_PEER, &InteractiveTransactionCli::parseAddPeer},
          {ADD_SIGN, &InteractiveTransactionCli::parseAddSignatory},
          {CREATE_ACC, &InteractiveTransactionCli::parseCreateAccount},
          {CREATE_DOMAIN, &InteractiveTransactionCli::parseCreateDomain},
          {CREATE_ASSET, &InteractiveTransactionCli::parseCreateAsset},
          {REMOVE_SIGN, &InteractiveTransactionCli::parseRemoveSignatory},
          {SET_QUO, &InteractiveTransactionCli::parseSetQuorum},
          {SUB_ASSET_QTY,
           &InteractiveTransactionCli::parseSubtractAssetQuantity},
          {TRAN_ASSET, &InteractiveTransactionCli::parseTransferAsset},
          {CREATE_ROLE, &InteractiveTransactionCli::parseCreateRole},
          {APPEND_ROLE, &InteractiveTransactionCli::parseAppendRole},
          {DETACH_ROLE, &InteractiveTransactionCli::parseDetachRole},
          {GRANT_PERM, &InteractiveTransactionCli::parseGrantPermission},
          {REVOKE_PERM, &InteractiveTransactionCli::parseRevokePermission},
          {SET_ACC_KV, &InteractiveTransactionCli::parseSetAccountDetail}
          // Command parsers
      };

      commands_menu_ = formMenu(
          command_handlers_, command_params_map_, commands_description_map_);
      // Add "go back" option
      addBackOption(commands_menu_);
    }

    void InteractiveTransactionCli::createResultMenu() {
      // --- Add result menu points ---

      auto result_desciption = getCommonDescriptionMap();
      const auto ADD_CMD = "add";

      result_desciption.insert(
          {ADD_CMD, "Add one more command to the transaction"});
      result_desciption.insert(
          {BACK_CODE, "Go back and start a new transaction"});

      result_params_map = getCommonParamsMap(default_peer_ip_, default_port_);

      result_params_map.insert({ADD_CMD, {}});
      result_params_map.insert({BACK_CODE, {}});

      result_handlers_ = {
          {SAVE_CODE, &InteractiveTransactionCli::parseSaveFile},
          {SEND_CODE, &InteractiveTransactionCli::parseSendToIroha},
          {ADD_CMD, &InteractiveTransactionCli::parseAddCommand},
          {BACK_CODE, &InteractiveTransactionCli::parseGoBack}
          // Parsers for result
      };

      result_menu_ =
          formMenu(result_handlers_, result_params_map, result_desciption);
    }

    InteractiveTransactionCli::InteractiveTransactionCli(
        const std::string &creator_account,
        const std::string &default_peer_ip,
        int default_port,
        const iroha::keypair_t &keypair,
        logger::LoggerManagerTreePtr response_handler_log_manager,
        logger::LoggerPtr log)
        : current_context_(MAIN),
          creator_(creator_account),
          default_peer_ip_(default_peer_ip),
          default_port_(default_port),
          keypair_(keypair),
          response_handler_log_manager_(
              std::move(response_handler_log_manager)),
          log_(std::move(log)) {
      createCommandMenu();
      createResultMenu();
    }

    void InteractiveTransactionCli::run() {
      bool is_parsing = true;
      current_context_ = MAIN;
      printMenu("Forming a new transactions, choose command to add: ",
                commands_menu_);
      // Creating a new transaction
      while (is_parsing) {
        auto line = promptString("> ");
        if (not line) {
          // The promtSting returns error, terminating symbol
          is_parsing = false;
          break;
        }
        switch (current_context_) {
          case MAIN:
            is_parsing = parseCommand(line.value());
            break;
          case RESULT:
            is_parsing = parseResult(line.value());
            break;
          default:
            // shouldn't get here
            BOOST_ASSERT_MSG(false, "not implemented");
            break;
        }
      }
    }

    bool InteractiveTransactionCli::parseCommand(std::string line) {
      if (isBackOption(line)) {
        // Switch current context
        return false;
      }

      auto res = handleParse<std::shared_ptr<TxCommandApplier>>(
          this, line, command_handlers_, command_params_map_);

      if (not res) {
        // Continue parsing
        return true;
      }

      commands_.push_back(res.value());
      current_context_ = RESULT;
      printMenu("Command is formed. Choose what to do:", result_menu_);
      return true;
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseCreateRole(
        std::vector<std::string> params) {
      auto role = params[0];
      auto read_self = parser::parseValue<bool>(params[1]);
      auto edit_self = parser::parseValue<bool>(params[2]);
      auto read_all = parser::parseValue<bool>(params[3]);
      auto transfer_receive = parser::parseValue<bool>(params[4]);
      auto asset_create = parser::parseValue<bool>(params[5]);
      auto create_domain = parser::parseValue<bool>(params[6]);
      auto roles = parser::parseValue<bool>(params[7]);
      auto create_account = parser::parseValue<bool>(params[8]);

      if (not(read_self and edit_self and read_all and transfer_receive
              and asset_create and create_domain and roles
              and create_account)) {
        std::cout << "Wrong format for permission" << std::endl;
        return nullptr;
      }

      std::unordered_set<RolePermission> perms;
      const auto add = [&perms](std::initializer_list<RolePermission> list) {
        perms.insert(list.begin(), list.end());
      };

      if (read_self.value()) {
        add({RolePermission::can_get_my_account,
             RolePermission::can_get_my_signatories,
             RolePermission::can_get_my_acc_ast,
             RolePermission::can_get_my_acc_detail,
             RolePermission::can_get_my_acc_txs,
             RolePermission::can_get_my_acc_ast_txs,
             RolePermission::can_get_my_txs,
             RolePermission::can_get_my_engine_receipts});
      }
      if (edit_self.value()) {
        add({RolePermission::can_set_quorum,
             RolePermission::can_add_signatory,
             RolePermission::can_remove_signatory,
             RolePermission::can_grant_can_set_my_quorum,
             RolePermission::can_grant_can_add_my_signatory,
             RolePermission::can_grant_can_remove_my_signatory,
             RolePermission::can_grant_can_transfer_my_assets,
             RolePermission::can_grant_can_set_my_account_detail});
      }
      if (read_all.value()) {
        add({RolePermission::can_get_all_accounts,
             RolePermission::can_get_all_signatories,
             RolePermission::can_get_all_acc_ast,
             RolePermission::can_get_all_acc_detail,
             RolePermission::can_get_all_acc_txs,
             RolePermission::can_get_all_acc_ast_txs,
             RolePermission::can_get_all_txs,
             RolePermission::can_get_roles,
             RolePermission::can_read_assets,
             RolePermission::can_get_blocks,
             RolePermission::can_get_all_engine_receipts});
      }
      if (transfer_receive.value()) {
        add({RolePermission::can_transfer, RolePermission::can_receive});
      }
      if (asset_create.value()) {
        add({RolePermission::can_create_asset,
             RolePermission::can_add_asset_qty});
      }
      if (create_domain.value()) {
        add({RolePermission::can_create_domain});
      }
      if (roles.value()) {
        add({RolePermission::can_create_role,
             RolePermission::can_append_role});
      }
      if (create_account.value()) {
        add({RolePermission::can_create_account});
      }

      return makeApplier([role, perms](iroha_lib::Tx &tx) {
        tx.createRole(role, perms);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseAppendRole(
        std::vector<std::string> params) {
      auto account_id = params[0];
      auto role = params[1];
      return makeApplier([account_id, role](iroha_lib::Tx &tx) {
        tx.appendRole(account_id, role);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseDetachRole(
        std::vector<std::string> params) {
      auto account_id = params[0];
      auto role = params[1];
      return makeApplier([account_id, role](iroha_lib::Tx &tx) {
        tx.detachRole(account_id, role);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseGrantPermission(
        std::vector<std::string> params) {
      auto account_id = params[0];
      auto permission = params[1];
      auto grantable = parseGrantablePermission(permission);
      if (not grantable) {
        std::cout << "Unknown permission '" << permission << "'" << std::endl;
        return nullptr;
      }
      return makeApplier([account_id, permission = *grantable](
                             iroha_lib::Tx &tx) {
        tx.grantPermission(account_id, permission);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseRevokePermission(
        std::vector<std::string> params) {
      auto account_id = params[0];
      auto permission = params[1];
      auto grantable = parseGrantablePermission(permission);
      if (not grantable) {
        std::cout << "Unknown permission '" << permission << "'" << std::endl;
        return nullptr;
      }
      return makeApplier([account_id, permission = *grantable](
                             iroha_lib::Tx &tx) {
        tx.revokePermission(account_id, permission);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseAddAssetQuantity(
        std::vector<std::string> params) {
      auto asset_id = params[0];
      auto amount = params[1];
      auto description = params[2];
      return makeApplier([asset_id, amount, description](iroha_lib::Tx &tx) {
        tx.addAssetQuantity(asset_id, amount, description);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseAddPeer(std::vector<std::string> params) {
      auto address = params[0];
      auto key = params[1];
      return makeApplier([address, key](iroha_lib::Tx &tx) {
        tx.addPeer(address, key);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseAddSignatory(
        std::vector<std::string> params) {
      auto account_id = params[0];
      auto key = params[1];
      return makeApplier([account_id, key](iroha_lib::Tx &tx) {
        tx.addSignatory(account_id, key);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseCreateAccount(
        std::vector<std::string> params) {
      auto account_id = params[0];
      auto domain_id = params[1];
      auto key = params[2];
      return makeApplier([account_id, domain_id, key](iroha_lib::Tx &tx) {
        tx.createAccount(account_id, domain_id, key);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseCreateDomain(
        std::vector<std::string> params) {
      auto domain_id = params[0];
      auto user_default_role = params[1];
      return makeApplier([domain_id, user_default_role](iroha_lib::Tx &tx) {
        tx.createDomain(domain_id, user_default_role);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseCreateAsset(
        std::vector<std::string> params) {
      auto asset_name = params[0];
      auto domain_id = params[1];
      auto val = parser::parseValue<uint32_t>(params[2]);
      if (not val) {
        std::cout << "Wrong format for precision" << std::endl;
        return nullptr;
      }
      return makeApplier([asset_name, domain_id, precision = val.value()](
                             iroha_lib::Tx &tx) {
        tx.createAsset(asset_name, domain_id, precision);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseRemoveSignatory(
        std::vector<std::string> params) {
      auto account_id = params[0];
      auto key = params[1];
      return makeApplier([account_id, key](iroha_lib::Tx &tx) {
        tx.removeSignatory(account_id, key);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseSetQuorum(
        std::vector<std::string> params) {
      auto account_id = params[0];
      auto quorum = parser::parseValue<uint64_t>(params[1]);
      if (not quorum) {
        std::cout << "Wrong format for quorum" << std::endl;
        return nullptr;
      }
      return makeApplier([account_id, quorum = quorum.value()](
                             iroha_lib::Tx &tx) {
        tx.setAccountQuorum(account_id,
                            static_cast<uint32_t>(quorum));
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseSubtractAssetQuantity(
        std::vector<std::string> params) {
      auto asset_id = params[0];
      auto amount = params[1];
      auto description = params[2];
      return makeApplier([asset_id, amount, description](iroha_lib::Tx &tx) {
        tx.subtractAssetQuantity(asset_id, amount, description);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseTransferAsset(
        std::vector<std::string> params) {
      auto src_account_id = params[0];
      auto dest_account_id = params[1];
      auto asset_id = params[2];
      auto amount = params[3];
      return makeApplier([src_account_id, dest_account_id, asset_id, amount](
                             iroha_lib::Tx &tx) {
        tx.transferAsset(
            src_account_id, dest_account_id, asset_id, "", amount);
      });
    }

    std::shared_ptr<TxCommandApplier>
    InteractiveTransactionCli::parseSetAccountDetail(
        std::vector<std::string> params) {
      auto account_id = params[0];
      auto key = params[1];
      auto value = params[2];
      return makeApplier([account_id, key, value](iroha_lib::Tx &tx) {
        tx.setAccountDetail(account_id, key, value);
      });
    }

    // --------- Result parsers -------------

    bool InteractiveTransactionCli::parseResult(std::string line) {
      // Find in result handler map
      auto res =
          handleParse<bool>(this, line, result_handlers_, result_params_map);
      return res.get_value_or(true);
    }

    iroha::protocol::Transaction
    InteractiveTransactionCli::buildTransaction() {
      iroha_lib::Tx tx(creator_, keypair_, iroha::time::now(), 1);
      for (const auto &applier : commands_) {
        (*applier)(tx);
      }
      return tx.signAndAddSignature();
    }

    bool InteractiveTransactionCli::parseSendToIroha(
        std::vector<std::string> params) {
      auto address = parseIrohaPeerParams(
          std::move(params), default_peer_ip_, default_port_);
      if (not address) {
        return true;
      }

      // Forming a transaction
      auto tx = buildTransaction();
      // clear commands so that we can start creating new tx
      commands_.clear();

      GrpcResponseHandler response_handler(response_handler_log_manager_);
      response_handler.handle(CliClient(address.value().first,
                                        address.value().second)
                                  .sendTx(tx));

      printTxHash(tx);
      printEnd();
      // Stop parsing
      return false;
    }

    bool InteractiveTransactionCli::parseSaveFile(
        std::vector<std::string> params) {
      auto path = params[0];
      std::ofstream output_file(path);
      if (not output_file) {
        std::cout << "Wrong path" << std::endl;
        // Continue parsing
        return true;
      }

      // Forming a transaction
      auto tx = buildTransaction();

      // clear commands so that we can start creating new tx
      commands_.clear();

      // Convert proto transaction back to the model one for JSON output
      auto model_tx =
          iroha::model::converters::PbTransactionFactory::deserialize(tx);
      if (not model_tx) {
        std::cout << "Cannot serialize transaction" << std::endl;
        return true;
      }

      iroha::model::converters::JsonTransactionFactory json_factory;
      auto json_doc = json_factory.serialize(*model_tx);
      auto json_string =
          iroha::model::converters::jsonToString(json_doc);
      output_file << json_string;
      std::cout << "Successfully saved!" << std::endl;
      printEnd();
      // Stop parsing
      return false;
    }

    bool InteractiveTransactionCli::parseGoBack(
        std::vector<std::string> params) {
      current_context_ = MAIN;
      // Remove all old commands
      commands_.clear();
      printEnd();
      printMenu("Forming a new transaction. Choose command to add: ",
                commands_menu_);
      // Continue parsing
      return true;
    }

    bool InteractiveTransactionCli::parseAddCommand(
        std::vector<std::string> params) {
      current_context_ = MAIN;
      printEnd();
      printMenu("Choose command to add: ", commands_menu_);
      // Continue parsing
      return true;
    }

    void InteractiveTransactionCli::printTxHash(
        const iroha::protocol::Transaction &tx) {
      std::cout
          << "Congratulation, your transaction was accepted for processing."
          << std::endl;
      std::cout << "Its hash is " << iroha::hash(tx).to_hexstring()
                << std::endl;
    }

  }  // namespace interactive
}  // namespace iroha_cli
