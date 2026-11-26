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
