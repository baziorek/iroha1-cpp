Iroha API reference
===================

API セクションでは、Iroha と対話するアプリケーションの構成要
素を見ていきます。システムが持つコマンドとクエリ、および
トランスポートとアプリケーション層のロジックを包含するクライ
アントライブラリのセットを概観します。

Iroha API は `コマンド・クエリ分離
<https://en.wikipedia.org/wiki/Command%E2%80%93query_separation>`_
の原則に従います。

Iroha ピアとクライアントアプリケーション間の通信は `gRPC
<https://grpc.io/about/>`_ フレームワークを介して維持されま
す。クライアントアプリケーションは説明されたプロトコルに従
い、その `説明
<../concepts_architecture/glossary.html#transaction>`__ に
従ってトランザクションを形成する必要があります。


.. toctree::
    :maxdepth: 2
    :caption: 目次

    api/commands.rst
    api/queries.rst
    api/permissions.rst

ここでのコマンドは `トランザクション
<../concepts_architecture/glossary.html#transaction>`__ の一部
— システム内の状態を変更する一連のアクションです。トランザク
ションが検証とコンセンサスの段階を通過すると、`ブロック
<../concepts_architecture/glossary.html#block>`__ に書き込まれ、
不変のブロックストア（ブロックチェーン）に保存されます。

トランザクションは、システム内の `エンティティ
<../concepts_architecture/er_model.html>`__ に対するアクショ
ンを実行するコマンドで構成されます。エンティティはアカウント、
アセットなどです。
