.. _syncing-node:

=========================
同期ノード (Syncing node)
=========================

** 同期ノード** （syncing node）は、チェーンに追従するものの
コンセンサスには参加しない Iroha ピアです。提案を作成せず、投票も
せず、クォーラムにも数えられません。その役割は、クエリを処理する
ため、読み取りレプリカとして、あるいは新しいフルノードを
バックフィルするために、ローカルの world state view (WSV) を
高速に最新に保つことです — フルのコンセンサスメカニズムのコストを
かけずに。

同期ピアは通常のピアと同じようにネットワークに登録されます。
``Peer`` メッセージは ``syncing_peer`` フラグを持ち、これは
`Add Peer <../develop/api/commands.html#add-peer>`_ コマンド
（または genesis block）によって設定されます。コマンドを送信する
アカウントには ``can_add_peer`` 権限が必要です — 通常のピアと
同様です。

ノードの起動
------------

``irohad`` を ``--syncing_node`` フラグ付きで起動します:

.. code-block:: shell

    irohad --config example/config.sample \
           --genesis_block example/genesis.block \
           --keypair_name example/node0 \
           --syncing_node

このフラグは ``config.syncing_mode`` に反映され、ordering と
コンセンサスの初期化時に使用されます。ノードは YAC 投票をスキップ
し、代わりにピアから確定済みブロックを取り込みます。

.. note:: 同期ノードも正しい設定、鍵ペア、データベースが必要です
  — ブロックと WSV を他のピアと同様に保存します。ただし、それらを
  生成したり投票したりはしません。

関連する便利なフラグ
--------------------

- ``--exit_after_init`` — WSV をブロックストアから再構築して
  終了します。メンテナンス後やストレージバックエンド変更後の
  再インデックスに便利です。:ref:`deploy-flags` を参照。
- ``--wait_for_new_blocks`` — ネットワークを起動せずにローカルの
  ブロックストアの新しいブロックを待つ起動ポリシー。

関連項目
--------

- :doc:`../concepts_architecture/glossary` — **Peer** の
  *syncing peer* エントリ
- :ref:`yac` — 通常の（検証ピア）コンセンサスの仕組み
- :doc:`../configure/index` — ``healthcheck_port`` がノードステータス
  応答で ``is_syncing`` を公開
