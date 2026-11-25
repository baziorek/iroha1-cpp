Biblioteka Python
-----------------

Gdzie zdobyć
^^^^^^^^^^^^

Dla tej dokumentacji (fork ``iroha1-cpp``) użyj forka
`iroha1-python <https://github.com/baziorek/iroha1-python>`__ —
zawiera on **oryginalne źródła ``iroha-python`` bez zmian**,
odtworzone na nowo względem współczesnych zależności, więc
instaluje się i działa na aktualnych wersjach Pythona.

Upstreamowy projekt jest również dostępny w `własnym
repozytorium Hyperledger iroha-python
<https://github.com/hyperledger/iroha-python/>`__, ale nie jest
już utrzymywany i nie buduje się na aktualnych toolchainach.
Obsługiwany jest Python 3+.

Bibliotekę Python możesz też zainstalować przez pip:

  .. code:: sh

      pip install iroha

Przykładowy kod
^^^^^^^^^^^^^^^

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
