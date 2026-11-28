# Stages and archives the Iroha distribution directory.
# Runs in script mode (-P); invoked by the iroha_dist custom target.
#
#   cmake -DSOURCE_DIR=<repo> -DBUILD_DIR=<build>
#         -DDIST_DIR=<out-parent> -DSTAGE_NAME=<name>
#         -DREADME_FILE=<configured README> -DCONFIG_FILE=<single-node json>
#         -P stage-iroha-dist.cmake
#
# Produces <DIST_DIR>/<archive> where <archive> is
# <STAGE_NAME>.tar.gz (non-Windows) or <STAGE_NAME>.zip (Windows,
# selected via -DARCHIVE_EXT).

foreach(var SOURCE_DIR BUILD_DIR DIST_DIR STAGE_NAME README_FILE CONFIG_FILE)
    if(NOT DEFINED ${var})
        message(FATAL_ERROR "${var} is required")
    endif()
endforeach()
if(NOT DEFINED ARCHIVE_EXT)
    set(ARCHIVE_EXT ".tar.gz")
endif()

set(STAGE "${DIST_DIR}/${STAGE_NAME}")
file(REMOVE_RECURSE "${STAGE}")
file(MAKE_DIRECTORY "${STAGE}/bin" "${STAGE}/example" "${STAGE}/iroha-lib")

# --- SDK: headers + merged lib via the iroha_lib_sdk install component ---
execute_process(
    COMMAND "${CMAKE_COMMAND}" --install "${BUILD_DIR}"
        --prefix "${STAGE}/iroha-lib" --component iroha_lib_sdk
    RESULT_VARIABLE rc
)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "cmake --install failed (rc=${rc})")
endif()

# --- binaries ---
# Located by glob so both single-config (build/bin/irohad) and
# multi-config (build/bin/Release/irohad.exe) layouts work.
set(binaries "irohad;iroha-cli;iroha_migrate;iroha_wsv_diff")
foreach(name IN LISTS binaries)
    file(GLOB_RECURSE hits
        "${BUILD_DIR}/**/${name}"
        "${BUILD_DIR}/**/${name}.exe"
    )
    # drop CMake internals and directories
    set(candidates "")
    foreach(hit IN LISTS hits)
        string(REPLACE "\\" "/" hit "${hit}")
        if(hit MATCHES "/CMakeFiles/")
            continue()
        endif()
        if(IS_DIRECTORY "${hit}")
            continue()
        endif()
        list(APPEND candidates "${hit}")
    endforeach()
    if(NOT candidates)
        message(STATUS "binary not found, skipping: ${name}")
        continue()
    endif()
    list(SORT candidates)
    # prefer a Release/ build when several configurations exist
    set(picked "")
    foreach(c IN LISTS candidates)
        if(c MATCHES "/(Release|RelWithDebInfo|MinSizeRel)/")
            set(picked "${c}")
            break()
        endif()
    endforeach()
    if(NOT picked)
        list(GET candidates 0 picked)
    endif()
    get_filename_component(fname "${picked}" NAME)
    file(COPY_FILE "${picked}" "${STAGE}/bin/${fname}")
    message(STATUS "bin: ${fname}")
endforeach()

# --- single-node example ---
set(example_files
    "genesis.block"
    "node0.pub"
    "node0.priv"
    "admin@test.pub"
    "admin@test.priv"
)
foreach(f IN LISTS example_files)
    if(EXISTS "${SOURCE_DIR}/example/${f}")
        file(COPY_FILE "${SOURCE_DIR}/example/${f}"
             "${STAGE}/example/${f}")
    else()
        message(WARNING "example file missing: example/${f}")
    endif()
endforeach()
if(EXISTS "${CONFIG_FILE}")
    file(COPY_FILE "${CONFIG_FILE}" "${STAGE}/example/config.json")
endif()
if(EXISTS "${README_FILE}")
    file(COPY_FILE "${README_FILE}" "${STAGE}/README.txt")
endif()

# --- archive ---
set(archive "${DIST_DIR}/${STAGE_NAME}${ARCHIVE_EXT}")
file(REMOVE "${archive}")
# cmake -E tar: 'z' enables gzip compression (for .tar.gz); zip is
# compressed natively by the archive writer.
if(ARCHIVE_EXT STREQUAL ".tar.gz")
    set(tar_flags "czf")
else()
    set(tar_flags "cf")
endif()
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar ${tar_flags} "${archive}" "${STAGE_NAME}"
    WORKING_DIRECTORY "${DIST_DIR}"
    RESULT_VARIABLE rc
)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "cmake -E tar failed (rc=${rc})")
endif()
message(STATUS "Created ${archive}")
