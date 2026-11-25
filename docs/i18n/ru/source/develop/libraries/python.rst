Библиотека Python
-----------------

Где получить
^^^^^^^^^^^^

Для этой документации (форк ``iroha1-cpp``) используйте форк
`iroha1-python <https://github.com/baziorek/iroha1-python>`__ —
он содержит **оригинальные исходники ``iroha-python`` без
изменений**, пересобранные на современных зависимостях, поэтому
устанавливается и работает на актуальных версиях Python.

Upstream-проект также доступен в `собственном репозитории
Hyperledger iroha-python
<https://github.com/hyperledger/iroha-python/>`__, но он больше
не поддерживается и не собирается на актуальных toolchain'ах.
Поддерживается Python 3+.

Библиотеку Python также можно установить через pip:

  .. code:: sh

      pip install iroha

Пример кода
^^^^^^^^^^^

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
