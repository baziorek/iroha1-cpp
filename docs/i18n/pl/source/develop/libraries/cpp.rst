Biblioteka C++ (iroha-lib)
--------------------------

``iroha-lib`` to natywna biblioteka kliencka C++ dla Iroha 1.
Pozwala budować i podpisywać transakcje i zapytania, serializować
je do formatu protobuf i wysyłać do węzła Iroha przez gRPC —
bez budowania lub wdrażania samego węzła.

Bibliotekę można budować na dwa sposoby:

* jako część pełnego builda Irohy (target ``iroha_lib``), lub
* **samodzielnie** — kompilowane są tylko biblioteka kliencka i jej
  zależności, nie jest wymagany kod węzła (żaden block storage,
  żadna usługa Torii, żadna synchronizacja).

Budowanie samodzielne
^^^^^^^^^^^^^^^^^^^^^

Samodzielny build znajduje się w katalogu ``iroha-lib/`` i jest
samodzielnym projektem CMake. Ponownie używa ``shared_model``,
``libs`` i konwerterów modelu z repozytorium, więc nadal
potrzebujesz pełnego drzewa źródeł — ale kompilowane są tylko
targety biblioteki.

Biblioteka ma mały zestaw zależności — ``protobuf``, ``grpc``,
``boost``, ``spdlog``, ``rapidjson``, ``gflags`` i
``iroha-ed25519`` (zobacz ``vcpkg/VCPKG_DEPS_LIST``). Przy użyciu
vcpkg są one instalowane raz z góry tym samym skryptem co dla
węzła — zobacz :ref:`build-guide`, aby uzyskać pełne instrukcje:

.. code:: sh

    # from the repository root; builds vcpkg into ./vcpkg-build
    ./vcpkg/build_iroha_deps.sh $PWD/vcpkg-build

Następnie skonfiguruj i zbuduj:

.. code:: sh

    cmake -S iroha-lib -B build-iroha-lib \
        -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg-build/scripts/buildsystems/vcpkg.cmake \
        -DVCPKG_TARGET_TRIPLET=x64-linux \
        -DCMAKE_BUILD_TYPE=Release

    cmake --build build-iroha-lib --target iroha_lib -j$(nproc)

Aby zbudować również dołączone przykłady (``TxExample``,
``BatchExample``, ``QueryExample``, ``TxOrderingExample``,
``BlocksQueryExample``, ``MstExample``,
``InfiniteBlocksStreamExample``, ``DomainAssetCreation``,
``TlsExample``):

.. code:: sh

    cmake --build build-iroha-lib -j$(nproc)

Budowanie wewnątrz projektu węzła
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Gdy cały projekt jest skonfigurowany, biblioteka jest dostępna
jako target ``iroha_lib`` (wraz z ``grpc_client``):

.. code:: sh

    cmake --build build --target iroha_lib -j$(nproc)

Używanie biblioteki
^^^^^^^^^^^^^^^^^^^

Główne punkty wejścia to:

* ``iroha_lib::Tx`` / ``iroha_lib::TxBatch`` — tworzenie,
  podpisywanie i wysyłanie transakcji (zobacz
  ``model/Tx.hpp``, ``model/TxBatch.hpp``).
* ``iroha_lib::Query`` — budowanie i wysyłanie zapytań (zobacz
  ``model/Query.hpp``).
* ``iroha_lib::BlocksQuery`` — budowanie podpisanych zapytań o
  bloki dla strumienia ``FetchCommits`` (zobacz
  ``model/BlocksQuery.hpp``).
* ``iroha_lib::GrpcClient`` — niskopoziomowy klient gRPC dla
  endpointów Torii ``CommandService`` i ``QueryService``,
  w tym server-streaming ``sendStatusStream`` /
  ``sendBlocksStreamQuery`` i opcjonalny transport TLS przez
  ``iroha_lib::TlsOptions`` (zobacz
  ``grpc_client/GrpcClient.hpp``).
* ``iroha_lib::createKeyFiles`` / ``iroha_lib::loadKeyFiles``
  (``model/utils/key_files.hpp``) — generowanie i ładowanie plików
  kluczy w formacie ``KeysManagerImpl`` (``<name>.pub`` /
  ``<name>.priv``, opcjonalne szyfrowanie hasłem).
* ``iroha_lib::generateGenesisBlock`` / ``readPeersAddresses``
  (``model/utils/genesis.hpp``) — generowanie przykładowego bloku
  genesis, odpowiednik ``iroha-cli --genesis_block``.

Pary kluczy można tworzyć z klucza prywatnego w formacie hex przez
``Utils::generateKeypairFromPrivateKey`` (``model/utils/Utils.h``)
lub losowo przez ``Utils::generateRandomKeypair``; podpisy używają
implementacji ed25519/SHA3 z ``shared_model``.

Wszystkie przykłady odczytują ustawienia połączenia ze zmiennych
środowiskowych, z domyślnymi wartościami pasującymi do
konfiguracji deweloperskiej ``iroha-python``
(``IROHA_HOST_ADDR``, ``IROHA_PORT``, ``ADMIN_ACCOUNT_ID``,
``ADMIN_PRIVATE_KEY`` — zobacz ``examples/common.hpp``).
Przykłady przechodzące przez ``Scenario`` lub ``makeClient``
dodatkowo honorują ``IROHA_TLS_ROOT_CERT`` — gdy jest ustawiona,
klient łączy się przez TLS.

Szybki start — budowanie, podpisywanie i wysyłanie transakcji
tworzącej domenę i asset (z ``examples/scenario.hpp``):

.. literalinclude:: ../../../../iroha-lib/examples/scenario.hpp
   :language: c++
   :lines: 45-56

Wysyłanie zapytania i odczytywanie odpowiedzi (z
``examples/scenario.hpp``):

.. literalinclude:: ../../../../iroha-lib/examples/scenario.hpp
   :language: c++
   :lines: 179-201

Kompletny działający przykład to ``examples/TxExample.cpp`` —
tworzy domenę i asset, rejestruje konto, transferuje monety i
wykonuje zapytania na żywym węźle:

.. code:: sh

    IROHA_HOST_ADDR=127.0.0.1 IROHA_PORT=50051 \
        ./build-iroha-lib/examples/irohalib_tx_example

TLS (szyfrowany transport)
^^^^^^^^^^^^^^^^^^^^^^^^^^

Domyślnie ``GrpcClient`` używa niezabezpieczonego kanału gRPC.
Aby zaszyfrować połączenie, przekaż ``iroha_lib::TlsOptions`` do
konstruktora:

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

Endpoint TLS Torii Irohy używa **TLS jednokierunkowego**
(tylko uwierzytelnianie serwera), więc w praktyce wymagany jest
tylko ``pem_root_certs``. Te same poświadczenia dotyczą wszystkich
operacji klienta — transakcji, zapytań i obu API strumieniowych.

Aby włączyć TLS po stronie węzła, skonfiguruj
``torii_tls_params`` w konfiguracji węzła (``port`` i
``key_path`` — serwer odczytuje ``<key_path>.crt`` i
``<key_path>.key``).

Dołączony ``TlsExample`` demonstruje połączenie TLS: wykonuje
podpisane zapytanie ``GetAccount`` i wywołanie ``GetTxStatus``
przez zaszyfrowany kanał. Wymaga ``IROHA_TLS_ROOT_CERT``
wskazującego na plik PEM z certyfikatem używanym do weryfikacji
węzła:

.. code:: sh

    IROHA_HOST_ADDR=127.0.0.1 IROHA_PORT=<torii_tls_port> \
    IROHA_TLS_ROOT_CERT=/path/to/node.crt \
        ./build-iroha-lib/examples/irohalib_tls_example

Istnieje test integracyjny pokrywający tę ścieżkę:
``test/integration/iroha_lib_tls`` uruchamia węzeł z włączonym
endpointem TLS i wykonuje na nim przykład.
