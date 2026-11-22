/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */


#include "interfaces/queries/query.hpp"
#include "common/visitor.hpp"
#include "interfaces/queries/get_account.hpp"
#include "interfaces/queries/get_account_asset_transactions.hpp"
#include "interfaces/queries/get_account_assets.hpp"
#include "interfaces/queries/get_account_detail.hpp"
#include "interfaces/queries/get_account_transactions.hpp"
#include "interfaces/queries/get_asset_info.hpp"
#include "interfaces/queries/get_block.hpp"
#include "interfaces/queries/get_engine_receipts.hpp"
#include "interfaces/queries/get_peers.hpp"
#include "interfaces/queries/get_pending_transactions.hpp"
#include "interfaces/queries/get_role_permissions.hpp"
#include "interfaces/queries/get_roles.hpp"
#include "interfaces/queries/get_signatories.hpp"
#include "interfaces/queries/get_transactions.hpp"
#include "interfaces/queries/query_payload_meta.hpp"
#include "utils/visitor_apply_for_all.hpp"


namespace shared_model {
  namespace interface {

    std::string Query::toString() const {
      return detail::PrettyStringBuilder()
          .init("Query")
          .appendNamed("creatorId", creatorAccountId())
          .appendNamed("queryCounter", queryCounter())
          .append(Signable::toString())
          .append(iroha::visit(detail::ToStringVisitor(), get()))
          .finalize();
    }

    bool Query::operator==(const ModelType &rhs) const {
      return this->get() == rhs.get();
    }

  }  // namespace interface
}  // namespace shared_model
