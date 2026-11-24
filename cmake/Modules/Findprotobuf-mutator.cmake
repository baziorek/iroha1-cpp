add_library(protobuf-mutator UNKNOWN IMPORTED)

set(URL https://github.com/google/libprotobuf-mutator.git)
## v1.5 (dc4ced33) is the newest tag; it builds with C++17, which the
## protobuf headers from the current vcpkg pin require (older commits
## hardcode -std=gnu++11 and fail against them).
set(VERSION dc4ced337a9fb4047e2dc727268fbac55ca82f73)
set(protomutator_LIB ${CMAKE_STATIC_LIBRARY_PREFIX}protobuf-mutator${CMAKE_STATIC_LIBRARY_SUFFIX})
set(libfuzzer_LIB ${CMAKE_STATIC_LIBRARY_PREFIX}protobuf-mutator-libfuzzer${CMAKE_STATIC_LIBRARY_SUFFIX})

externalproject_add(google_protobuf-mutator
    GIT_REPOSITORY  ${URL}
    GIT_TAG         ${VERSION}
    CMAKE_ARGS
                    -G${CMAKE_GENERATOR}
                    -DLIB_PROTO_MUTATOR_TESTING=OFF
                    -DCMAKE_CXX_STANDARD=17
                    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    BUILD_BYPRODUCTS ${EP_PREFIX}/src/google_protobuf-mutator-build/src/${protomutator_LIB}
                     ${EP_PREFIX}/src/google_protobuf-mutator-build/src/libfuzzer/${libfuzzer_LIB}
    INSTALL_COMMAND ""
    TEST_COMMAND "" # remove test step
    UPDATE_COMMAND "" # remove update step
    )
externalproject_get_property(google_protobuf-mutator source_dir binary_dir)
set(protobuf_mutator_INCLUDE_DIR ${source_dir}/src)
set(protobuf_mutator_LIBRARY ${binary_dir}/src/${protomutator_LIB})
file(MAKE_DIRECTORY ${protobuf_mutator_INCLUDE_DIR})
include_directories(${source_dir})
link_directories(${binary_dir})

add_dependencies(protobuf-mutator google_protobuf-mutator)

set_target_properties(protobuf-mutator PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${protobuf_mutator_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES ${protobuf_mutator_LIBRARY}
    IMPORTED_LOCATION ${binary_dir}/src/libfuzzer/${libfuzzer_LIB}
    )
