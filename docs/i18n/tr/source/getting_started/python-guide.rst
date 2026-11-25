Python kütüphanesiyle işlem gönderme
====================================

Yeni bir terminal açın (Iroha konteynerinin ve ``irohad``'ın
çalıştığından emin olun) ve ``iroha`` docker konteynerine
bağlanın:

.. code-block:: shell

  docker exec -it iroha /bin/bash

Artık Iroha konteynerinin etkileşimli kabuğundasınız.

Önkoşullar
----------

.. note:: Kütüphane yalnızca Python 3 ortamında çalışır (Python 2
desteklenmez).

Iroha Python kütüphanesini kullanmak için onu `depodan
<https://github.com/baziorek/iroha1-python>`_ (`iroha-python
<https://github.com/hyperledger/iroha-python>`_ topluluk
çatallanması) edinmeniz veya pip3 ile kurmanız gerekir:

.. code-block:: shell

	pip3 install iroha

Python kütüphanesiyle kendi anahtar çiftlerinizi oluşturma
-----------------------------------------------------------

Test amaçları için örnek anahtarları kullanabilirsiniz. Ancak
kendi anahtarlarınızı da oluşturabilirsiniz. **Yerel Iroha
ed25519 SHA-3** anahtarları oluşturmak için (algoritmalar
arasındaki fark `burada <../develop/keys.html>`__), şu kodu
kullanın:

.. code-block:: python

	from iroha import IrohaCrypto

	# bu ilk iki satır anahtarları oluşturmak için yeterlidir
	private_key = IrohaCrypto.private_key()
	public_key = IrohaCrypto.derive_public_key(private_key)

	# geri kalan kod onları dosyaya yazar
	with open('keypair.priv', 'wb') as f:
	    f.write(private_key)

	with open('keypair.pub', 'wb') as f:
	    f.write(public_key)

Multihash biçimindeki HL Ursa ed25519 SHA-2 anahtarları için ise
şunu kullanın:

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

Kütüphane ve anahtarlar elimizde olduğuna göre, gerçek işlemleri
göndermeye başlayabiliriz.

Örnek işlemleri çalıştırma
--------------------------

Iroha işlemlerinin nasıl göründüğünü denemek istiyorsanız,
doğrudan depodaki örneklere `buradan
<https://github.com/baziorek/iroha1-python/tree/iroha1-main/examples>`__
gidebilirsiniz. Her adımı açıklayan yorumlarla birlikte
`tx-example.py` dosyası:

.. remoteliteralinclude:: https://raw.githubusercontent.com/baziorek/iroha1-python/iroha1-main/examples/tx-example.py
   :language: python

``irohad`` çalışıyorsa, .py dosyasını başka bir sekmede açarak
örneği veya kendi dosyanızı çalıştırabilirsiniz.

.. note:: `iroha-python örnek dizininde
  <https://github.com/baziorek/iroha1-python/tree/iroha1-main/examples>`__
  daha fazla örnek var.
