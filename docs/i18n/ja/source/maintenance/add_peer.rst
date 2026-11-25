============
ピアの追加
============

HL Iroha では、ネットワークが稼働している間に新しいピアを追加で
きます。これは特別なコマンド `AddPeer
<../develop/api/commands.html#add-peer>`__ を使用して行います。

要件
====

**次の条件を満たすピアが存在する必要があります:**

— ネットワーク内の既存ピアと同一の Genesis Block（ブロックチェー
ンの初期ブロック）で実行されている;

— 解決可能なアドレスを持っている;

— ピア鍵ペア（SHA-2/SHA-3 を使用する Ed25519）を持っている

.. important:: ピアを追加するトランザクションを送信するアカウン
   トは、ロールに `Can Add Peer
   <../develop/api/permissions.html#can-add-peer>`__ パーミッションを
   持ち、ピアを削除するには `Can Remove Peer
   <../develop/api/permissions.html#can-remove-peer>`__ パーミッション
   が必要です — これは genesis block で設定する必要があります。

使用方法
========

`API リファレンス
<../develop/api/commands.html#add-peer>`__ に記載されているように、
コマンドを使用するには以下だけが必要です:

— ネットワークに追加したいピアの公開鍵;

— ピアの解決可能な IP アドレス

手順:

1. ユーザーに割り当てられた genesis block に `Can Add Peer
   <../develop/api/permissions.html#can-add-peer>`__ と `Can
   Remove Peer
   <../develop/api/permissions.html#can-remove-peer>`__
   パーミッションを設定してネットワークを作成する;
2. 同じ genesis block と類似の設定で HL Iroha を実行する別のピア
   を作成する;
3. 必要なパーミッションを持つアカウントから ``add peer``
   コマンドを含むトランザクションを送信する（以下の例を参照）
4. ピアのログを確認して、すべてが正しく動作しているか確認する。

トランザクションを送信し、ノード上のブロック数が同じか確認するこ
ともできます。

.. note:: 稼働中の既存ピアが 1 つだけの場合、追加するピアを、すべ
   てのピア（既存のものと新しいものの両方）を `設定
   <../configure/index.html#environment-specific-parameters>`__ の
   "initial_peers" パラメータに持つように設定する必要があります。
   これが必要な別のケースは、ネットワークがしばらく稼働しており、
   genesis block に示されたピアがもはや存在しない場合です（新しいピ
   アが追加されたときに Remove Peer コマンドで削除されたため）。

例
==

Python でのコマンドの例を示します。この例では、`Can Add Peer
<../develop/api/permissions.html#can-add-peer>`__ と `Can Remove
Peer <../develop/api/permissions.html#can-remove-peer>`__
を含むすべてのパーミッションを持つ `Root パーミッション
<../develop/api/permissions.html#root>`__ を使用しました:

.. code-block:: python

    def add_peer():
        peer1 = primitive_pb2.Peer()
        peer1.address = '192.168.1.1:50541'
        peer1.peer_key = '716fe505f69f18511a1b083915aa9ff73ef36e6688199f3959750db38b8f4bfc'
        tx = iroha.transaction([
            iroha.command('AddPeer', peer=peer1)
        ], creator_account=ADMIN_ACCOUNT_ID, quorum=1)

        IrohaCrypto.sign_transaction(tx, ADMIN_PRIVATE_KEY)

    add_peer()

ピアの削除
==========

ピアを削除するには、``CanRemovePeer`` パーミッションを持つアカウン
トから `Remove Peer
<../develop/api/commands.html#remove-peer>`__ コマンドを使用する必
要があります。
