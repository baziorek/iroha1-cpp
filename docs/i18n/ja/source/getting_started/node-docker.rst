.. _node-docker:

==============================
Docker で Iroha ノードを実行
==============================

このガイドでは、ローカルマシンの Docker で ``irohad`` — ノード
自体 — をデプロイする手順を説明します。これらの手順を完了すると、
``iroha-cli`` （:doc:`cli` ）またはクライアントライブラリのいずれ
か（:doc:`../develop/libraries` ）を通じてトランザクションを送信
できる実行中のノードが得られます。

.. note:: このガイドはコミュニティフォーク
  `baziorek/iroha1-cpp <https://github.com/baziorek/iroha1-cpp>`_
  に関するものです。

開始前に
--------

以下がインストールされ、実行されていることを確認してください:

- `Docker <https://docs.docker.com/get-docker/>`_ （macOS/Windows
  では Docker Desktop、Linux では Docker Engine）
- `docker compose <https://docs.docker.com/compose/>`_ （通常
  Docker Desktop に含まれています。Linux では個別インストールが
  必要な場合があります）
- リポジトリをクローンして設定例を取得するための
  `git <https://git-scm.com/downloads>`_
- （オプション）便利なコマンドエイリアスを設定する場合は
  `Python <https://www.python.org/downloads/>`_

1. リポジトリをクローン
-----------------------

コミュニティフォークのリポジトリをクローンし、``example/``
ディレクトリに移動します:

.. code-block:: shell

  git clone -b iroha1-main https://github.com/baziorek/iroha1-cpp
  cd iroha1-cpp/example

ここには初回ノード起動に必要なすべてのファイルがあります:
``config.docker`` （Docker イメージ用の設定）、``genesis.block`` 、
``node0`` 鍵ペアなど。

2. PostgreSQL と Iroha ノードを起動
-----------------------------------

``example/`` には、PostgreSQL（Iroha のストレージ）、Iroha ノード
1 台、Torii 用の Nginx ポートフォワーダーの 3 つのサービスを起動
する準備済みの ``docker-compose.yml`` があります:

.. code-block:: shell

  docker compose up

初回は Docker が ``irohad`` イメージをダウンロードしてビルドする
ため、数分かかります。コンテナが ``healthy`` と報告するまで待ち、
ノードが応答することを確認します:

.. code-block:: shell

  docker compose ps

期待される出力は、3 つのサービスすべてが ``running`` または
``healthy`` ステータスであることです。

3. 最初のトランザクションを送信
--------------------------------

実行中のコンテナの 1 つには既に ``iroha-cli`` があります。対話
セッションを開きます:

.. code-block:: shell

  docker exec -it iroha iroha-cli -account_name admin@test

デフォルトで CLI は利用可能なコマンドを提示します。すべての
オプションを見るには、コンテナ内または実行中のイメージ内で
``iroha-cli -h`` を実行してください。

``iroha-cli`` を使ったトランザクションとクエリの送信の詳細な
ガイドは :doc:`cli-guide` を参照してください。

4. 次のステップ
---------------

- :doc:`configuration` — 設定パラメータの詳細な内訳
- :doc:`../deploy/single` — 実際の（非 Docker）サーバーでの実行
- :doc:`../develop/libraries/cpp` — ``iroha-lib`` を使った独自の
  C++ アプリケーションの作成

ノードの停止
------------

終了したら、コンテナを停止します:

.. code-block:: shell

  docker compose down

PostgreSQL のデータも削除するには:

.. code-block:: shell

  docker compose down -v
