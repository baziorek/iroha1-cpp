================================================
Wysyłanie transakcji za pomocą biblioteki Python
================================================

Otwórz nowy terminal (upewnij się, że kontener Iroha i ``irohad``
są uruchomione) i dołącz do kontenera docker ``iroha``:

.. code-block:: shell

  docker exec -it iroha /bin/bash

Teraz jesteś w interaktywnej powłoce kontenera Iroha.

Wymagania wstępne
-----------------

.. note:: Biblioteka działa tylko w środowisku Python 3 (Python 2
  nie jest obsługiwany).

Aby używać biblioteki Python Iroha, musisz ją pobrać z
`repozytorium <https://github.com/baziorek/iroha1-python>`_
(community fork `iroha-python
<https://github.com/hyperledger/iroha-python>`_) lub przez pip3:

.. code-block:: shell

	pip3 install iroha

Tworzenie własnych par kluczy za pomocą biblioteki Python
----------------------------------------------------------

Do celów testowych możesz użyć przykładowych kluczy. Ale możesz
też utworzyć własne. Aby utworzyć **natywne klucze Iroha ed25519
SHA-3** (różnicę między algorytmami znajdziesz `tutaj
<../develop/keys.html>`__), użyj następującego kodu:

.. code-block:: python

	from iroha import IrohaCrypto

	# te pierwsze dwie linie wystarczą do utworzenia kluczy
	private_key = IrohaCrypto.private_key()
	public_key = IrohaCrypto.derive_public_key(private_key)

	# reszta kodu zapisuje je do pliku
	with open('keypair.priv', 'wb') as f:
	    f.write(private_key)

	with open('keypair.pub', 'wb') as f:
	    f.write(public_key)

A dla kluczy HL Ursa ed25519 SHA-2 w formacie Multihash użyj:

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

Teraz, gdy mamy bibliotekę i klucze, możemy zacząć wysyłać
rzeczywiste transakcje.

Uruchamianie przykładowych transakcji
-------------------------------------

Jeśli chcesz tylko zobaczyć, jak wyglądają transakcje Iroha,
możesz po prostu przejść do przykładów z repozytorium `tutaj
<https://github.com/baziorek/iroha1-python/tree/iroha1-main/examples>`_.
Oto plik `tx-example.py` z komentarzami wyjaśniającymi każdy krok:

.. remoteliteralinclude:: https://raw.githubusercontent.com/baziorek/iroha1-python/iroha1-main/examples/tx-example.py
   :language: python

Teraz, jeśli masz uruchomiony ``irohad``, możesz uruchomić
przykład lub własny plik, po prostu otwierając plik .py w
innej karcie.

.. note:: W `katalogu przykładów iroha-python
  <https://github.com/baziorek/iroha1-python/tree/iroha1-main/examples>`_
  znajduje się więcej próbek.
