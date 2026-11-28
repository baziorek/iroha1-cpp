/// @file iroha-lib.hpp
/// @brief Umbrella header for the whole iroha-lib C++ client library.
/// Includes the gRPC client and all model/command/query/utils headers.
/// For faster compiles, include a granular header instead:
///   - model/transactions.hpp  - Tx, TxBatch, commands
///   - model/queries.hpp       - Query, BlocksQuery, query generators
///   - model/utils.hpp         - genesis, key files, error codes
///   - grpc_client/GrpcClient.hpp - the gRPC client itself
#pragma once

#include "grpc_client/GrpcClient.hpp"
#include "model/all.hpp"
