#pragma once

#include <string>
#include <unordered_map>

#include "interfaces/command_error_codes.hpp"

/**
 * Human-readable descriptions of command error codes.
 * Codes are defined in shared_model/interfaces/command_error_codes.hpp
 * (single source of truth shared with the command executor);
 * command names match the names reported by the node in
 * ToriiResponse::err_or_cmd_name.
 */

inline const std::unordered_map<std::string, std::unordered_map<uint32_t, std::string>>& errorCodesPerCommand()
{
    static const std::unordered_map<std::string, std::unordered_map<uint32_t, std::string>> codes{
        {"CreateDomain", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not create domain (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::create_domain::kAlreadyExists, "Domain already exists"},
            {command_error::create_domain::kNoDefaultRole, "No default role found"},
        }},
        {"CreateAsset", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not create asset"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::create_asset::kNoSuchDomain, "No such domain"},
            {command_error::create_asset::kAlreadyExists, "Asset already exists"},
        }},
        {"AddAssetQuantity", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not add asset quantity (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::add_asset_quantity::kNoSuchAsset, "No such asset or insufficient precision"},
            {command_error::add_asset_quantity::kQuantityOverflow, "Summation overflow (Resulting asset quantity is greater than the system can support)"},
        }},
        {"SubtractAssetQuantity", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not subtract asset quantity (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::subtract_asset_quantity::kNoSuchAsset, "No such asset"},
            {command_error::subtract_asset_quantity::kNegativeQuantity, "Asset quantity cannot be negative"},
        }},
        {"CreateAccount", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not create account"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::create_account::kNoSuchDomain, "No such domain"},
            {command_error::create_account::kAlreadyExists, "Account already exists"},
        }},
        {"TransferAsset", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not transfer asset (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::transfer_asset::kNoSuchSourceAccount, "No such source account"},
            {command_error::transfer_asset::kNoSuchDestinationAccount, "No such destination account"},
            {command_error::transfer_asset::kNoSuchAsset, "No such asset found"},
            {command_error::transfer_asset::kNotEnoughBalance, "Not enough balance"},
            {command_error::transfer_asset::kQuantityOverflow, "Too much asset to transfer"},
            {command_error::transfer_asset::kDescriptionTooLong, "Description is too long"},
        }},
        {"GrantPermission", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not grant permission (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::grant_permission::kNoSuchAccount, "No such account"},
        }},
        {"RevokePermission", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not revoke permission (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::revoke_permission::kNoSuchAccount, "No such account"},
        }},
        {"SetAccountDetail", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not set account detail (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::set_account_detail::kNoSuchAccount, "No such account"},
        }},
        {"CompareAndSetAccountDetail", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not compare and set account detail (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::compare_and_set_account_detail::kNoSuchAccount, "No such account"},
            {command_error::compare_and_set_account_detail::kNoMatchValue, "No match values"},
        }},
        {"CreateRole", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not create role (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::create_role::kAlreadyExists, "Role already exists"},
        }},
        {"AppendRole", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not append role (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::append_role::kNoSuchAccount, "No such account"},
            {command_error::append_role::kNoSuchRole, "No such role"},
        }},
        {"DetachRole", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not detach role (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::detach_role::kNoSuchAccount, "No such account"},
            {command_error::detach_role::kNoSuchRole, "Role is not attached to the account"},
            {command_error::detach_role::kRoleNotFound, "No such role"},
        }},
        {"AddPeer", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not add peer (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
        }},
        {"RemovePeer", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not remove peer (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::remove_peer::kNoSuchPeer, "No such peer"},
            {command_error::remove_peer::kTooFewPeers, "Network size does not allow to remove peer (After removing the peer the network would be empty)"},
        }},
        {"AddSignatory", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not add signatory (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::add_signatory::kNoSuchAccount, "No such account"},
            {command_error::add_signatory::kAlreadyExists, "Signatory already exists"},
        }},
        {"RemoveSignatory", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not remove signatory (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::remove_signatory::kNoSuchAccount, "No such account"},
            {command_error::remove_signatory::kNoSuchSignatory, "No such signatory"},
            {command_error::remove_signatory::kQuorumTooHigh, "Quorum does not allow to remove signatory (After removing the signatory account will be left with less signatories, than its quorum allows)"},
        }},
        {"SetQuorum", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not set quorum (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::set_quorum::kNoSuchAccount, "No such account"},
            {command_error::set_quorum::kNoSignatories, "No signatories on account (Add some signatories before setting quorum)"},
            {command_error::set_quorum::kInvalidQuorum, "New quorum is incorrect (New quorum size is less than account's signatories amount)"},
        }},
        {"CallEngine", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not call engine (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions"},
            {command_error::call_engine::kEngineError, "Contract engine call failed"},
        }},
        {"SetSettingValue", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not set setting value (Internal error happened)"},
            {command_error::kNoPermissions, "No such permissions (SetSettingValue is only allowed in genesis block)"},
        }},
        {"StoreEngineReceiptsResponse", {
            {command_error::kOk, "OK"},
            {command_error::kInternalError, "Could not store engine response (Internal error happened)"},
        }},
    };
    return codes;
}


inline std::string errorCodeToHumanReadable(
        const std::string& command_name,
        uint32_t error_code)
{
    const auto& commands = errorCodesPerCommand();
    const auto it = commands.find(command_name);
    if (it == commands.end()) {
        return "";
    }
    const auto& codes = it->second;
    const auto code_it = codes.find(error_code);
    if (code_it == codes.end()) {
        return "";
    }
    return code_it->second;
}
