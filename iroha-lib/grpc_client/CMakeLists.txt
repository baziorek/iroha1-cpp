# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(grpc_client
    GrpcClient.hpp
    GrpcClient.cpp
    )
target_link_libraries(grpc_client
    endpoint
    )
target_include_directories(grpc_client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
