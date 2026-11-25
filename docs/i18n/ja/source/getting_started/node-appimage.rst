.. _node-appimage:

==============================
AppImage で Iroha ノードを実行
==============================

AppImage は自己完結型の実行ファイルです — インストールや依存関係の
設定は不要です。Iroha 1 のリリースパイプラインは 2 つを生成します:

- ``irohad-x86_64.AppImage`` — ノード本体
- ``iroha-cli-x86_64.AppImage`` — コマンドラインクライアント
  （:doc:`cli` 参照）

.. note:: AppImage はコミュニティフォークの
  `GitHub Releases
  <https://github.com/baziorek/iroha1-cpp/releases>`__ にタグ
  ``1.6.1`` から添付されます。それらを含むリリースが公開される
  までは、:ref:`Docker <node-docker>` または
  :ref:` ソースビルド <node-build>` を使用してください。

ダウンロード
------------

お使いのプラットフォーム用の AppImage を `Releases
<https://github.com/baziorek/iroha1-cpp/releases>`__ ページから
ダウンロードします。

ノードの起動
------------

AppImage はデフォルトでは実行可能ではありません — まず実行
権限を付与します:

.. code-block:: shell

  chmod +x irohad-x86_64.AppImage

ノードには他の ``irohad`` ビルドと同じ入力が必要です: JSON 設定
ファイル、genesis block、ノードの鍵ペア。リポジトリには ``example/``
に設定例（``config.sample`` 、``genesis.block`` 、
``node0.priv``/``node0.pub`` ）が用意されています — フォークを
クローンするか、このディレクトリをコピーしてください:

.. code-block:: shell

  git clone -b iroha1-main https://github.com/baziorek/iroha1-cpp --depth=1

初回起動では ``--overwrite_ledger`` を使用して、genesis block が
空のストレージに適用されるようにします:

.. code-block:: shell

  ./irohad-x86_64.AppImage \
    --config=iroha1-cpp/example/config.sample \
    --genesis_block=iroha1-cpp/example/genesis.block \
    --keypair_name=iroha1-cpp/example/node0 \
    --overwrite_ledger

.. note:: まず ``config.sample`` を環境に合わせて調整してください
  — 特に ``database`` セクション（PostgreSQL の host/認証情報または
  RocksDB）と ``torii_port`` です。完全なパラメータリファレンスは
  :doc:`configuration` にあります。

2 回目以降の起動では ``--genesis_block`` と ``--overwrite_ledger``
を省略できます — ノードはブロックストアから状態を復元します。

.. note:: リポジトリの ``config.sample`` は PostgreSQL を使用します。
  内蔵の RocksDB ストレージを使うには、書き込み可能なディレクトリを
  ``block_store_path`` に指定し、``database`` セクションを削除または
  修正してください — 詳細は :ref:`configuration` を参照。
