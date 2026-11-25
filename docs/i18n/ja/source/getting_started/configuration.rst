.. _node-configuration:

======================
ノードの設定
======================

各 ``irohad`` インスタンスは、``--config`` フラグで渡す 1 つの
JSON ファイルで設定されます（初回起動時は ``--genesis_block`` と
``--keypair_name`` とともに — :ref:`node-appimage` を参照）。

リポジトリには、コピーしてカスタマイズできる設定例が用意されて
います:

- ``example/config.sample`` — 汎用設定（localhost の PostgreSQL）
- ``example/config.docker`` — Docker イメージが使用
- ``example/config.postgres.sample`` — 明示的な PostgreSQL バリアント
- ``example/config-win.sample`` — Windows バリアント

.. note:: このガイドはコミュニティがメンテナンスするフォーク
  `baziorek/iroha1-cpp <https://github.com/baziorek/iroha1-cpp>`_
  に関するものです。

主要な設定項目
--------------

- ``database`` — ストレージバックエンド: ``postgres`` （host、port、
  認証情報、ワーキング/メンテナンス DB）または ``block_store_path``
  による RocksDB
- ``torii_port`` — クライアント向け gRPC ポート（デフォルト
  ``50051`` ）
- ``internal_port`` — ピア間 gRPC ポート（デフォルト ``10001`` ）
- ``max_proposal_size`` — ブロック/提案あたりの最大トランザクション数
- ``vote_delay`` / ``proposal_creation_timeout`` — コンセンサスの
  タイミング
- ``mst_enable`` / ``mst_expiration_time`` — マルチシグ
  トランザクションのサポート（:ref:`architecture` 参照）
- ``metrics`` / ``healthcheck_port`` — オブザーバビリティの
  エンドポイント

完全なリファレンス
------------------

すべての設定パラメータは、型、デフォルト値、単位、副作用を含めて
:ref:`configuration` で詳細に文書化されています。特に
` 環境依存パラメータ
<../configure/index.html#environment-specific-parameters>`_
のチューニングアドバイスを参照してください。

.. hint:: 複数ノードのネットワークでは、各ピアに独自の設定、鍵ペア、
  すべてのピアを含む genesis block が必要です。その構成は
  :ref:`deploy-guide` で説明しています。
