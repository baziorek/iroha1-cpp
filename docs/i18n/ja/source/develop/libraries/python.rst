Python ライブラリ
------------------

入手先
^^^^^^

このドキュメント（``iroha1-cpp`` フォーク）では、
`iroha1-python <https://github.com/baziorek/iroha1-python>`__
フォークを使用してください — これは**オリジナルの
``iroha-python`` ソースを変更せずに**含んでおり、最新の依存関係
に対して再ビルドされているため、現在の Python バージョンでインス
トールして動作します。

アップストリームプロジェクトは `独自の Hyperledger
iroha-python リポジトリ
<https://github.com/hyperledger/iroha-python/>`__ でも利用できま
すが、もはやメンテナンスされておらず、現在のツールチェーンではビ
ルドできません。Python 3+ がサポートされています。

Python ライブラリは pip 経由でインストールすることもできます:

  .. code:: sh

      pip install iroha

サンプルコード
^^^^^^^^^^^^^^

.. code:: python

    from iroha import Iroha, IrohaCrypto, IrohaGrpc

    iroha = Iroha('alice@test')
    net = IrohaGrpc('127.0.0.1:50051')

    alice_key = IrohaCrypto.private_key()
    alice_tx = iroha.transaction(
     [iroha.command(
         'TransferAsset',
         src_account_id='alice@test',
         dest_account_id='bob@test',
         asset_id='bitcoin#test',
         description='test',
         amount='1'
     )]
    )

    IrohaCrypto.sign_transaction(alice_tx, alice_key)
        net.send_tx(alice_tx)

    for status in net.tx_status_stream(alice_tx):
        print(status)
