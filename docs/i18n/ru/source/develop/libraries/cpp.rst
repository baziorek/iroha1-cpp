Библиотека C++ (iroha-lib)
--------------------------

``iroha-lib`` — нативная клиентская библиотека C++ для Iroha 1.
Она позволяет создавать и подписывать транзакции и запросы,
сериализовать их в формат protobuf и отправлять на узел Iroha
через gRPC — без сборки или развёртывания самого узла.

Библиотеку можно собирать двумя способами:

* как часть полной сборки Iroha (target ``iroha_lib``), или
* **автономно** — компилируется только клиентская библиотека и её
  зависимости, код узла не требуется (ни block storage, ни сервис
  Torii, ни синхронизация).

Автономная сборка
^^^^^^^^^^^^^^^^^

Автономная сборка находится в каталоге ``iroha-lib/`` и является
автономным проектом CMake. Она повторно использует
``shared_model``, ``libs`` и конвертеры модели из репозитория,
поэтому всё ещё нужно полное дерево исходников — но
компилируются только цели библиотеки.

У библиотеки небольшой набор зависимостей — ``protobuf``,
``grpc``, ``boost``, ``spdlog``, ``rapidjson``, ``gflags`` и
``iroha-ed25519`` (см. ``vcpkg/VCPKG_DEPS_LIST``). При
использовании vcpkg они устанавливаются один раз тем же скриптом,
что и для узла — см. :ref:`build-guide` для полных
инструкций:

.. code:: sh

    # from the repository root; builds vcpkg into ./vcpkg-build
    ./vcpkg/build_iroha_deps.sh $PWD/vcpkg-build

Затем настройте и соберите:

.. code:: sh

    cmake -S iroha-lib -B build-iroha-lib \
        -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg-build/scripts/buildsystems/vcpkg.cmake \
        -DVCPKG_TARGET_TRIPLET=x64-linux \
        -DCMAKE_BUILD_TYPE=Release

    cmake --build build-iroha-lib --target iroha_lib -j$(nproc)

Чтобы также собрать прилагаемые примеры (``TxExample``,
``BatchExample``, ``QueryExample``, ``TxOrderingExample``,
``BlocksQueryExample``, ``MstExample``,
``InfiniteBlocksStreamExample``, ``DomainAssetCreation``,
``TlsExample``):

.. code:: sh

    cmake --build build-iroha-lib -j$(nproc)

Сборка внутри проекта узла
^^^^^^^^^^^^^^^^^^^^^^^^^^

Когда весь проект настроен, библиотека доступна как target
``iroha_lib`` (вместе с ``grpc_client``):

.. code:: sh

    cmake --build build --target iroha_lib -j$(nproc)

Использование библиотеки
^^^^^^^^^^^^^^^^^^^^^^^^

Основные точки входа:

* ``iroha_lib::Tx`` / ``iroha_lib::TxBatch`` — создание,
  подписание и отправка транзакций (см.
  ``model/Tx.hpp``, ``model/TxBatch.hpp``).
* ``iroha_lib::Query`` — построение и отправка запросов (см.
  ``model/Query.hpp``).
* ``iroha_lib::BlocksQuery`` — построение подписанных запросов
  блоков для потока ``FetchCommits`` (см.
  ``model/BlocksQuery.hpp``).
* ``iroha_lib::GrpcClient`` — низкоуровневый gRPC-клиент для
  эндпоинтов Torii ``CommandService`` и ``QueryService``, включая
  server-streaming ``sendStatusStream`` /
  ``sendBlocksStreamQuery`` и опциональный TLS-транспорт через
  ``iroha_lib::TlsOptions`` (см. ``grpc_client/GrpcClient.hpp``).
* ``iroha_lib::createKeyFiles`` / ``iroha_lib::loadKeyFiles``
  (``model/utils/key_files.hpp``) — генерация и загрузка файлов
  ключей в формате ``KeysManagerImpl`` (``<name>.pub`` /
  ``<name>.priv``, опциональное шифрование паролем).
* ``iroha_lib::generateGenesisBlock`` / ``readPeersAddresses``
  (``model/utils/genesis.hpp``) — генерация образца genesis
  block, эквивалент ``iroha-cli --genesis_block``.

Пары ключей можно создавать из приватного ключа в hex через
``Utils::generateKeypairFromPrivateKey``
(``model/utils/Utils.h``) или случайно через
``Utils::generateRandomKeypair``; подписи используют реализацию
ed25519/SHA3 из ``shared_model``.

Все примеры читают настройки подключения из переменных окружения,
с значениями по умолчанию, соответствующими dev-настройке
``iroha-python`` (``IROHA_HOST_ADDR``, ``IROHA_PORT``,
``ADMIN_ACCOUNT_ID``, ``ADMIN_PRIVATE_KEY`` — см.
``examples/common.hpp``). Примеры, проходящие через ``Scenario``
или ``makeClient``, дополнительно учитывают
``IROHA_TLS_ROOT_CERT`` — когда он установлен, клиент подключается
через TLS.

Быстрый старт — сборка, подписание и отправка транзакции,
создающей домен и актив (из ``examples/scenario.hpp``):

.. literalinclude:: ../../../../iroha-lib/examples/scenario.hpp
   :language: c++
   :lines: 45-56

Отправка запроса и чтение ответа (из ``examples/scenario.hpp``):

.. literalinclude:: ../../../../iroha-lib/examples/scenario.hpp
   :language: c++
   :lines: 179-201

Полный рабочий пример — ``examples/TxExample.cpp``: он создаёт
домен и актив, регистрирует аккаунт, переводит монеты и выполняет
запросы к живому узлу:

.. code:: sh

    IROHA_HOST_ADDR=127.0.0.1 IROHA_PORT=50051 \
        ./build-iroha-lib/examples/irohalib_tx_example

TLS (шифрованный транспорт)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

По умолчанию ``GrpcClient`` использует незащищённый канал gRPC.
Чтобы зашифровать соединение, передайте ``iroha_lib::TlsOptions``
конструктору:

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

TLS-эндпоинт Torii Iroha использует **односторонний TLS** (только
аутентификация сервера), поэтому на практике требуется только
``pem_root_certs``. Те же учётные данные применяются ко всем
операциям клиента — транзакциям, запросам и обоим
streaming API.

Чтобы включить TLS на стороне узла, настройте
``torii_tls_params`` в конфигурации узла (``port`` и
``key_path`` — сервер читает ``<key_path>.crt`` и
``<key_path>.key``).

Прилагаемый ``TlsExample`` демонстрирует TLS-соединение: он
выполняет подписанный запрос ``GetAccount`` и вызов
``GetTxStatus`` через зашифрованный канал. Он требует
``IROHA_TLS_ROOT_CERT``, указывающий на PEM-файл с сертификатом,
используемым для проверки узла:

.. code:: sh

    IROHA_HOST_ADDR=127.0.0.1 IROHA_PORT=<torii_tls_port> \
    IROHA_TLS_ROOT_CERT=/path/to/node.crt \
        ./build-iroha-lib/examples/irohalib_tls_example

Существует интеграционный тест, покрывающий этот путь:
``test/integration/iroha_lib_tls`` запускает узел с включённым
TLS-эндпоинтом и выполняет на нём пример.
