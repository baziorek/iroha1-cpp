.. _build-guide:

==============
Iroha のビルド
==============

このガイドでは、Iroha のビルドに必要なすべての依存関係のインストール方法と、実際のビルド方法を学びます。

3 つのステップがあります:

#. 環境の前提条件のインストール

#. Iroha の依存関係のインストール（Docker では自動的に実行されます）

#. Iroha のビルド

.. note:: Iroha を使い始めるためにビルドする必要はありません。代わりに、Hub から準備済みの Docker イメージをダウンロードできます。このプロセスはこのドキュメントの :ref:`getting-started` ページで詳しく説明されています。

前提条件
=============

Iroha を正常にビルドするには、環境を設定する必要があります。
それを行う方法はいくつかあり、ここですべて説明します。

現在、Unix 系システムをサポートしています（主に人気のある Linux ディストリビューションと MacOS を対象としています）。Windows をお使いの場合や、すべての依存関係のインストールに時間をかけたくない場合は、Docker 環境の使用を検討してください。また、Windows ユーザーは
`WSL <https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux>`_
の使用を検討することもできます。

技術的には、Iroha は Windows 上で実験的モードでネイティブにビルドできます。
このガイドではその方法もカバーしています。
Windows ネイティブビルドに関連するすべてのステージは、その大きな違いのためにメインフローから分離されています。

以下からお好みのプラットフォームを選択してクイックアクセスしてください:

    - :ref:`docker-pre`
    - :ref:`linux-pre`
    - :ref:`MacOS-pre`
    - :ref:`Windows-pre`


.. hint:: 困っていますか？ FAQ セクションを確認するか、何かで行き詰まった場合は直接お問い合わせください。こうなるとは思いませんが、環境に関する問題が発生する可能性はあります。

.. _docker-pre:

Docker
^^^^^^

最新の Docker イメージを用意するというアイデアは、Iroha をビルドせずに実行できるようにすることです。
しかし今では、使い方そのままのイメージだけでなく、Docker 内で特別な **Iroha Builder** を実行して、希望どおりの方法で Iroha をビルドするオプションもあります。

まず、``docker`` と ``docker-compose`` をインストールする必要があります。インストール方法は
`Docker のウェブサイト <https://www.docker.com/community-edition/>`_
で読むことができます。

.. note:: 最新の docker デーモンと docker-compose を使用してください。

Iroha イメージ
""""""""""""""

すべての Iroha Docker イメージは、`Docker Hub を検索 <https://hub.docker.com/search?q=hyperledger%2Firoha&type=image>`_ するか、`GitHub <https://github.com/orgs/hyperledger/packages?repo_name=iroha>`_ で見つけることができます。

現在、以下のイメージがあります:

- ``iroha`` -- Iroha 1.x の一般ビルド（Docker Hub 上のアップストリーム成果物）;
- ``iroha-burrow`` -- `Burrow 統合 <../integrations/index.html#hyperledger-burrow>`_ を含む Iroha 1.x のビルド（非推奨: Burrow は EOL）;
- ``iroha-ursa`` -- `Ursa 統合 <../integrations/index.html#hyperledger-ursa>`_ を含む Iroha 1.x のビルド（非推奨: Ursa は EOL）;
- ``iroha2`` -- Iroha 2 のビルド;
- ``iroha-builder`` -- Docker 内で Iroha ビルダーを実行できる特別なイメージ。

.. hint:: イメージの実行については `クイックスタートガイド <../getting_started/index.html>`_ でさらに読むことができます。

各イメージは、イメージがビルドされたブランチを示す対応するタグとともに使用できます。
利用可能なすべてのタグは Docker Hub で見つけられます。``iroha`` イメージの `すべてのタグはこちら <https://hub.docker.com/r/hyperledger/iroha/tags>`_ です。

例えば、``iroha:1.6.0`` は最後の Iroha 1 ビルドです。``latest`` 、``develop`` 、``main`` タグは **使用しないでください** — これらは現在 Iroha 2（Rust）ビルドまたは古い 2022 年の成果物を指しています。

GHCR のコミュニティフォークイメージ
""""""""""""""""""""""""""""""""""""""

``baziorek/iroha1-cpp`` フォークの CI は、イメージを追加で **GitHub Container Registry** の ``ghcr.io/baziorek/iroha1-cpp`` に公開しています。
現在は **builder** イメージのみが公開されています（ワークフローでランタイムの ``docker/release`` イメージジョブは無効になっています）。タグはブランチ名に一致します — 例えば ``iroha1-main`` — およびリリースタグです:

.. code-block:: shell

  docker pull ghcr.io/baziorek/iroha1-cpp:iroha1-main

以下の ``hyperledger/iroha-builder`` とまったく同じように使用できますが、現在のフォークソースからビルドされているため、そのツールチェーンはリポジトリが今日必要とするものと一致していることに注意してください。


Iroha ビルダー
""""""""""""""

.. note:: ``hyperledger/iroha-builder`` イメージはアップストリームの成果物であり、Iroha 1 がアップストリームで非推奨になって以来再ビルドされていません。そのツールチェーンは現在のソースに対して古すぎる可能性があります。ネイティブまたは独自のコンテナでのビルドを推奨します — イメージがたまたま十分に新しい場合は、以下のステップは引き続き機能します。

Iroha ビルダーを使用すると、他のどのプラットフォームでも利用可能な `任意のパラメーター <CMakeパラメーター_>`__ で Iroha をビルドできますが、それを Docker で便利かつ安全に行うことができます。

手順は次のとおりです:

1. まず、ビルダーを実行しましょう:

.. code-block:: shell

  docker run -it hyperledger/iroha-builder:latest

このステップで、イメージをコンテナ内で対話モードで起動して実行します。利用可能な任意のタグを使用でき、デフォルトは ``latest`` 、開発タグは ``develop`` です。一部の操作に ``sudo`` 権限が必要になる場合があることに注意してください。

2. コンテナ内にいるとき、Iroha リポジトリをクローンします:

.. code-block:: shell

  git clone https://github.com/baziorek/iroha1-cpp.git

3. Iroha がクローンされたら、Iroha フォルダーに移動します:

.. code-block:: shell

  cd iroha1-cpp

4. 次に、vcpkg 経由ですべての必要な依存関係をビルドするスクリプトを実行します:

.. code-block:: shell

  ./vcpkg/build_iroha_deps.sh $PWD/vcpkg-build



.. _linux-pre:

Linux
^^^^^

Iroha をビルドするには、以下のパッケージが必要です:

``build-essential`` ``git`` ``ca-certificates`` ``tar`` ``ninja-build`` ``curl`` ``unzip`` ``cmake``

Debian ベースの Linux ディストリビューションで環境の依存関係をインストールするには、このコードを使用します。

.. code-block:: shell

  apt-get update; \
  apt-get -y --no-install-recommends install \
  build-essential ninja-build \
  git ca-certificates tar curl unzip cmake \
  pkg-config zip

.. note:: WSV-diff テストツール ``test/tool/test_tool_iroha_wsv_diff.sh`` は、テストを実行するマシンに ``jq`` と ``postgresql-client`` （``psql`` 用）も追加で必要とします。

.. Important:: `Burrow 統合 <../integrations/burrow.html>`_ を使用したい場合は GO も必要です。`公式ウェブサイト <https://golang.org/doc/install>`_ の指示に従ってインストールし、次のコマンドを使用してください:

.. code-block:: shell

  go get github.com/golang/protobuf/protoc-gen-go

.. note::  Iroha を積極的に開発し、共有ライブラリをビルドする予定がある場合は、CMake の `最新リリース <https://cmake.org/download/>`_ のインストールを検討してください。

RaspberryPi 4
"""""""""""""""""""""""

Raspberry Pi 4 で Iroha をビルドするには、Linux と同じ手順に従ってください。

**注意**: Iroha には 8GiB の RAM が必要です。ビルドが ``SIGKILL`` で終了する場合は、ホストデバイスにスワップファイルまたはスワップパーティションを作成するか、クロスコンパイルを検討してください。

これで `Iroha の依存関係のインストール <Vcpkg依存関係マネージャーによる依存関係のインストール_>`_ の準備ができました。

.. _macos-pre:

MacOS
^^^^^

Iroha をゼロからビルドして積極的に開発したい場合は、Homebrew ですべての環境依存関係をインストールするために次のコードを使用してください:

.. code-block:: shell

  xcode-select --install
  brew install cmake go pkg-config coreutils ninja git gcc@9

.. hint:: Homebrew 自体をインストールするには次を実行してください:

  ``ruby -e "$(curl -fsSL https://raw.githubusercontent.com/homebrew/install/master/install)"``

.. Important:: `Burrow 統合 <../integrations/burrow.html>`_ を使用したい場合は GO も必要です。`公式ウェブサイト <https://golang.org/doc/install>`_ の指示に従ってインストールし、次のコマンドを使用してください:

.. code-block:: shell

  go get github.com/golang/protobuf/protoc-gen-go

これで `Iroha の依存関係のインストール <Vcpkg依存関係マネージャーによる依存関係のインストール_>`_ の準備ができました。

.. _windows-pre:

Windows
^^^^^^^

.. note:: 記載されているすべてのコマンドは、Iroha の 64 ビットバージョンのビルド用に設計されています。

Chocolatey パッケージマネージャー
""""""""""""""""""""""""""""""""""

まず、Chocolatey パッケージマネージャーがインストールされている必要があります。chocolatey のインストールについては `ガイド <https://chocolatey.org/install>`_ を参照してください。

ツールセットのビルド
""""""""""""""""""""""""

コマンドプロンプトの管理者モードで chocolatey 経由で CMake、Git、Microsoft コンパイラをインストールします:

.. code-block:: shell

  choco install cmake git visualstudio2019-workload-vctools ninja


PostgreSQL はビルド依存関係ではありませんが、後のテストのために今インストールしておくことをお勧めします:

  .. code-block:: shell

    choco install postgresql
    # 設定したパスワードを忘れないでください！

これで `Iroha の依存関係のインストール <Vcpkg依存関係マネージャーによる依存関係のインストール_>`_ の準備ができました。

Vcpkg依存関係マネージャーによる依存関係のインストール
======================================================

現在、すべてのプラットフォーム — Linux、Windows、MacOS — で依存関係マネージャーとして Vcpkg を使用しています。
必要なパッチが機能することを保証するため、固定バージョンの Vcpkg（``vcpkg/VCPKG_COMMIT_SHA`` に固定）を使用しています。

必要な依存関係の完全なリストは 1 か所 — ``vcpkg/VCPKG_DEPS_LIST`` — で維持されており、以下に再掲します。vcpkg は各ポートを固定された vcpkg ベースラインで利用可能な最新バージョンに解決します。これらは成功するビルドのための **最低サポートバージョン** です:

.. literalinclude:: ../../../vcpkg/VCPKG_DEPS_LIST
   :language: text

.. note:: vcpkg の外でビルドする場合（例えばシステムパッケージ）、固定された vcpkg ベースラインが提供するものより **古くない** バージョンであることを確認してください。特に、コードは **soci 4.x** （``soci/std-optional.h`` を使用）、**gRPC コード生成付き protobuf** 、**Boost >= 1.73** 、**OpenSSL 3.x** を必要とします。

その安定バージョンは Iroha リポジトリ内でのみ見つかるため、Iroha をクローンする必要があります。
プロセス全体はすべてのプラットフォームで非常に似ていますが、正確なコマンドはわずかに異なります。

Linux と MacOS
^^^^^^^^^^^^^^^

ターミナルで実行:

.. code-block:: shell

  git clone https://github.com/baziorek/iroha1-cpp.git
  cd iroha
  ./vcpkg/build_iroha_deps.sh $PWD/vcpkg-build

これで完了です！ `Iroha のビルド <ビルドプロセス_>`_ セクションに進むことができます。

Windows
^^^^^^^

PowerShell から実行:

.. code-block:: shell

  git clone https://github.com/baziorek/iroha1-cpp.git
  cd iroha
  powershell -ExecutionPolicy ByPass -File .\.packer\win\scripts\vcpkg.ps1 .\vcpkg .\iroha\vcpkg


素晴らしい仕事です！ `Iroha のビルド <ビルドプロセス_>`_ セクションに進むことができます。

.. note:: Iroha の 32 ビットバージョンをビルドする予定がある場合は、上記で言及したすべてのライブラリを ``x64`` の代わりに ``x86`` プレフィックス付きでインストールする必要があります。

ビルドプロセス
==============

Iroha のビルド
^^^^^^^^^^^^^^

1. 依存関係がビルドされたので、Iroha 自体のビルドを開始できます:

.. code-block:: shell

  cmake -B build -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg-build/scripts/buildsystems/vcpkg.cmake . -DCMAKE_BUILD_TYPE=RELEASE   -GNinja -DUSE_BURROW=OFF -DUSE_LIBURSA=OFF -DTESTING=OFF -DPACKAGE_DEB=OFF

``-DUSE_BURROW=ON`` のような cmake パラメーターは、まさに特別なビルドのために選択できるパラメーターです。これらのパラメーターの完全なリストと説明は `こちら <CMakeパラメーター_>`_ で見ることができます。

2. 実行

.. code-block:: shell

  cmake --build ./build --target irohad

.. warning:: 後でテストを使用したい場合は、``irohad`` ターゲットをビルドする代わりに、次を使用する必要があります:

.. code-block:: shell

  cmake --build ./build --target all


3. ヘルプを実行して結果を確認:

.. code-block:: shell

  ./build/bin/irohad --help

このステップですべてのパラメーターが表示されます。これで完了です！

.. note:: Windows でビルドする場合は、PowerShell から実行しないでください。代わりに x64 Native tools コマンドプロンプトを使用してください。

これで Iroha がビルドされました。ただし、希望する場合は、以下で説明する追加パラメーターでビルドすることもできます。

結果に満足している場合は、次のステップに進んで `Iroha インスタンスを実行 <../deploy/single.html>`_ できます。

CMakeパラメーター
^^^^^^^^^^^^^^^^^^

プラットフォーム依存のビルドファイルを生成するために CMake を使用しています。
最終ビルドを設定するための多数のフラグがあります。
記載されているパラメーター以外にも cmake の変数が役立つことに注意してください。
また、このページは非推奨になる可能性がある（または単に不完全である）ため、``cmake -L`` 、``cmake-gui`` 、``ccmake`` でカスタムフラグを参照することもできます。

.. hint::  cmake 設定ステージでパラメーターを指定できます（例: cmake -DTESTING=ON）。

主要パラメーター
""""""""""""""""

.. list-table::
   :header-rows: 1
   :widths: 20 20 10 50

   * - パラメーター
     - 可能な値
     - デフォルト
     - 説明
   * - TESTING
     - ON/OFF
     - ON
     - テストのビルドを有効または無効にする
   * - BENCHMARKING
     - ON/OFF
     - OFF
     - Google Benchmarks ライブラリのビルドを有効または無効にする
   * - COVERAGE
     - ON/OFF
     - OFF
     - コードカバレッジ生成のための lcov 設定を有効または無効にする
   * - USE_LIBURSA
     - ON/OFF
     - OFF
     - 標準のものの代わりに HL Ursa 暗号の使用を有効にする
   * - USE_BURROW
     - ON/OFF
     - OFF
     - HL Burrow EVM 統合を有効にする

.. warning:: ``USE_LIBURSA`` と ``USE_BURROW`` オプションは **非推奨** です。Hyperledger Ursa は 2023 年 4 月 27 日に、Hyperledger Burrow は 2022 年 5 月 12 日に End of Life に達しました。両方の統合は古いデプロイメントとの互換性のためだけに保持されています。

.. note:: ビルドに HL Ursa 暗号を使用したい場合は、他の依存関係に加えて `Rust <https://www.rust-lang.org/tools/install>`_ もインストールしてください。HL Ursa 統合の詳細は `こちら <../integrations/index.html#hyperledger-ursa>`_ を参照してください。

  HL Burrow 統合を使用したい場合は、まず `Go <https://golang.org/doc/install>`_ と `protoc-gen-go <https://developers.google.com/protocol-buffers/docs/reference/go-generated>`_ をインストールすることを忘れないでください。`HL Burrow 統合 <../integrations/index.html#hyperledger-burrow>`_ の詳細を参照してください。

パッケージング固有のパラメーター
""""""""""""""""""""""""""""""""

.. list-table::
   :header-rows: 1
   :widths: 20 20 10 50

   * - パラメーター
     - 可能な値
     - デフォルト
     - 説明
   * - PACKAGE_ZIP
     - ON/OFF
     - OFF
     - zip パッケージングを有効または無効にする
   * - PACKAGE_TGZ
     - ON/OFF
     - OFF
     - tar.gz パッケージングを有効または無効にする
   * - PACKAGE_RPM
     - ON/OFF
     - OFF
     - rpm パッケージングを有効または無効にする
   * - PACKAGE_DEB
     - ON/OFF
     - OFF
     - deb パッケージングを有効または無効にする

テストの実行（任意）
^^^^^^^^^^^^^^^^^^^^
まず、テストのために Iroha が正しく `ビルドされていることを確認 <ビルドプロセス_>`__ してください。

Iroha をビルドした後、デーモンの動作をチェックするためにテストを実行するのが良いでしょう。次のコードでテストを実行できます:

.. code-block:: shell

  cmake --build build --target test

または、``build`` フォルダーで次のコマンドを実行できます:

.. code-block:: shell

  cd build
  ctest . --output-on-failure

.. note:: PostgreSQL ストレージが実行されていないと一部のテストは失敗します。``scripts/run-iroha-dev.sh`` スクリプトを使用していない場合は、Docker コンテナを実行するか、次のパラメーターでローカル接続を作成してください:

  .. code-block:: shell

    docker run --name some-postgres \
    -e POSTGRES_USER=postgres \
    -e POSTGRES_PASSWORD=mysecretpassword \
    -p 5432:5432 \
    -d postgres:9.5 \
    -c 'max_prepared_transactions=100'
