#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(iroha_lib_examples_test iroha_lib_examples_test.cpp)
# Each TEST_F boots a dedicated Iroha node via IntegrationTestFramework;
# 7 nodes + example runs need well over the 120s default integration timeout.
set_tests_properties(integration_iroha_lib_examples_test PROPERTIES
    TIMEOUT 900)
target_link_libraries(iroha_lib_examples_test
    integration_framework
    )

# Paths of the example executables, resolved at generate time.
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/iroha_lib_examples_paths.h
    CONTENT [==[
#pragma once
#define IROHA_EXAMPLE_TX "$<TARGET_FILE:irohalib_TxExample_example>"
#define IROHA_EXAMPLE_QUERY "$<TARGET_FILE:irohalib_QueryExample_example>"
#define IROHA_EXAMPLE_TX_ORDERING "$<TARGET_FILE:irohalib_TxOrderingExample_example>"
#define IROHA_EXAMPLE_BLOCKS_QUERY "$<TARGET_FILE:irohalib_BlocksQueryExample_example>"
#define IROHA_EXAMPLE_BATCH "$<TARGET_FILE:irohalib_BatchExample_example>"
#define IROHA_EXAMPLE_MST "$<TARGET_FILE:irohalib_MstExample_example>"
#define IROHA_EXAMPLE_INFINITE_BLOCKS_STREAM "$<TARGET_FILE:irohalib_InfiniteBlocksStreamExample_example>"
]==]
)
target_include_directories(iroha_lib_examples_test PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

# make sure the examples are built before the test runs
add_dependencies(iroha_lib_examples_test
    irohalib_TxExample_example
    irohalib_QueryExample_example
    irohalib_TxOrderingExample_example
    irohalib_BlocksQueryExample_example
    irohalib_BatchExample_example
    irohalib_MstExample_example
    irohalib_InfiniteBlocksStreamExample_example
    )
