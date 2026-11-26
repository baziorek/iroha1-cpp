=======================
単一インスタンスの実行
=======================

通常、ユーザーは API を試したり機能を探求するために Iroha をローカル
で実行したいと考えます。これはローカル環境またはコンテナ環境
（Docker）で実行できます。両方のケースを検討しますが、ピアコンポー
ネントのデプロイを簡素化するために、*マシンに Docker がインストー
ルされていることをお勧めします*。

ローカル環境
------------

ローカル環境とは、コンテナなしでデーモンプロセスと Postgres がデプ
ロイされた状態を意味します。Docker をいじるのが好ましくない場合、
つまり機能の簡単な探索には役立つかもしれません。

postgres サーバーの実行
"""""""""""""""""""""""

postgres サーバーをローカルで実行するには、postgres の `website
<https://www.postgresql.org/docs/current/static/server-start.html>`__
を確認し、そこに書かれた説明に従ってください。通常、postgres サー
バーはシステム起動時に自動的に起動しますが、システムの設定で確認す
るべきです。

Postgres データベースサーバーは、通常のシステム統合なしに手動で初期
化して起動することもできます:

.. code-block::shell

   initdb ~/iroha/nodeX_db/
   ## Start server in background, logs will appear in current console
   postgres -D ~/iroha/nodaX_db/ -p5433 &
   createuser -s iroha_user -p5433

選択されたポート ``5433``（デフォルトは 5432）とデータベースユーザー
``iroha_user`` は、irohad がデータベースに接続するために使用されま
す。（詳細は `設定パラメータ
<../configure/index.html>`__ を参照してください。）メンテナンスデー
タベース ``postgres`` はデフォルトで作成されますが、何らかの理由で
別の名前が必要な場合は作成してください:

.. code-block::shell

   createdb iroha_mainteance -p5433


Iroha デーモン（irohad）の実行
"""""""""""""""""""""""""""""""""""""""

続行する前に満たすべき前提条件のリストがあります:

* Postgres サーバーが起動している
* Iroha デーモンバイナリ `irohad` がビルドされシステムで利用可能
* genesis block と設定ファイルが作成されている
* 設定ファイルが有効な postgres 接続設定を使用している
* ピア用の鍵ペアが生成されている
* このピアで Iroha を実行するのは初めてで、新しいチェーンを作成し
  たい

.. Hint:: 前提条件のリストと一致しないものがありますか? セクション
  :ref:`deploy_troubles` を参照してください。

前提条件が満たされている場合、残るのは次のパラメータでデーモンプロ
セスを起動することだけです:

+---------------+----------------------------------------------------------------+
| パラメータ    | 意味                                                           |
+===============+================================================================+
| config        | postgres 接続とシステム調整の値を含む設定ファイル              |
| genesis_block | 台帳の初期ブロック                                             |
| keypair_name  | 拡張子なしの秘密鍵・公開鍵ファイル名。ブロック署名にピアが使用 |
+---------------+----------------------------------------------------------------+

.. Attention:: 既に台帳にブロックがある状態で `--genesis_block` を使
  って新しい genesis block を指定するには、`--overwrite_ledger` フラ
  グを設定する必要があります。そうでない場合デーモンは失敗します。

Iroha デーモンを実行するシェルコマンドの例:

.. code-block:: shell

    irohad --config example/config.sample --genesis_block example/genesis.block --keypair_name example/node0

.. Note:: `HL Ursa サポート
  <../integrations/index.html#hyperledger-ursa>`__ 付きでビルドされ
  た Iroha を実行している場合は、`example/ursa-keys/` からサンプル鍵
  と genesis block を取得してください

.. Attention:: デーモンを停止して既存のチェーンを使用したい場合は、
  genesis block パラメータを渡さないでください。


Docker
------

Docker で Iroha ピアを単一インスタンスとして実行するには、まず
Iroha のイメージを pull する必要があります:

.. code-block:: shell

    docker pull hyperledger/iroha:1.6.0

.. note:: ``hyperledger/iroha:1.6.0`` は最後の公式 upstream Iroha 1
  イメージです — ``:latest`` （2022 年の古いビルド）や
  ``:develop``/``:main`` タグ（現在は Iroha 2 ビルドを指す）を使
  わないでください。

次に、イメージが問題なく動作するための環境を作成する必要がありま
す:

docker ネットワークの作成
"""""""""""""""""""""""""

Postgres と Iroha のコンテナは、互いにアクセスできるよう同じ仮想
ネットワークで動作する必要があります。次のコマンドを入力してネット
ワークを作成してください（ネットワーク名は任意ですが、例では
*iroha-network* を使用）:

.. code-block:: shell

    docker network create iroha-network

コンテナでの Postgresql の実行
""""""""""""""""""""""""""""""""""""""

同様に、postgres サーバーを以前に作成したネットワークに接続し、通
信用のポートを公開して実行します:

.. code-block:: shell

    docker run --name some-postgres \
    -e POSTGRES_USER=postgres \
    -e POSTGRES_PASSWORD=mysecretpassword \
    -p 5432:5432 \
    --network=iroha-network \
    -d postgres:9.5

ブロックストレージ用ボリュームの作成
"""""""""""""""""""""""""""""""""""""""""""""

コンテナ内で iroha デーモンを実行する前に、チェーンのブロックを保存
するファイル用の永続ボリュームを作成する必要があります。これは次
のコマンドで行います:

.. code-block:: shell

    docker volume create blockstore

docker コンテナでの iroha デーモンの実行
"""""""""""""""""""""""""""""""""""""""""""""""""""

続行する前に確認すべき前提条件のリストがあります:

* Postgres サーバーが同じ docker ネットワークで動作している
* 単一ノード用の設定ファイルと鍵ペアを含むフォルダーがある
* このピアで Iroha を実行するのは初めてで、新しいチェーンを作成し
  たい

これらが満たされていれば、次のコマンドに進めます:

.. code-block:: shell

    docker run --name iroha \
    # External port
    -p 50051:50051 \
    # Folder with configuration files
    -v ~/Developer/iroha/example:/opt/iroha_data \
    # Blockstore volume
    -v blockstore:/tmp/block_store \
    # Postgres settings
    -e POSTGRES_HOST='some-postgres' \
    -e POSTGRES_PORT='5432' \
    -e POSTGRES_PASSWORD='mysecretpassword' \
    -e POSTGRES_USER='postgres' \
    # Node keypair name
    -e KEY='node0' \
    # Docker network name
    --network=iroha-network \
    hyperledger/iroha:1.6.0

.. note:: ``hyperledger/iroha:1.6.0`` は最後の公式 upstream Iroha 1
  イメージです — ``:latest`` （2022 年の古いビルド）を使わないでく
  ださい。

.. note:: **初回起動時** 、Iroha は blockstore から WSV を再構築しま
  すが、これはメモリを大量に消費します — 大きな台帳ではコンテナが
  OOM で kill される可能性があります（
  `issue #1833
  <https://github.com/hyperledger-iroha/iroha/issues/1833>`__ 参照）。
  docker コンテナにはデフォルトでメモリ制限がありませんが、
  **Docker Desktop** （macOS/Windows）では VM 全体にメモリ上限があ
  ります — *Settings → Resources* で引き上げるか、Linux ホストで
  ``docker run`` に ``--memory`` を渡してください。
