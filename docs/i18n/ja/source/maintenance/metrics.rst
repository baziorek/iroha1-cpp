==========
メトリクス
==========

ネットワークのパフォーマンスを便利かつ徹底的に監視するために、
メトリクスを使用できます。これは Iroha ネットワークについて時間
の経過とともに収集される数値データです。データを分析してプロジェ
クトをさらに効率的にすることができます!

メトリクスの使い方は?
=====================

メトリクスを使用するには、`Iroha 設定
<../configure/index.html#deployment-specific-parameters>`__ に追加
し、1.2.1 より新しい Iroha バージョンを使用する必要があります。

.. note:: Iroha を Docker で実行している場合、コンテナの外からメ
   トリクスにアクセスするには: 1) `設定
   <../configure/index.html>`__ で ``"metrics":0.0.0.0:PORT`` を設定
   する; 2) ``run ... -pPORT:PORT ...`` 実行時に Docker で対応する
   ポートを公開する

その後、実行中の Iroha インスタンスからデータにアクセスするため
に IP アドレスを使用できます。

例を示します:

.. code-block:: shell

  > curl http://127.0.0.1:8080/metrics

次のような結果が得られます:

.. code-block:: shell

  # HELP blocks_height Total number of blocks in chain
  # TYPE blocks_height gauge
  blocks_height 135543
  # HELP peers_number Total number peers to send transactions and request proposals
  # TYPE peers_number gauge
  peers_number 7
  # HELP number_of_domains Total number of domains in WSV
  # TYPE number_of_domains gauge
  number_of_domains 14
  # HELP total_number_of_transactions Total number of transactions in blockchain
  # TYPE total_number_of_transactions gauge
  total_number_of_transactions 216499
  # HELP number_of_signatures_in_last_block Number of signatures in last block
  # TYPE number_of_signatures_in_last_block gauge
  number_of_signatures_in_last_block 5

マルチシグネチャトランザクション（MST）メトリクス
==================================================

マルチシグネチャトランザクションが使用されている場合、エンドポイ
ントは署名を待っている MST のカウンターも公開します:

.. code-block:: shell

  # HELP number_of_pending_mst_batches Number of pending MST batches
  # TYPE number_of_pending_mst_batches gauge
  number_of_pending_mst_batches 0
  # HELP number_of_pending_mst_transactions Number of pending MST transactions
  # TYPE number_of_pending_mst_transactions gauge
  number_of_pending_mst_transactions 0

これらのゲージはローカルの保留中 MST キャッシュを反映します —
設定で ``mst_enable`` が ``true`` に設定されたピア（:ref:`configuration`
参照）で最も有用です。そこでは MST 状態がピア間で共有されます。
