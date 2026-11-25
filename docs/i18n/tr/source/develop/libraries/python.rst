Python Kütüphanesi
------------------

Nereden edinilir
^^^^^^^^^^^^^^^^

Bu dokümantasyon için (``iroha1-cpp`` fork'u),
`iroha1-python <https://github.com/baziorek/iroha1-python>`__
fork'unu kullanın — **orijinal ``iroha-python`` kaynaklarını
değiştirmeden** içerir ve modern bağımlılıklara karşı yeniden
derlenmiştir, böylece mevcut Python sürümlerinde yüklenir ve
çalışır.

Upstream proje de `kendi Hyperledger iroha-python deposunda
<https://github.com/hyperledger/iroha-python/>`__ mevcuttur,
ancak artık bakımsızdır ve mevcut araç zincirlerinde
derlenmez. Python 3+ desteklenir.

Python kütüphanesini pip ile de yükleyebilirsiniz:

  .. code:: sh

      pip install iroha

Örnek kod
^^^^^^^^^

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
