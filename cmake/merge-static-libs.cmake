# Merges all Iroha project static libraries into a single archive.
# Runs in script mode (-P); invoked by the iroha_lib_all custom target.
#
#   cmake -DBUILD_DIR=<build> -DOUTPUT_FILE=<out> -DMERGE_MODE=<ar|lib>
#         [-DMERGE_TOOL=<path-to-ar-or-lib.exe>] -P merge-static-libs.cmake
#
# MERGE_MODE=ar  : GNU ar — objects are extracted and re-archived with
#                  unique member names (ar stores only basenames).
# MERGE_MODE=lib : MSVC lib.exe — concatenates archives directly.

if(NOT DEFINED BUILD_DIR OR NOT DEFINED OUTPUT_FILE OR NOT DEFINED MERGE_MODE)
    message(FATAL_ERROR "BUILD_DIR, OUTPUT_FILE and MERGE_MODE are required")
endif()
if(NOT DEFINED MERGE_TOOL)
    set(MERGE_TOOL ar)
endif()

# Normalize to forward slashes for matching
set(BUILD_DIR_NORM "${BUILD_DIR}")
string(REPLACE "\\" "/" BUILD_DIR_NORM "${BUILD_DIR_NORM}")

file(GLOB_RECURSE archives "${BUILD_DIR_NORM}/*.a" "${BUILD_DIR_NORM}/*.lib")

set(libs "")
foreach(archive IN LISTS archives)
    string(REPLACE "\\" "/" archive "${archive}")
    # skip CMake internals and test/fuzz trees
    if(archive MATCHES "/CMakeFiles/")
        continue()
    endif()
    if(archive MATCHES "/(test|fuzz|benchmark)/")
        continue()
    endif()
    # skip import libraries sitting next to an .exe of the same name
    get_filename_component(dir "${archive}" DIRECTORY)
    get_filename_component(base "${archive}" NAME_WE)
    if(EXISTS "${dir}/${base}.exe")
        continue()
    endif()
    list(APPEND libs "${archive}")
endforeach()

list(LENGTH libs nlibs)
if(nlibs EQUAL 0)
    message(FATAL_ERROR "no static libraries found under ${BUILD_DIR}")
endif()
message(STATUS "Merging ${nlibs} archives into ${OUTPUT_FILE}")

if(MERGE_MODE STREQUAL "lib")
    # MSVC: lib.exe /OUT merges archives in one step
    execute_process(
        COMMAND "${MERGE_TOOL}" /NOLOGO /OUT:"${OUTPUT_FILE}" ${libs}
        RESULT_VARIABLE rc
    )
    if(NOT rc EQUAL 0)
        message(FATAL_ERROR "${MERGE_TOOL} failed with ${rc}")
    endif()
else()
    # GNU ar: unique member names are required because ar stores only
    # basenames and different modules can contain same-named objects.
    set(tmp "${OUTPUT_FILE}.merge-tmp")
    file(REMOVE_RECURSE "${tmp}")
    file(MAKE_DIRECTORY "${tmp}/objects")

    set(i 0)
    foreach(archive IN LISTS libs)
        math(EXPR i "${i} + 1")
        get_filename_component(aname "${archive}" NAME_WE)
        string(REGEX REPLACE "^lib" "" aname "${aname}")
        set(srcdir "${tmp}/src${i}")
        file(MAKE_DIRECTORY "${srcdir}")
        execute_process(
            COMMAND "${MERGE_TOOL}" x "${archive}"
            WORKING_DIRECTORY "${srcdir}"
            RESULT_VARIABLE rc
        )
        if(NOT rc EQUAL 0)
            message(FATAL_ERROR "ar x failed on ${archive} (rc=${rc})")
        endif()
        file(GLOB members "${srcdir}/*")
        foreach(member IN LISTS members)
            get_filename_component(mbase "${member}" NAME)
            file(RENAME "${member}"
                 "${tmp}/objects/${aname}__${mbase}")
        endforeach()
        file(REMOVE_RECURSE "${srcdir}")
    endforeach()

    file(GLOB objects "${tmp}/objects/*")
    execute_process(
        COMMAND "${MERGE_TOOL}" rcs "${OUTPUT_FILE}" ${objects}
        RESULT_VARIABLE rc
    )
    file(REMOVE_RECURSE "${tmp}")
    if(NOT rc EQUAL 0)
        message(FATAL_ERROR "ar rcs failed (rc=${rc})")
    endif()
    # refresh the archive index (ranlib is a no-op if unavailable)
    execute_process(COMMAND ranlib "${OUTPUT_FILE}" RESULT_VARIABLE rc)
endif()

message(STATUS "Merged archive written to ${OUTPUT_FILE}")
