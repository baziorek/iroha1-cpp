/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_COMMAND_ERROR_CODES_HPP
#define IROHA_SHARED_MODEL_COMMAND_ERROR_CODES_HPP

#include <cstdint>

/**
 * Canonical numeric codes returned by command executors
 * (CommandError::error_code / ToriiResponse::error_code).
 *
 * Codes 0-2 have the same meaning for every command:
 *   0 - success, 1 - internal error, 2 - no permissions.
 * Codes >= 3 are command-specific and listed in the per-command
 * namespaces below. The values mirror what the Postgres command
 * executor emits; see docs/source/develop/api/commands.rst for
 * the documented meanings.
 */
namespace command_error {

inline constexpr std::uint32_t kOk = 0;
inline constexpr std::uint32_t kInternalError = 1;
inline constexpr std::uint32_t kNoPermissions = 2;

namespace add_asset_quantity {
// note: the checks CTE also emits code 1 when the creator account row is
// missing; code 1 is the universal "internal error" bucket, so it is not
// listed here separately.
inline constexpr std::uint32_t kNoSuchAsset = 3;     ///< asset missing or precision too low
inline constexpr std::uint32_t kQuantityOverflow = 4;///< resulting quantity exceeds supported range
}  // namespace add_asset_quantity

namespace add_peer {
// only universal codes 0, 1, 2 are emitted
}  // namespace add_peer

namespace add_signatory {
inline constexpr std::uint32_t kNoSuchAccount = 3;   ///< account does not exist
inline constexpr std::uint32_t kAlreadyExists = 4;   ///< (account, public_key) pair exists (via SQL error map)
}  // namespace add_signatory

namespace append_role {
inline constexpr std::uint32_t kNoSuchAccount = 3;   ///< account does not exist (via SQL error map)
inline constexpr std::uint32_t kNoSuchRole = 4;      ///< role does not exist
}  // namespace append_role

namespace add_sync_peer {
// only universal codes 0, 1, 2 are emitted
}  // namespace add_sync_peer

namespace compare_and_set_account_detail {
inline constexpr std::uint32_t kNoSuchAccount = 3;   ///< target account does not exist
inline constexpr std::uint32_t kNoMatchValue = 4;    ///< expected_value does not match stored detail
}  // namespace compare_and_set_account_detail

namespace create_account {
inline constexpr std::uint32_t kNoSuchDomain = 3;   ///< domain has no default role
inline constexpr std::uint32_t kAlreadyExists = 4;   ///< (via SQL error map)
}  // namespace create_account

namespace create_asset {
inline constexpr std::uint32_t kNoSuchDomain = 3;   ///< (via SQL error map)
inline constexpr std::uint32_t kAlreadyExists = 4;   ///< (via SQL error map)
}  // namespace create_asset

namespace create_domain {
inline constexpr std::uint32_t kAlreadyExists = 3;   ///< (via SQL error map)
inline constexpr std::uint32_t kNoDefaultRole = 4;   ///< (via SQL error map)
}  // namespace create_domain

namespace create_role {
inline constexpr std::uint32_t kAlreadyExists = 3;   ///< (via SQL error map); note: SQL CASE emits 2 for existing role
}  // namespace create_role

namespace detach_role {
inline constexpr std::uint32_t kNoSuchAccount = 3;   ///< (via SQL error map)
inline constexpr std::uint32_t kNoSuchRole = 4;      ///< role is not attached to the account
inline constexpr std::uint32_t kRoleNotFound = 5;    ///< role does not exist (via SQL error map)
}  // namespace detach_role

namespace grant_permission {
inline constexpr std::uint32_t kNoSuchAccount = 3;   ///< (via SQL error map)
}  // namespace grant_permission

namespace remove_peer {
inline constexpr std::uint32_t kNoSuchPeer = 3;      ///< peer not found
inline constexpr std::uint32_t kTooFewPeers = 4;     ///< removal would leave network without peers
}  // namespace remove_peer

namespace remove_signatory {
inline constexpr std::uint32_t kNoSuchAccount = 3;   ///< account does not exist
inline constexpr std::uint32_t kNoSuchSignatory = 4; ///< public key is not a signatory
inline constexpr std::uint32_t kQuorumTooHigh = 5;   ///< removal would violate quorum
}  // namespace remove_signatory

namespace remove_sync_peer {
inline constexpr std::uint32_t kNoSuchPeer = 3;      ///< sync peer not found
inline constexpr std::uint32_t kTooFewPeers = 4;     ///< removal would leave no sync peers
}  // namespace remove_sync_peer

namespace revoke_permission {
inline constexpr std::uint32_t kNoSuchAccount = 3;   ///< (via SQL error map)
}  // namespace revoke_permission

namespace set_account_detail {
inline constexpr std::uint32_t kNoSuchAccount = 3;   ///< target account does not exist
}  // namespace set_account_detail

namespace set_quorum {
inline constexpr std::uint32_t kNoSuchAccount = 3;   ///< account does not exist (via SQL error map)
inline constexpr std::uint32_t kNoSignatories = 4;   ///< account has no signatories
inline constexpr std::uint32_t kInvalidQuorum = 5;   ///< quorum exceeds signatory count
}  // namespace set_quorum

namespace store_engine_response {
// only universal codes 0, 1 are emitted
}  // namespace store_engine_response

namespace subtract_asset_quantity {
inline constexpr std::uint32_t kNoSuchAsset = 3;     ///< asset missing on account
inline constexpr std::uint32_t kNegativeQuantity = 4;///< subtraction would make quantity negative
}  // namespace subtract_asset_quantity

namespace transfer_asset {
inline constexpr std::uint32_t kNoSuchSourceAccount = 3;   ///< checks CTE
inline constexpr std::uint32_t kNoSuchDestinationAccount = 4; ///< checks CTE
inline constexpr std::uint32_t kNoSuchAsset = 5;            ///< checks CTE
inline constexpr std::uint32_t kNotEnoughBalance = 6;       ///< checks CTE
inline constexpr std::uint32_t kQuantityOverflow = 7;       ///< checks CTE
inline constexpr std::uint32_t kDescriptionTooLong = 8;     ///< checks CTE
}  // namespace transfer_asset

namespace call_engine {
inline constexpr std::uint32_t kEngineError = 3;  ///< contract engine call failed
}  // namespace call_engine

namespace set_setting_value {
// only universal codes are emitted (2 = updates only allowed in genesis block)
}  // namespace set_setting_value

}  // namespace command_error

#endif  // IROHA_SHARED_MODEL_COMMAND_ERROR_CODES_HPP
