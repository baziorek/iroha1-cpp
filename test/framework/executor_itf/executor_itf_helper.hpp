/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_TEST_FRAMEWORK_EXECUTOR_ITF_HELPER_HPP
#define IROHA_TEST_FRAMEWORK_EXECUTOR_ITF_HELPER_HPP

#include <memory>
#include <type_traits>
#include <variant>

#include <boost/mpl/at.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/map.hpp>
#include "common/result.hpp"
#include "interfaces/commands/command.hpp"
#include "interfaces/queries/query.hpp"

#include "interfaces/query_responses/account_asset_response.hpp"
#include "interfaces/query_responses/account_detail_response.hpp"
#include "interfaces/query_responses/account_response.hpp"
#include "interfaces/query_responses/asset_response.hpp"
#include "interfaces/query_responses/block_error_response.hpp"
#include "interfaces/query_responses/block_query_response.hpp"
#include "interfaces/query_responses/block_response.hpp"
#include "interfaces/query_responses/engine_receipts_response.hpp"
#include "interfaces/query_responses/error_query_response.hpp"
#include "interfaces/query_responses/pending_transactions_page_response.hpp"
#include "interfaces/query_responses/query_response.hpp"
#include "interfaces/query_responses/role_permissions.hpp"
#include "interfaces/query_responses/roles_response.hpp"
#include "interfaces/query_responses/signatories_response.hpp"
#include "interfaces/query_responses/transactions_page_response.hpp"
#include "interfaces/query_responses/transactions_response.hpp"

namespace iroha {
  namespace integration_framework {
    namespace detail {

      /**
       * A type map from specific query to the corresponding response.
       *
       * Is used only to get a default expected response type for some query.
       * The queries may also return different responses, and if you expect a
       * different response type, you should either use the interface of
       * conversion functions to specify it, or extract it from the response
       * variant manually.
       */
      typedef boost::mpl::map<
          boost::mpl::pair<shared_model::interface::GetAccount,
                           shared_model::interface::AccountResponse>,
          boost::mpl::pair<shared_model::interface::GetSignatories,
                           shared_model::interface::SignatoriesResponse>,
          boost::mpl::pair<shared_model::interface::GetAccountTransactions,
                           shared_model::interface::TransactionsResponse>,
          boost::mpl::pair<shared_model::interface::GetAccountAssetTransactions,
                           shared_model::interface::TransactionsResponse>,
          boost::mpl::pair<shared_model::interface::GetTransactions,
                           shared_model::interface::TransactionsResponse>,
          boost::mpl::pair<shared_model::interface::GetAccountAssets,
                           shared_model::interface::AccountAssetResponse>,
          boost::mpl::pair<shared_model::interface::GetAccountDetail,
                           shared_model::interface::AccountDetailResponse>,
          boost::mpl::pair<shared_model::interface::GetRoles,
                           shared_model::interface::RolesResponse>,
          boost::mpl::pair<shared_model::interface::GetRolePermissions,
                           shared_model::interface::RolePermissionsResponse>,
          boost::mpl::pair<shared_model::interface::GetAssetInfo,
                           shared_model::interface::AssetResponse>,
          boost::mpl::pair<
              shared_model::interface::GetPendingTransactions,
              shared_model::interface::PendingTransactionsPageResponse>,
          boost::mpl::pair<shared_model::interface::GetBlock,
                           shared_model::interface::BlockResponse>,
          boost::mpl::pair<shared_model::interface::GetEngineReceipts,
                           shared_model::interface::EngineReceiptsResponse>>
          SpecificQueryResponses;

      /// whether Variant holds an alternative equivalent to const T &
      /// (variant alternatives are std::reference_wrapper<const T>)
      template <typename Variant, typename T>
      struct is_variant_alternative {
        template <std::size_t... Is>
        static constexpr bool impl(std::index_sequence<Is...>) {
          return (std::is_same_v<
                      std::remove_cv_t<std::remove_reference_t<
                          std::variant_alternative_t<Is, Variant>>>,
                      std::reference_wrapper<const std::decay_t<T>>> ||
                  ...);
        }

        static constexpr bool value =
            impl(std::make_index_sequence<std::variant_size_v<Variant>>{});
      };

      /// true for specific commands
      template <typename T>
      constexpr bool isSpecificCommand =
          is_variant_alternative<
              shared_model::interface::Command::CommandVariantType,
              T>::value;

      /// true for specific queries
      template <typename T>
      constexpr bool isSpecificQuery =
          is_variant_alternative<
              shared_model::interface::Query::QueryVariantType,
              T>::value;

      template <typename T>
      std::enable_if_t<isSpecificQuery<T>, const T &> getInterfaceQueryRef(
          const T &query) {
        return query;
      }

      template <typename T, typename SpecificQuery = typename T::ModelType>
      std::enable_if_t<isSpecificQuery<SpecificQuery>, const SpecificQuery &>
      getInterfaceQueryRef(const T &query) {
        return static_cast<const SpecificQuery &>(query);
      }

      template <typename T>
      using InterfaceQuery = decltype(getInterfaceQueryRef(std::declval<T>()));

      /// response for specific query
      template <typename SpecificQuery,
                typename = std::enable_if_t<isSpecificQuery<SpecificQuery>>>
      using GetSpecificQueryResponse =
          typename boost::mpl::at<SpecificQueryResponses,
                                  std::decay_t<SpecificQuery>>::type;

      /**
       * Try to extract the given specific response from a general query
       * response.
       * @tparam SpecificQueryResponse The specific response type to extract.
       * @param query_result The general query response.
       * @return The specific response if the general one has it, the general
       * response if not.
       */
      template <typename SpecificQueryResponse>
      iroha::expected::Result<const SpecificQueryResponse &,
                              iroha::ametsuchi::QueryExecutorResult &>
      convertToSpecificQueryResponse(
          iroha::ametsuchi::QueryExecutorResult &query_result) {
        if (auto specific_query_response = std::get_if<
                std::reference_wrapper<const SpecificQueryResponse>>(
                &query_result->get())) {
          return iroha::expected::makeValue<const SpecificQueryResponse &>(
              specific_query_response->get());
        }
        return iroha::expected::makeError<
            iroha::ametsuchi::QueryExecutorResult &>(query_result);
      }
    }  // namespace detail
  }    // namespace integration_framework
}  // namespace iroha

#endif  // IROHA_TEST_FRAMEWORK_EXECUTOR_ITF_HELPER_HPP
