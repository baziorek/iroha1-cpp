# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.14.0)

## CMP0091 (CMake >= 3.15): honour CMAKE_MSVC_RUNTIME_LIBRARY below;
## with the 3.14 minimum it stays OLD and MSVC silently keeps /MD.
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

## Dependencies are provided by vcpkg static triplets (e.g.
## x64-windows-static) built with /MT; CMake's MSVC default is /MD,
## which fails at link time with LNK2038 RuntimeLibrary mismatch.
## Must be set before project()/enable_language() to take effect.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  message(STATUS "ccache enabled (${CCACHE_PROGRAM})")
endif()

PROJECT(iroha
  VERSION 1.6.0
  LANGUAGES C CXX)

SET(CMAKE_CXX_STANDARD 23)

SET(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
if (NOT MSVC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fdiagnostics-color=always")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wno-error=deprecated-declarations")
  SET(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -g -Wextra -Wno-unused-parameter -Wno-deprecated-declarations -O0")
else()
  add_compile_options(/bigobj)
endif()
SET(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
SET(CMAKE_INSTALL_RPATH "../lib")
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)  ## use for code static analyze and vscode cpp plugin

if(CMAKE_BUILD_TYPE MATCHES "Release")
  include(CheckIPOSupported)
  check_ipo_supported(RESULT lto_supported OUTPUT error)

  if(lto_supported)
    message(STATUS "IPO / LTO enabled")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(STATUS "IPO / LTO not supported: <${error}>")
  endif()
endif()

## FIXME revert this after change CI to GitHub actions.
## Temporary fix for 'bytecode stream version incompatible' between gcc-9 and gcc-10 and clang
## when dependencies were build via vcpkg with default GCC9 could not be linked with iroha built with GCC-10
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)

if(WIN32)
    # We have to set _WIN32_WINNT for gRPC
    if(${CMAKE_SYSTEM_VERSION} EQUAL 10) # Windows 10
        add_definitions(-D _WIN32_WINNT=0x0A00)
    elseif(${CMAKE_SYSTEM_VERSION} EQUAL 6.3) # Windows 8.1
        add_definitions(-D _WIN32_WINNT=0x0603)
    elseif(${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        add_definitions(-D _WIN32_WINNT=0x0602)
    elseif(${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        add_definitions(-D _WIN32_WINNT=0x0601)
    elseif(${CMAKE_SYSTEM_VERSION} EQUAL 6.0) # Windows Vista
        add_definitions(-D _WIN32_WINNT=0x0600)
    else() # Windows XP (5.1)
        add_definitions(-D _WIN32_WINNT=0x0501)
    endif()
endif()

# workaround to prevent user-installed dependencies having higher precedence
# than the ones downloaded with externalproject
if(UNIX)
  list(APPEND CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES
    /usr/local/include
    )
  list(APPEND CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES
    /usr/local/include
    )
endif()

if(COVERAGE)
  find_program(LCOV_PROGRAM lcov)
  if(NOT LCOV_PROGRAM)
    message(FATAL_ERROR "lcov not found! Aborting...")
  endif()
  find_file(LCOV_CONFIG_FILE .lcovrc ${PROJECT_SOURCE_DIR})
  if(NOT LCOV_CONFIG_FILE)
    message(FATAL_ERROR "lcov config file not found in project root! Aborting...")
  endif()
  message(STATUS "lcov enabled (${LCOV_PROGRAM})")
  # remove -g flag to reduce binary size
  list(REMOVE_ITEM CMAKE_CXX_FLAGS -g)
  set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
  set(CMAKE_CXX_FLAGS "--coverage ${CMAKE_CXX_FLAGS}")
  set(CMAKE_C_FLAGS "--coverage ${CMAKE_C_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "--coverage ${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "--coverage ${CMAKE_EXE_LINKER_FLAGS}")
  add_custom_target(coverage.init.info
      COMMAND ${LCOV_PROGRAM} --config-file ${LCOV_CONFIG_FILE} -o ${PROJECT_BINARY_DIR}/reports/coverage.init.info -c -i -d ${PROJECT_BINARY_DIR}
      )
  add_custom_target(coverage.info
      COMMAND ${LCOV_PROGRAM} --config-file ${LCOV_CONFIG_FILE} -o ${PROJECT_BINARY_DIR}/reports/coverage.info -c -d ${PROJECT_BINARY_DIR}
      COMMAND ${LCOV_PROGRAM} --config-file ${LCOV_CONFIG_FILE} -o ${PROJECT_BINARY_DIR}/reports/coverage.info -a ${PROJECT_BINARY_DIR}/reports/coverage.init.info -a ${PROJECT_BINARY_DIR}/reports/coverage.info
      COMMAND ${LCOV_PROGRAM} --config-file ${LCOV_CONFIG_FILE} -o ${PROJECT_BINARY_DIR}/reports/coverage.info -r ${PROJECT_BINARY_DIR}/reports/coverage.info '/usr*' '/Library/Developer/CommandLineTools/*' '${PROJECT_BINARY_DIR}/*' '${CMAKE_BINARY_DIR}/*' '${CMAKE_SOURCE_DIR}/test/*'
      )
  set(REPORT_DIR ${CMAKE_BINARY_DIR}/reports)
  file(MAKE_DIRECTORY ${REPORT_DIR})
  include(cmake/analysis.cmake)
endif()

include(cmake/functions.cmake)

if(SANITIZE_THREAD)
  append_build_flags(-fsanitize=thread)
endif()
if(SANITIZE_ADDRESS)
  append_build_flags(-fsanitize=address -fno-omit-frame-pointer)
endif()
if(SANITIZE_MEMORY)
  append_build_flags(-fsanitize=memory -fsanitize-memory-track-origins)
endif()
if(SANITIZE_UNDEFINED)
  append_build_flags(-fsanitize=undefined)
endif()
set(DEPS_CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -G${CMAKE_GENERATOR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    )

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

option(BENCHMARKING          "Build benchmarks"                         OFF)
option(TESTING               "Build tests"                              ON )
option(USE_BTF               "Build Binary Testing Framework"           OFF)
option(COVERAGE              "Enable coverage"                          OFF)
option(FUZZING               "Build fuzzing binaries"                   OFF)
option(PACKAGE_ZIP           "Create ZIP package"                       OFF)
option(PACKAGE_TGZ           "Create TGZ package"                       OFF)
option(PACKAGE_RPM           "Create RPM package"                       OFF)
option(PACKAGE_DEB           "Create DEB package"                       OFF)
option(USE_LIBURSA           "Use Hyperledger Ursa cryptography (deprecated: Ursa is EOL since 27 Apr 2023)" OFF)
option(USE_BURROW            "Use Hyperledger Burrow EVM (deprecated: Burrow is EOL since 12 May 2022)"        OFF)
option(SANITIZE_THREAD       "Build with thread sanitizer"              OFF)
option(SANITIZE_ADDRESS      "Build with address sanitizer"             OFF)
option(SANITIZE_MEMORY       "Build with memory sanitizer"              OFF)
option(SANITIZE_UNDEFINED    "Build with undefined behaviour sanitizer" OFF)

if(USE_LIBURSA)
  message(DEPRECATION "USE_LIBURSA is deprecated: Hyperledger Ursa reached End of Life on 27 April 2023 and is no longer maintained. Consider migrating to the default ed25519/sha3 cryptography.")
endif()
if(USE_BURROW)
  message(DEPRECATION "USE_BURROW is deprecated: Hyperledger Burrow reached End of Life on 12 May 2022 and is no longer maintained.")
endif()


if (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_GENERATOR MATCHES "Make")
  set(MAKE "$(MAKE)")
else()
  set(MAKE make)
endif()

message(STATUS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message(STATUS "-DTESTING=${TESTING}")
message(STATUS "-DUSE_BTF=${USE_BTF}")
message(STATUS "-DBENCHMARKING=${BENCHMARKING}")
message(STATUS "-DFUZZING=${FUZZING}")
message(STATUS "-DCOVERAGE=${COVERAGE}")
message(STATUS "-DPACKAGE_ZIP=${PACKAGE_ZIP}")
message(STATUS "-DPACKAGE_TGZ=${PACKAGE_TGZ}")
message(STATUS "-DPACKAGE_RPM=${PACKAGE_RPM}")
message(STATUS "-DPACKAGE_DEB=${PACKAGE_DEB}")
message(STATUS "-DSANITIZE_THREAD=${SANITIZE_THREAD}")
message(STATUS "-DSANITIZE_ADDRESS=${SANITIZE_ADDRESS}")
message(STATUS "-DSANITIZE_MEMORY=${SANITIZE_MEMORY}")
message(STATUS "-DSANITIZE_UNDEFINED=${SANITIZE_UNDEFINED}")

set(IROHA_SCHEMA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/schema")
set(SM_SCHEMA_DIR "${PROJECT_SOURCE_DIR}/shared_model/schema")
set(SCHEMA_OUT_DIR ${CMAKE_BINARY_DIR}/schema)
set(GO_GENERATED_SCHEMA_PATH "${CMAKE_BINARY_DIR}/schema_go")
include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/shared_model
  ${PROJECT_SOURCE_DIR}/irohad
  ${PROJECT_SOURCE_DIR}/libs
)

SET(IROHA_ROOT_PROJECT ON)

# Boost uses RTTI to perform some actions (such as type erasure).
# This is slow. This flag forces boost to use other methods,
# which are generally faster
add_definitions(-DBOOST_NO_RTTI)

include(FeatureSummary)
include(cmake/dependencies.cmake)
include(cmake/clang-cxx-dev-tools.cmake)

if(PACKAGE_TGZ OR PACKAGE_ZIP OR PACKAGE_RPM OR PACKAGE_DEB)
  include(cmake/release.cmake)
endif()

add_subdirectory(schema)
if(USE_BURROW)
  add_subdirectory(goSrc/src/vmCaller)
endif()
add_subdirectory(libs)
add_subdirectory(irohad)
add_subdirectory(iroha-cli)
add_subdirectory(iroha-lib)
add_subdirectory(shared_model)

if(TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

# ---- merged single static library (iroha_lib_all) ----
# Combines every project STATIC_LIBRARY target into one archive so that
# consumers of iroha-lib link a single file instead of ~100 granular
# archives in a fragile link order. Test/fuzz/benchmark subtrees are
# excluded. On multi-config generators the merged file is shared between
# configurations (last build wins) — it is intended for packaging.
function(_iroha_collect_static_libs dir out_var)
    get_filename_component(dir "${dir}" REALPATH)
    if(dir MATCHES "/(test|fuzz|benchmark)(/|$)")
        return()
    endif()
    get_property(_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    foreach(_tgt IN LISTS _targets)
        get_target_property(_type ${_tgt} TYPE)
        if(_type STREQUAL "STATIC_LIBRARY")
            list(APPEND _collected ${_tgt})
        endif()
    endforeach()
    get_property(_subs DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(_sub IN LISTS _subs)
        get_filename_component(_sub "${_sub}" REALPATH)
        _iroha_collect_static_libs(${_sub} _collected)
    endforeach()
    set(${out_var} "${_collected}" PARENT_SCOPE)
endfunction()

set(_iroha_static_libs "")
_iroha_collect_static_libs(${CMAKE_SOURCE_DIR} _iroha_static_libs)
list(REMOVE_DUPLICATES _iroha_static_libs)

if(MSVC)
    set(_iroha_all_name "iroha_lib_all.lib")
    set(_iroha_merge_mode "lib")
else()
    set(_iroha_all_name "libiroha_all.a")
    set(_iroha_merge_mode "ar")
endif()
set(_iroha_all_output "${CMAKE_BINARY_DIR}/lib/${_iroha_all_name}")

add_custom_command(
    OUTPUT "${_iroha_all_output}"
    COMMAND ${CMAKE_COMMAND}
        -DBUILD_DIR=${CMAKE_BINARY_DIR}
        -DOUTPUT_FILE=${_iroha_all_output}
        -DMERGE_MODE=${_iroha_merge_mode}
        -DMERGE_TOOL=${CMAKE_CXX_COMPILER_AR}
        -P ${CMAKE_SOURCE_DIR}/cmake/merge-static-libs.cmake
    DEPENDS ${_iroha_static_libs}
    COMMENT "Merging Iroha static libraries into ${_iroha_all_name}"
    VERBATIM
)
add_custom_target(iroha_lib_all ALL DEPENDS "${_iroha_all_output}")

# ---- iroha-lib SDK install rules ----
# `cmake --install <build> --prefix <dir>` produces:
#   <dir>/include/iroha-lib/      public iroha-lib headers (+ umbrella)
#   <dir>/include/shared_model/   shared_model headers used by the API
#   <dir>/include/libs/           libs/ headers used by the API
#   <dir>/include/irohad/         irohad headers used by the API
#   <dir>/include/gen/schema/     generated protobuf/gRPC headers
#   <dir>/lib/libiroha_all.a      merged archive (libiroha_all.lib on MSVC)
# Package scripts (packaging/linux/package-linux.sh,
# packaging/windows/package-windows.ps1) use these rules for the SDK part.
install(DIRECTORY ${PROJECT_SOURCE_DIR}/iroha-lib/
        DESTINATION include/iroha-lib
        COMPONENT iroha_lib_sdk
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
        PATTERN "examples" EXCLUDE)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/shared_model/
        DESTINATION include/shared_model
        COMPONENT iroha_lib_sdk
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
        PATTERN "schema" EXCLUDE)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/libs/
        DESTINATION include/libs
        COMPONENT iroha_lib_sdk
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/irohad/
        DESTINATION include/irohad
        COMPONENT iroha_lib_sdk
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
install(DIRECTORY ${CMAKE_BINARY_DIR}/schema/
        DESTINATION include/gen/schema
        COMPONENT iroha_lib_sdk
        FILES_MATCHING PATTERN "*.pb.h" PATTERN "*.grpc.pb.h"
        PATTERN "CMakeFiles" EXCLUDE)
install(FILES "${_iroha_all_output}"
        DESTINATION lib
        COMPONENT iroha_lib_sdk)
