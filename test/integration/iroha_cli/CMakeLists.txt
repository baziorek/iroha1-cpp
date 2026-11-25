#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(iroha_cli_test iroha_cli_test.cpp)
# The JsonTransactionAndQuery case boots a dedicated Iroha node via
# IntegrationTestFramework; allow well over the default integration timeout.
set_tests_properties(integration_iroha_cli_test PROPERTIES
    TIMEOUT 900)
target_link_libraries(iroha_cli_test
    integration_framework
    iroha_lib
    pb_model_converters
    json_model_converters
    )

# Path of the iroha-cli executable, resolved at generate time.
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/iroha_cli_paths.h
    CONTENT [==[
#pragma once
#define IROHA_CLI_BIN "$<TARGET_FILE:iroha-cli>"
]==]
)
target_include_directories(iroha_cli_test PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

# make sure the CLI is built before the test runs
add_dependencies(iroha_cli_test iroha-cli)
