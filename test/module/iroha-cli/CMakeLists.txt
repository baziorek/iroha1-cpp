#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(iroha_cli_tx_status_test tx_status_test.cpp)
target_link_libraries(iroha_cli_tx_status_test
        endpoint
        )
target_include_directories(iroha_cli_tx_status_test PRIVATE
        ${PROJECT_SOURCE_DIR}/iroha-cli
        )
