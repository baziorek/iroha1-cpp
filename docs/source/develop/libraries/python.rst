Python Library
--------------

Where to Get
^^^^^^^^^^^^

For this documentation (the ``iroha1-cpp`` fork), use the `iroha1-python <https://github.com/baziorek/iroha1-python>`__ fork — it contains the **original ``iroha-python`` sources unchanged**, rebuilt against modern dependencies so it installs and works on current Python versions.

The upstream project is also available at its `own Hyperledger iroha-python repo <https://github.com/hyperledger/iroha-python/>`__, but it is no longer maintained and does not build against current toolchains.
Python 3+ is supported.

You can also install Python library via pip:

  .. code:: sh

      pip install iroha

Example Code
^^^^^^^^^^^^

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
