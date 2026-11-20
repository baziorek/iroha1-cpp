/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_GRPC_SERVERCONTEXT_DTOR_SEGV_WORKAROUND_HPP
#define IROHA_GRPC_SERVERCONTEXT_DTOR_SEGV_WORKAROUND_HPP

// grpc::internal::GrpcLibraryInitializer was removed in gRPC 1.6x;
// grpc::internal::GrpcLibrary (a base class of channels/servers) now handles
// initialization automatically, so no explicit initializer is needed here.

#endif  // IROHA_GRPC_SERVERCONTEXT_DTOR_SEGV_WORKAROUND_HPP
