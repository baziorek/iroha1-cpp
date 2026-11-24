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
``QueryExample``, ``TxOrderingExample``, ``BlocksQueryExample``,
``MstExample``, ``InfiniteBlocksStreamExample``, ``DomainAssetCreation``,
``TlsExample``):

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
  transactions (see ``model/Tx.hpp``, ``model/TxBatch.hpp``).
* ``iroha_lib::Query`` — build and send queries
  (see ``model/Query.hpp``).
* ``iroha_lib::BlocksQuery`` — build signed blocks queries for the
  ``FetchCommits`` stream (see ``model/BlocksQuery.hpp``).
* ``iroha_lib::GrpcClient`` — low-level gRPC client for the Torii
  ``CommandService`` and ``QueryService`` endpoints, including
  server-streaming ``sendStatusStream`` / ``sendBlocksStreamQuery``
  and optional TLS transport via ``iroha_lib::TlsOptions``
  (see ``grpc_client/GrpcClient.hpp``).
* ``iroha_lib::createKeyFiles`` / ``iroha_lib::loadKeyFiles``
  (``model/utils/key_files.hpp``) — generate and load key files in the
  ``KeysManagerImpl`` format (``<name>.pub`` / ``<name>.priv``, optional
  passphrase encryption).
* ``iroha_lib::generateGenesisBlock`` / ``readPeersAddresses``
  (``model/utils/genesis.hpp``) — sample genesis block generation,
  equivalent of ``iroha-cli --genesis_block``.

Key pairs can be created from a hex private key with
``Utils::generateKeypairFromPrivateKey`` (``model/utils/Utils.h``) or
randomly with ``Utils::generateRandomKeypair``; signatures use the
ed25519/SHA3 implementation from ``shared_model``.

All examples read connection settings from environment variables, with
defaults matching the ``iroha-python`` development setup
(``IROHA_HOST_ADDR``, ``IROHA_PORT``, ``ADMIN_ACCOUNT_ID``,
``ADMIN_PRIVATE_KEY`` — see ``examples/common.hpp``). Examples that go
through ``Scenario`` or ``makeClient`` additionally honour
``IROHA_TLS_ROOT_CERT`` — when set, the client connects over TLS.

Quick start — building, signing and sending a transaction that creates
a domain and an asset (from ``examples/scenario.hpp``):

.. literalinclude:: ../../../../iroha-lib/examples/scenario.hpp
   :language: c++
   :lines: 45-56

Sending a query and reading the response (from
``examples/scenario.hpp``):

.. literalinclude:: ../../../../iroha-lib/examples/scenario.hpp
   :language: c++
   :lines: 179-201

A complete runnable example is ``examples/TxExample.cpp`` — it creates a
domain and asset, registers an account, transfers coins and runs
queries against a live node:

.. code:: sh

    IROHA_HOST_ADDR=127.0.0.1 IROHA_PORT=50051 \
        ./build-iroha-lib/examples/irohalib_tx_example

TLS (Encrypted Transport)
^^^^^^^^^^^^^^^^^^^^^^^^^

By default ``GrpcClient`` uses an insecure gRPC channel. To encrypt the
connection, pass ``iroha_lib::TlsOptions`` to the constructor:

.. code:: c++

    #include <fstream>
    #include <sstream>

    std::ifstream cert_file("/path/to/node.crt");
    std::ostringstream pem;
    pem << cert_file.rdbuf();

    iroha_lib::TlsOptions tls;
    tls.pem_root_certs = pem.str();   // PEM root certificate(s)
    // tls.pem_cert_chain / tls.pem_private_key - optional, for mutual TLS

    iroha_lib::GrpcClient client("127.0.0.1", 50051, tls);

Iroha's Torii TLS endpoint uses **one-way TLS** (server authentication
only), so in practice only ``pem_root_certs`` is required. The same
credentials apply to all client operations — transactions, queries and
both streaming APIs.

To enable TLS on the node side, configure ``torii_tls_params`` in the
node configuration (``port`` and ``key_path`` — the server reads
``<key_path>.crt`` and ``<key_path>.key``).

The bundled ``TlsExample`` demonstrates a TLS connection: it performs a
signed ``GetAccount`` query and a ``GetTxStatus`` call over the
encrypted channel. It requires ``IROHA_TLS_ROOT_CERT`` pointing to a PEM
file with the certificate used to verify the node:

.. code:: sh

    IROHA_HOST_ADDR=127.0.0.1 IROHA_PORT=<torii_tls_port> \
    IROHA_TLS_ROOT_CERT=/path/to/node.crt \
        ./build-iroha-lib/examples/irohalib_tls_example

There is an integration test covering this path:
``test/integration/iroha_lib_tls`` boots a node with the TLS endpoint
enabled and runs the example against it.
