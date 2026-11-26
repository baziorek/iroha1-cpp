Отправка транзакций с помощью библиотеки Python
================================================

Откройте новый терминал (обратите внимание, что контейнер Iroha и
``irohad`` должны быть запущены) и подключитесь к docker-контейнеру
``iroha``:

.. code-block:: shell

  docker exec -it iroha /bin/bash

Теперь вы в интерактивной оболочке контейнера Iroha.

Предварительные требования
---------------------------

.. note:: Библиотека работает только в среде Python 3 (Python 2 не
  поддерживается).

Чтобы использовать библиотеку Python Iroha, её нужно получить из
`репозитория <https://github.com/baziorek/iroha1-python>`_
(community-форк `iroha-python
<https://github.com/hyperledger/iroha-python>`_) или через pip3:

.. code-block:: shell

	pip3 install iroha

Создание собственных пар ключей с помощью библиотеки Python
-----------------------------------------------------------

Для тестовых целей можно использовать примерные ключи. Но можно
создать и свои. Чтобы создать **нативные ключи Iroha ed25519
SHA-3** (различие между алгоритмами можно найти `здесь
<../develop/keys.html>`__), используйте следующий код:

.. code-block:: python

	from iroha import IrohaCrypto

	# этих двух первых строк достаточно для создания ключей
	private_key = IrohaCrypto.private_key()
	public_key = IrohaCrypto.derive_public_key(private_key)

	# остальной код записывает их в файл
	with open('keypair.priv', 'wb') as f:
	    f.write(private_key)

	with open('keypair.pub', 'wb') as f:
	    f.write(public_key)

А для ключей HL Ursa ed25519 SHA-2 в формате Multihash
используйте:

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

Теперь, когда у нас есть библиотека и ключи, мы можем начать
отправлять реальные транзакции.

Запуск примерных транзакций
---------------------------

Если вы просто хотите посмотреть, как выглядят транзакции Iroha,
можно просто перейти к примерам из репозитория `здесь
<https://github.com/baziorek/iroha1-python/tree/iroha1-main/examples>`_.
Вот файл `tx-example.py` с комментариями, поясняющими каждый шаг:

.. remoteliteralinclude:: https://raw.githubusercontent.com/baziorek/iroha1-python/iroha1-main/examples/tx-example.py
   :language: python

Теперь, если у вас запущен ``irohad``, вы можете запустить пример
или собственный файл, просто открыв файл .py в другой вкладке.

.. note:: В `каталоге примеров iroha-python
  <https://github.com/baziorek/iroha1-python/tree/iroha1-main/examples>`_
  есть ещё образцы.
