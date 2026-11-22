# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.24)

if (NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # ------------------------------------------------------------------
  # In-tree mode: built as part of the Iroha node project.
  # ------------------------------------------------------------------
  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

  add_subdirectory(grpc_client)
  add_subdirectory(model)
  add_subdirectory(examples)

  target_include_directories(grpc_client PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
      )
  return()
endif()

# --------------------------------------------------------------------
# Standalone mode: build only the Iroha C++ client library.
#
#   cmake -S iroha-lib -B build-iroha-lib \
#       -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake \
#       -DVCPKG_TARGET_TRIPLET=x64-linux
#   cmake --build build-iroha-lib --target iroha_lib_model
#
# Required dependencies (see vcpkg/VCPKG_DEPS_LIST): protobuf, grpc,
# boost, spdlog, rapidjson, gflags, iroha-ed25519.
# --------------------------------------------------------------------

project(iroha_lib VERSION 1.6.0 LANGUAGES CXX)

option(IROHA_LIB_BUILD_EXAMPLES "Build iroha-lib examples" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(TESTING OFF)

# ---- external dependencies ----
find_package(Threads REQUIRED)
find_package(Boost 1.65 REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC 1.21.1 REQUIRED CONFIG)
find_package(RapidJSON 1.1.0 REQUIRED CONFIG)
find_package(spdlog REQUIRED)
if (IROHA_LIB_BUILD_EXAMPLES)
  find_package(gflags 2.2.2 REQUIRED CONFIG)
endif ()

# Boost::boost header-only target (same convention as the main build)
if (NOT TARGET Boost::boost)
  add_library(Boost::boost INTERFACE IMPORTED)
  set_target_properties(Boost::boost PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
      )
endif ()

# RapidJSON interface target (same convention as the main build)
add_library(RapidJSON::rapidjson INTERFACE IMPORTED)
set_target_properties(RapidJSON::rapidjson PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${RAPIDJSON_INCLUDE_DIRS}"
    INTERFACE_COMPILE_DEFINITIONS RAPIDJSON_HAS_STDSTRING=1
    )

# Headers of the libraries we reuse from the repository tree
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../irohad
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared_model
    )

# ---- shared_model (standalone mode: no tests, pulls libs/generator) ----
set(GO_GENERATED_SCHEMA_PATH ${CMAKE_BINARY_DIR}/schema_go)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../shared_model
    ${CMAKE_BINARY_DIR}/shared_model)

# ---- minimal libs subset ----
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libs/common
    ${CMAKE_BINARY_DIR}/libs-common)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libs/crypto
    ${CMAKE_BINARY_DIR}/libs-crypto)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libs/logger
    ${CMAKE_BINARY_DIR}/libs-logger)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libs/multihash
    ${CMAKE_BINARY_DIR}/libs-multihash)

# ---- gRPC endpoint stubs (endpoint.proto lives in shared_model/schema) ----
set(SCHEMA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../shared_model/schema)
set(SM_SCHEMA_PATH ${SCHEMA_PATH})
set(SCHEMA_OUT_DIR ${CMAKE_BINARY_DIR}/schema)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/functions.cmake)

compile_proto_only_grpc_to_cpp(endpoint.proto "-I${SM_SCHEMA_PATH}")

add_library(endpoint STATIC
    ${SCHEMA_OUT_DIR}/endpoint.grpc.pb.cc
    )
target_link_libraries(endpoint
    gRPC::grpc++
    schema
    )
target_include_directories(endpoint PUBLIC
    ${SCHEMA_OUT_DIR}
    )

# ---- model layer used by the converters (subset of irohad/model) ----
add_library(model STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../irohad/model/impl/model_operators.cpp
    )
target_link_libraries(model
    common
    )
target_include_directories(model PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../irohad
    )

# ---- protobuf/json converters ----
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../irohad/model/converters
    ${CMAKE_BINARY_DIR}/model-converters)

# ---- the library itself ----
add_subdirectory(grpc_client)
add_subdirectory(model)

if (IROHA_LIB_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif ()
