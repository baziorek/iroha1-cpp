#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(iroha_lib_utils_test utils_test.cpp)
target_link_libraries(iroha_lib_utils_test
        iroha_lib_model
        test_logger
        )
