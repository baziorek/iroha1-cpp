/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */


#include "interfaces/query_responses/query_response.hpp"
#include "common/visitor.hpp"
#include "interfaces/query_responses/account_asset_response.hpp"
#include "interfaces/query_responses/account_detail_response.hpp"
#include "interfaces/query_responses/account_response.hpp"
#include "interfaces/query_responses/asset_response.hpp"
#include "interfaces/query_responses/block_response.hpp"
#include "interfaces/query_responses/engine_receipts_response.hpp"
#include "interfaces/query_responses/error_query_response.hpp"
#include "interfaces/query_responses/peers_response.hpp"
#include "interfaces/query_responses/pending_transactions_page_response.hpp"
#include "interfaces/query_responses/role_permissions.hpp"
#include "interfaces/query_responses/roles_response.hpp"
#include "interfaces/query_responses/signatories_response.hpp"
#include "interfaces/query_responses/transactions_page_response.hpp"
#include "interfaces/query_responses/transactions_response.hpp"
#include "utils/visitor_apply_for_all.hpp"


namespace shared_model {
  namespace interface {

    std::string QueryResponse::toString() const {
      return iroha::visit(detail::ToStringVisitor(), get());
    }

    bool QueryResponse::operator==(const ModelType &rhs) const {
      return queryHash() == rhs.queryHash() and get() == rhs.get();
    }

  }  // namespace interface
}  // namespace shared_model
