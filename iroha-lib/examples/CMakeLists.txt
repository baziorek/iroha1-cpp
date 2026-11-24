if (APPLE)
    message("iroha-lib does not support MacOS currently!")
else()
    ###### iroha-lib examples:
    # Each executable is a single .cpp that uses the shared helpers in
    # common.hpp / scenario.hpp (header-only).
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
    )
        add_executable(irohalib_${example}_example
            ${example}.cpp
        )

        target_link_libraries(irohalib_${example}_example
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
