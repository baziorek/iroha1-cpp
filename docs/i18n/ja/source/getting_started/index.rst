.. _getting-started:

=================
クイックスタート
=================

Hyperledger Iroha を使うには、2 つのものが必要です:

#. ** 少なくとも 1 つの Iroha ノード** — ネットワークを作成または
   参加する動作中の ``irohad`` プロセス
#. ** トランザクションを送信する手段** — gRPC 経由でノードと通信する
   クライアント: ``iroha-cli`` ツール、クライアントライブラリの
   いずれか、または独自のコード

このガイドは両方を順に説明します。まず Iroha ノードの入手方法を
選び、次にそれとの通信方法を選びます。

.. note:: Ledger はブロックチェーンの同義語であり、Hyperledger Iroha
  は Distributed Ledger Technology フレームワークとしても知られ
  ています — 実質的に「ブロックチェーンフレームワーク」と同じです。
  その他の用語は :ref:`core-concepts` を参照してください。

.. toctree::
   :maxdepth: 2
   :caption: Iroha ノードを入手する

   node-docker.rst
   node-appimage.rst
   node-deb.rst
   node-build.rst

.. toctree::
   :maxdepth: 2
   :caption: 通信ツール — ``iroha-cli``

   cli.rst

.. toctree::
   :maxdepth: 2
   :caption: クライアントライブラリ

   cli-guide.rst
   python-guide.rst

.. toctree::
   :maxdepth: 2
   :caption: ノードの設定

   configuration.rst
