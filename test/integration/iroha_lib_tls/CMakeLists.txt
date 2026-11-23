#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(iroha_lib_tls_test iroha_lib_tls_test.cpp)
# Boots a dedicated Iroha node with the TLS torii endpoint enabled.
set_tests_properties(integration_iroha_lib_tls_test PROPERTIES
    TIMEOUT 600)
target_link_libraries(iroha_lib_tls_test
    integration_framework
    )

# TLS test certificate: server uses <path>.crt + <path>.key,
# clients verify against <path>.crt.
set(IROHA_TLS_KEY_PATH "${CMAKE_SOURCE_DIR}/test/system/tls/correct")

# Paths resolved at generate time.
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/iroha_lib_tls_paths.h
    CONTENT [==[
#pragma once
#define IROHA_EXAMPLE_TLS "$<TARGET_FILE:irohalib_TlsExample_example>"
]==]
)
target_include_directories(iroha_lib_tls_test PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})
target_compile_definitions(iroha_lib_tls_test PRIVATE
    IROHA_TLS_KEY_PATH="${IROHA_TLS_KEY_PATH}"
    IROHA_TLS_CERT_PATH="${IROHA_TLS_KEY_PATH}.crt"
    IROHA_TLS_KEY_FILE="${IROHA_TLS_KEY_PATH}.key"
    )

add_dependencies(iroha_lib_tls_test
    irohalib_TlsExample_example
    )
