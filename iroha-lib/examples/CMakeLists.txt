if (APPLE)
    message("iroha-lib does not support MacOS currently!")
else()
    ###### iroha-lib examples:
    # Shared header-only helpers (common.hpp / scenario.hpp /
    # error_codes.hpp) are exposed through an interface target so the
    # dependency is explicit instead of relying on quoted-include lookup.
    add_library(iroha_lib_examples INTERFACE)
    target_include_directories(iroha_lib_examples INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    foreach(example IN ITEMS
            TxExample
            BatchExample
            MstExample
            QueryExample
            TxOrderingExample
            BlocksQueryExample
            InfiniteBlocksStreamExample
            DomainAssetCreation
            TlsExample
            PermissionsExample
    )
        add_executable(irohalib_${example}_example
            ${example}.cpp
        )

        target_link_libraries(irohalib_${example}_example
            iroha_lib_examples
            iroha_lib_model
            logger_manager
        )

        ## Examples are build-only: they are not installed into packages.
    endforeach()

    # DomainAssetCreation additionally uses gflags
    target_link_libraries(irohalib_DomainAssetCreation_example
        gflags
    )
endif()
