.. _architecture:

Iroha の内部構造
=================

HL Iroha ネットワークは、ノード間の通信を提供するいくつかの
重要なコンポーネントで構成されています。以下でそれらについて
説明します。

.. image:: ../../image_assets/pipeline-diagram.png
	:width: 80%
	:align: center
	:alt: Iroha Architecture Diagram

Torii
-----

⛩

*Torii* () は、神社の入口を示す伝統的な日本の門です — Iroha の
唯一のエントリーポイントにぴったりの名前です。

`クライアント <glossary.html#client>`__ 向けのエントリーポイント。
トランスポートとして gRPC を使用します。
Iroha と対話するには、`Commands
<../develop/api/commands.html>`__ および `Queries
<../develop/api/queries.html>`__ セクションで説明されている gRPC
エンドポイントを使用するか、`クライアントライブラリ
<../develop/libraries.html>`__ を使用できます。

MST Processor
-------------

*マルチシグネチャトランザクションプロセッサ*

他のピアとの間で `Gossip プロトコル
<https://en.wikipedia.org/wiki/Gossip_protocol>`_ を通じてメッ
セージを送受信する内部 gRPC サービスです。その使命は、`クォー
ラム <glossary.html#quorum>`_ に達するのに十分な署名をまだ
得ていない `マルチシグネチャトランザクション
<glossary.html#multisignature-transactions>`_ を、クォーラムに
達するまで配信し続けることです。

.. note::
    ピア間のマルチシグネチャ（MST）トランザクションの伝播は、
    ノード設定で ``mst_enable`` が ``true`` に設定されている場合
    のみ機能します（:ref:`configuration` を参照）。フラグが
    オフの場合、MST トランザクションは各ピアでローカルに動作
    しますが、ピア間で保留中の MST トランザクションを交換し
    ません。

Peer Communication Service
--------------------------

Iroha の内部コンポーネント — `Torii <#torii>`__ から
`MstProcessor <#MstProcessor>`_ を経て `Ordering Gate
<#ordering-gate>`__ へ `トランザクション
<glossary.html#transaction>`__ を中継する仲介者です。PCS の
主な目的は、コンセンサス実装との相互作用の複雑さを隠すことで
す。

Ordering Gate
-------------

Iroha の内部コンポーネント（gRPC クライアント）で、
`Peer Communication Service
<#peer-communication-service>`__ から `Ordering Service
<#ordering-service>`__ へ `トランザクション
<glossary.html#transaction>`__ を中継します。Ordering Gate は
Ordering Service から `提案 <glossary.html#proposal>`_
（チェーン内の潜在的なブロック）を受け取り、`ステートフル検証
<glossary.html#stateful-validation>`__ のために `シミュレータ
<#simulator>`__ に送信します。また、コンセンサスラウンドに基づ
いて Ordering Service に提案を要求します。

Ordering Service
----------------

Iroha の内部コンポーネント（gRPC サーバー）で、他の `ピア
<glossary.html#peer>`__ からメッセージを受信し、`ステートレス
検証 <glossary.html#stateless-validation>`__ を通過した複数の
`トランザクション <glossary.html#transaction>`__ を 1 つの
`提案 <glossary.html#proposal>`__ にまとめます。各ノードには
独自の ordering service があります。提案の作成は次のいずれかの
イベントによってトリガーされます:

1. トランザクション収集に割り当てられた時間制限が切れた。

2. ordering service が単一の提案に許可された最大数のトランザ
   クションを受信した。

両方のパラメータ（タイムアウトと提案の最大サイズ）は設定可能で
す（`環境依存パラメータ
<../configure/index.html#environment-specific-parameters>`_
ページを参照）。

両方のトリガーに共通する前提条件は、少なくとも 1 つのトランザ
クションが ordering service に到達することです。そうでなけれ
ば、提案は形成されません。

ordering service は提案の予備検証も実行します（例: 提案から
ステートレスに拒否されたトランザクションを除去）。

Verified Proposal Creator
-------------------------

`Ordering Service <#ordering-service>`__ から受信した `提案
<glossary.html#proposal>`__ に含まれる `トランザクション
<glossary.html#transaction>`__ の `ステートフル検証
<glossary.html#stateful-validation>`__ を実行する Iroha の内部
コンポーネントです。ステートフル検証を通過したトランザクション
に基づいて **検証済み提案** が作成され、`ブロッククリエイター
<#block-creator>`__ に渡されます。ステートフル検証を通過しな
かったすべてのトランザクションは破棄され、検証済み提案には
含まれません。

Block Creator
-------------

`ステートレス
<glossary.html#stateless-validation>`__ および `ステートフル
<glossary.html#stateful-validation>`__ 検証を通過したトランザ
クションのセットからブロックを形成し、さらなる `コンセンサス
<#consensus>`__ への伝播のためのシステムコンポーネントです。

Block creator は `Verified Proposal Creator
<#verified-proposal-creator>`__ とともに、`シミュレータ
<https://github.com/baziorek/iroha1-cpp/tree/iroha1-main/irohad/simulator>`_
と呼ばれるコンポーネントを形成します。

Block Consensus (YAC)
---------------------

*コンポーネントとしてのコンセンサス*

コンセンサスはブロックチェーンの心臓部です — ピアネットワーク内
の `ピア <glossary.html#peer>`__ 間で一貫した状態を維持します。
Iroha は Yet Another Consensus（通称 YAC）と呼ばれる独自の
コンセンサスアルゴリズムを使用します。プロトコルの詳細な説明
— ラウンド、投票、スーパー多数決モデル、障害処理 — は
:ref:`yac` を参照してください。

コンセンサスの原理、特に YAC について詳しく説明した動画は
`こちら <https://youtu.be/mzuAbalxOKo>`__ でご覧いただけます。

YAC アルゴリズムの特徴は、そのスケーラビリティ、パフォーマンス、
クラッシュ障害耐性です。

ネットワーク内の一貫性を確保するため、不足しているブロックは
`同期化 <#synchronizer>`__ を通じて別のピアからダウンロードさ
れます。コミットされたブロックは `Ametsuchi
<#ametsuchi>`__ ブロックストレージに保存されます。

コンセンサスの一般的な定義は `このリンク
<glossary.html#consensus>`_ を参照してください。


Synchronizer
------------

`コンセンサス <#consensus>`__ の一部です。`ピア
<glossary.html#peer>`__ のチェーンに不足しているブロックを追加
します（一貫性を保つため他のピアからダウンロード）。

Ametsuchi Blockstore
--------------------

*Ametsuchi* () は日本の神話における天の宝物の保管庫です — ここ
では、ブロックとブロックから生成された状態（`World State View
<#world-state-view>`__ と呼ばれます）を保存する Iroha のスト
レージコンポーネントです。`クライアント
<glossary.html#client>`__ が Ametsuchi と直接対話する方法はあり
ません。


World State View
----------------

WSV はシステムの現在の状態を反映し、スナップショットと見なす
ことができます。たとえば、WSV は `アカウント
<glossary.html#account>`__ が現在保有する `アセット
<glossary.html#asset>`__ の量に関する情報を保持しますが、
`トランザクション <glossary.html#transaction>`__ フローの履歴
情報は含みません。

Python ライブラリを使ったトランザクションの送信
================================================

新しいターミナルを開き（Iroha コンテナと ``irohad`` が起動
していることを確認）、``iroha`` docker コンテナにアタッチしま
す:

.. code-block:: shell

  docker exec -it iroha /bin/bash

これで Iroha コンテナの対話シェルにいます。

前提条件
--------

.. note:: このライブラリは Python 3 環境でのみ動作します
  （Python 2 はサポートされていません）。

Iroha Python ライブラリを使用するには、`リポジトリ
<https://github.com/baziorek/iroha1-python>`_
（`iroha-python
<https://github.com/hyperledger/iroha-python>`_ のコミュニティ
フォーク）から取得するか、pip3 経由でインストールします:

.. code-block:: shell

	pip3 install iroha

Python ライブラリを使った独自キーの作成
----------------------------------------

テスト目的では、サンプルキーを使用できます。しかし独自のキーを
作成することもできます。**ネイティブ Iroha ed25519 SHA-3**
キーを作成するには（アルゴリズムの違いは `こちら
<../develop/keys.html>`__）、次のコードを使用してください:

.. code-block:: python

	from iroha import IrohaCrypto

	# この最初の 2 行でキーを作成できます
	private_key = IrohaCrypto.private_key()
	public_key = IrohaCrypto.derive_public_key(private_key)

	# 残りのコードはそれらをファイルに書き込みます
	with open('keypair.priv', 'wb') as f:
	    f.write(private_key)

	with open('keypair.pub', 'wb') as f:
	    f.write(public_key)

Multihash 形式の HL Ursa ed25519 SHA-2 キーには、次を使用して
ください:

.. code-block:: python

	from iroha import IrohaCrypto, ed25519_sha2
	from nacl.encoding import HexEncoder

	private_key = ed25519_sha2.SigningKey.generate()
	public_key = IrohaCrypto.derive_public_key(private_key).encode('ascii')

	with open('keypair.priv', 'wb') as f:
	    f.write(private_key.encode(encoder=HexEncoder))
	    f.write(public_key[6:])

	with open('keypair.pub', 'wb') as f:
	    f.write(public_key)

ライブラリとキーができたので、実際のトランザクションを送信
し始めることができます。

サンプルトランザクションの実行
------------------------------

Iroha のトランザクションがどのように見えるか試したいだけな
ら、リポジトリの例 `こちら
<https://github.com/baziorek/iroha1-python/tree/iroha1-main/examples>`__
に進むだけで構いません。各ステップを説明するコメント付きの
`tx-example.py` ファイルはこちらです:

.. remoteliteralinclude:: https://raw.githubusercontent.com/baziorek/iroha1-python/iroha1-main/examples/tx-example.py
   :language: python

``irohad`` が実行されていれば、別のタブで .py ファイルを開く
だけで、例または独自のファイルを実行できます。

.. note:: `iroha-python の例ディレクトリ
  <https://github.com/baziorek/iroha1-python/tree/iroha1-main/examples>`__
  にはさらにサンプルがあります。
