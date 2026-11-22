C++ Library (iroha-lib)
-----------------------

``iroha-lib`` is the native C++ client library for Iroha 1. It allows you to
build and sign transactions and queries, serialize them to the protobuf
format, and send them to an Iroha node over gRPC — without building or
deploying the node itself.

The library can be built in two ways:

* as a part of the full Iroha build (target ``iroha_lib_model``), or
* **standalone** — only the client library and its dependencies are
  compiled, no node code (no block storage, no Torii service, no
  synchronization) is required.

Building Standalone
^^^^^^^^^^^^^^^^^^^

The standalone build lives in the ``iroha-lib/`` directory and is a
self-contained CMake project. It reuses ``shared_model``, ``libs`` and the
model converters from the repository, so you still need the full source
tree — but only the library targets are compiled.

.. code:: sh

    # from the repository root; adjust the toolchain file to your vcpkg
    cmake -S iroha-lib -B build-iroha-lib \
        -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake \
        -DVCPKG_TARGET_TRIPLET=x64-linux \
        -DCMAKE_BUILD_TYPE=Release

    cmake --build build-iroha-lib --target iroha_lib_model -j$(nproc)

Required dependencies: ``protobuf``, ``grpc``, ``boost``, ``spdlog``,
``rapidjson``, ``gflags`` and ``iroha-ed25519`` (see
``vcpkg/VCPKG_DEPS_LIST``). When using vcpkg they are installed
automatically by the toolchain file.

To also build the bundled examples (``TxExample``, ``BatchExample``,
``QueryExample``, ``DomainAssetCreation``):

.. code:: sh

    cmake --build build-iroha-lib -j$(nproc)

Building Inside the Node Project
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When the whole project is configured, the library is available as the
``iroha_lib_model`` target (together with ``grpc_client``):

.. code:: sh

    cmake --build build --target iroha_lib_model -j$(nproc)

Using the Library
^^^^^^^^^^^^^^^^^

The main entry points are:

* ``iroha_lib::Tx`` / ``iroha_lib::TxBatch`` — create, sign and send
  transactions (see ``model/generators/CommandGenerator.hpp``).
* ``iroha_lib::Query`` — build and send queries
  (see ``model/generators/QueryGenerator.hpp``).
* ``iroha_lib::GrpcClient`` — low-level gRPC client for the Torii
  ``CommandService`` and ``QueryService`` endpoints
  (see ``grpc_client/GrpcClient.hpp``).

Key files are loaded with ``libs/crypto`` ``KeysManager`` and signatures
use the ed25519/SHA3 implementation from ``shared_model``.

Working examples are in ``iroha-lib/examples/`` — e.g. ``TxExample.cpp``
demonstrates creating a transaction, signing it with a key file and
sending it to a node (peer ``127.0.0.1:50051`` is hardcoded):

.. code:: sh

    ./build-iroha-lib/examples/irohalib_tx_example /path/to/key.json
