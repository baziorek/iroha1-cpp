# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(iroha_lib_model
    BlocksQuery.hpp
    BlocksQuery.cpp
    Query.hpp
    Query.cpp
    Tx.hpp
    Tx.cpp
    TxBatch.hpp
    generators/CommandGenerator.hpp
    generators/CommandGenerator.cpp
    generators/QueryGenerator.hpp
    generators/QueryGenerator.cpp
    utils/Utils.h
    utils/Utils.cpp
    utils/key_files.hpp
    utils/key_files.cpp
    utils/genesis.hpp
    utils/genesis.cpp
    )
target_link_libraries(iroha_lib_model
    grpc_client
    pb_model_converters
    json_model_converters
    keys_manager
    logger_manager
    shared_model_proto_backend
    )
target_include_directories(iroha_lib_model PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    )
