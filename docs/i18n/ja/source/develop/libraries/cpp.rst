C++ ライブラリ (iroha-lib)
--------------------------

``iroha-lib`` は Iroha 1 向けのネイティブ C++ クライアント
ライブラリです。トランザクションやクエリの作成・署名、
protobuf 形式へのシリアライズ、gRPC 経由での Iroha ノード
への送信を、ノード自体をビルド・デプロイせずに行えます。

ライブラリは 2 つの方法でビルドできます:

* Iroha のフルビルドの一部として (``iroha_lib`` ターゲット)、
  または
* **単独で** — クライアントライブラリとその依存関係のみが
  コンパイルされ、ノードのコードは不要です (block storage、
  Torii サービス、同期はいずれも不要)。

単独ビルド
^^^^^^^^^^

単独ビルドは ``iroha-lib/`` ディレクトリにあり、独立した
CMake プロジェクトです。リポジトリの ``shared_model``、
``libs``、モデルコンバータを再利用するため、引き続き
完全なソースツリーが必要ですが、コンパイルされるのは
ライブラリのターゲットのみです。

ライブラリの依存関係は少なく、``protobuf``、``grpc``、
``boost``、``spdlog``、``rapidjson``、``gflags``、
``iroha-ed25519`` です (``vcpkg/VCPKG_DEPS_LIST`` 参照)。
vcpkg を使う場合、これらはノードと同じスクリプトで一度
だけインストールされます — 詳細は :ref:`build-guide` を
参照してください:

.. code:: sh

    # from the repository root; builds vcpkg into ./vcpkg-build
    ./vcpkg/build_iroha_deps.sh $PWD/vcpkg-build

次に、設定してビルドします:

.. code:: sh

    cmake -S iroha-lib -B build-iroha-lib \
        -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg-build/scripts/buildsystems/vcpkg.cmake \
        -DVCPKG_TARGET_TRIPLET=x64-linux \
        -DCMAKE_BUILD_TYPE=Release

    cmake --build build-iroha-lib --target iroha_lib -j$(nproc)

付属のサンプル (``TxExample``、``BatchExample``、
``QueryExample``、``TxOrderingExample``、
``BlocksQueryExample``、``MstExample``、
``InfiniteBlocksStreamExample``、``DomainAssetCreation``、
``TlsExample``) もビルドするには:

.. code:: sh

    cmake --build build-iroha-lib -j$(nproc)

ノードプロジェクト内でのビルド
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

プロジェクト全体が設定されると、ライブラリは ``iroha_lib``
ターゲット (``grpc_client`` とともに) として利用できます:

.. code:: sh

    cmake --build build --target iroha_lib -j$(nproc)

ライブラリの使用
^^^^^^^^^^^^^^^^

主なエントリポイント:

* ``iroha_lib::Tx`` / ``iroha_lib::TxBatch`` — トランザクションの
  作成、署名、送信 (``model/Tx.hpp``、``model/TxBatch.hpp``
  参照)。
* ``iroha_lib::Query`` — クエリの構築と送信
  (``model/Query.hpp`` 参照)。
* ``iroha_lib::BlocksQuery`` — ``FetchCommits`` ストリーム用の
  署名済みブロッククエリの構築
  (``model/BlocksQuery.hpp`` 参照)。
* ``iroha_lib::GrpcClient`` — Torii ``CommandService`` と
  ``QueryService`` エンドポイント向けの低レベル gRPC
  クライアント。``sendStatusStream`` /
  ``sendBlocksStreamQuery`` のサーバーストリーミングと、
  ``iroha_lib::TlsOptions`` によるオプションの TLS
  トランスポートを含む (``grpc_client/GrpcClient.hpp``
  参照)。
* ``iroha_lib::createKeyFiles`` / ``iroha_lib::loadKeyFiles``
  (``model/utils/key_files.hpp``) — ``KeysManagerImpl`` 形式の
  キーファイルの生成と読み込み (``<name>.pub`` /
  ``<name>.priv``、オプションでパスワード暗号化)。
* ``iroha_lib::generateGenesisBlock`` / ``readPeersAddresses``
  (``model/utils/genesis.hpp``) — サンプル genesis ブロックの
  生成。``iroha-cli --genesis_block`` と同等。

キーペアは hex 形式の秘密鍵から
``Utils::generateKeypairFromPrivateKey``
(``model/utils/Utils.h``) で、またはランダムに
``Utils::generateRandomKeypair`` で作成できます。署名には
``shared_model`` の ed25519/SHA3 実装を使用します。

すべてのサンプルは接続設定を環境変数から読み込みます。
デフォルト値は ``iroha-python`` の開発設定と一致します
(``IROHA_HOST_ADDR``、``IROHA_PORT``、``ADMIN_ACCOUNT_ID``、
``ADMIN_PRIVATE_KEY`` — ``examples/common.hpp`` 参照)。
``Scenario`` または ``makeClient`` を経由するサンプルは
さらに ``IROHA_TLS_ROOT_CERT`` を考慮します — 設定されて
いる場合、クライアントは TLS 経由で接続します。

クイックスタート — ドメインとアセットを作成するトランザクション
の構築、署名、送信 (``examples/scenario.hpp`` より):

.. literalinclude:: ../../../../iroha-lib/examples/scenario.hpp
   :language: c++
   :lines: 45-56

クエリの送信とレスポンスの読み取り (``examples/scenario.hpp``
より):

.. literalinclude:: ../../../../iroha-lib/examples/scenario.hpp
   :language: c++
   :lines: 179-201

完全な動作例は ``examples/TxExample.cpp`` です — ドメインと
アセットを作成し、アカウントを登録し、コインを送金し、
ライブノードでクエリを実行します:

.. code:: sh

    IROHA_HOST_ADDR=127.0.0.1 IROHA_PORT=50051 \
        ./build-iroha-lib/examples/irohalib_tx_example

TLS (暗号化トランスポート)
^^^^^^^^^^^^^^^^^^^^^^^^^^

デフォルトでは ``GrpcClient`` は暗号化されていない gRPC
チャネルを使用します。接続を暗号化するには、コンストラクタに
``iroha_lib::TlsOptions`` を渡します:

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

Iroha Torii TLS エンドポイントは**一方向 TLS** (サーバー認証
のみ) を使用するため、実際には ``pem_root_certs`` のみが
必要です。同じ認証情報がクライアントのすべての操作 —
トランザクション、クエリ、両方のストリーミング API — に
適用されます。

ノード側で TLS を有効にするには、ノード設定で
``torii_tls_params`` を設定します (``port`` と
``key_path`` — サーバーは ``<key_path>.crt`` と
``<key_path>.key`` を読み込みます)。

付属の ``TlsExample`` は TLS 接続を実演します: 暗号化
チャネル経由で署名済み ``GetAccount`` クエリと
``GetTxStatus`` 呼び出しを実行します。ノード検証に使用する
証明書を指す ``IROHA_TLS_ROOT_CERT`` が必要です:

.. code:: sh

    IROHA_HOST_ADDR=127.0.0.1 IROHA_PORT=<torii_tls_port> \
    IROHA_TLS_ROOT_CERT=/path/to/node.crt \
        ./build-iroha-lib/examples/irohalib_tls_example

このパスをカバーする統合テストがあります:
``test/integration/iroha_lib_tls`` は TLS エンドポイントを
有効にしたノードを起動し、その上でサンプルを実行します。
