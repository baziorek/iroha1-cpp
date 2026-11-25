.. _architecture:

Co jest w Iroha?
================

Sieć HL Iroha składa się z kilku istotnych komponentów, które
zapewniają komunikację między węzłami. Możesz o nich przeczytać
poniżej.

.. image:: ../../image_assets/pipeline-diagram.png
	:width: 80%
	:align: center
	:alt: Iroha Architecture Diagram

Torii
-----

⛩

*Torii* () to tradycyjna japońska brama oznaczająca wejście do
świątyni — trafna nazwa dla jednego punktu wejścia Irohy.

Punkt wejścia dla `klientów <glossary.html#client>`__.
Używa gRPC jako transportu.
Aby wchodzić w interakcję z Irohą, każdy może używać endpointów
gRPC opisanych w sekcjach `Commands
<../develop/api/commands.html>`__ i `Queries
<../develop/api/queries.html>`__, albo skorzystać z `bibliotek
klienckich <../develop/libraries.html>`__.

MST Processor
-------------

*Procesor transakcji multisignature*

To wewnętrzna usługa gRPC, która wysyła i odbiera wiadomości od
innych peerów przez `protokół Gossip
<https://en.wikipedia.org/wiki/Gossip_protocol>`_. Jej misją jest
rozsyłanie `transakcji multisignature
<glossary.html#multisignature-transactions>`_, które nie zebrały
jeszcze wystarczającej liczby podpisów, aby osiągnąć `kworum
<glossary.html#quorum>`_, dopóki ono nie zostanie osiągnięte.

.. note::
    Rozsyłanie transakcji multisignature (MST) między peerami działa
    tylko gdy ``mst_enable`` jest ustawione na ``true`` w
    konfiguracji węzła (zobacz :ref:`configuration`). Przy
    wyłączonej fladze transakcje MST nadal działają lokalnie na
    każdym peerze, ale peerowie nie wymieniają się oczekującymi
    transakcjami MST.

Peer Communication Service
--------------------------

Wewnętrzny komponent Irohy — pośrednik, który przesyła
`transakcję <glossary.html#transaction>`__ z `Torii <#torii>`__
przez `MstProcessor <#MstProcessor>`_ do `Ordering Gate
<#ordering-gate>`__. Głównym celem PCS jest ukrycie złożoności
interakcji z implementacją konsensusu.

Ordering Gate
-------------

To wewnętrzny komponent Irohy (klient gRPC), który przekazuje
`transakcje <glossary.html#transaction>`__ z `Peer Communication
Service <#peer-communication-service>`__ do `Ordering Service
<#ordering-service>`__. Ordering Gate odbiera `propozycje
<glossary.html#proposal>`_ (potencjalne bloki w łańcuchu) od
Ordering Service i wysyła je do `Symulatora <#simulator>`__ w celu
`walidacji stanowej <glossary.html#stateful-validation>`__. Żąda
również propozycji od Ordering Service na podstawie rundy
konsensusu.

Ordering Service
----------------

Wewnętrzny komponent Irohy (serwer gRPC), który odbiera wiadomości
od innych `peerów <glossary.html#peer>`__ i łączy kilka
`transakcji <glossary.html#transaction>`__, które przeszły
`walidację bezstanową <glossary.html#stateless-validation>`__, w
`propozycję <glossary.html#proposal>`__. Każdy węzeł ma własny
ordering service. Utworzenie propozycji może być wywołane jednym
z następujących zdarzeń:

1. Wygasł limit czasu poświęcony na zbieranie transakcji.

2. Ordering service otrzymał maksymalną dozwoloną liczbę
   transakcji dla pojedynczej propozycji.

Oba parametry (timeout i maksymalny rozmiar propozycji) są
konfigurowalne (sprawdź stronę `parametrów zależnych od
środowiska <../configure/index.html#environment-specific-parameters>`_).

Wspólnym warunkiem wstępnym dla obu wyzwalaczy jest to, że co
najmniej jedna transakcja powinna dotrzeć do ordering service.
W przeciwnym razie propozycja nie zostanie ut tworzona.

Ordering service wykonuje również wstępną walidację propozycji
(np. usuwanie transakcji odrzuconych bezstanowo z propozycji).

Verified Proposal Creator
-------------------------

Wewnętrzny komponent Irohy, który wykonuje `walidację stanową
<glossary.html#stateful-validation>`__ `transakcji
<glossary.html#transaction>`__ zawartych w otrzymanej `propozycji
<glossary.html#proposal>`__ od `Ordering Service
<#ordering-service>`_. Na podstawie transakcji, które przeszły
walidację stanową, zostanie utworzona **zweryfikowana propozycja**
i przekazana do `Block Creatora <#block-creator>`__. Wszystkie
transakcje, które nie przeszły walidacji stanowej, zostaną
odrzucone i nie zostaną uwzględnione w zweryfikowanej propozycji.

Block Creator
-------------

Komponent systemu, który tworzy blok ze zbioru transakcji, które
przeszły walidację `bezstanową <glossary.html#stateless-validation>`__
i `stanową <glossary.html#stateful-validation>`__, do dalszego
rozpowszechniania w `konsensusie <#consensus>`__.

Block creator, razem z `Verified Proposal Creatorem
<#verified-proposal-creator>`_, tworzy komponent zwany
`Symulatorem <https://github.com/baziorek/iroha1-cpp/tree/iroha1-main/irohad/simulator>`_.

Block Consensus (YAC)
---------------------

*Konsensus jako komponent*

Konsensus jest sercem blockchaina — utrzymuje spójny stan wśród
`peerów <glossary.html#peer>`__ w sieci peerów. Iroha używa
własnego algorytmu konsensusu zwanego Yet Another Consensus
(znany jako YAC). Szczegółowy opis protokołu — rundy, głosowanie,
modele superwiększości i obsługa awarii — zobacz :ref:`yac`.

Możesz obejrzeć film ze szczegółowym wyjaśnieniem zasad konsensusu,
a w szczególności YAC, `tutaj
<https://youtu.be/mzuAbalxOKo>`__.

Charakterystycznymi cechami algorytmu YAC są skalowalność,
wydajność i odporność na awarie (Crash fault tolerance).

Aby zapewnić spójność w sieci, jeśli brakuje bloków, zostaną
one pobrane od innego peera przez `Synchronizator <#synchronizer>`__.
Zatwierdzone bloki są przechowywane w magazynie bloków
`Ametsuchi <#ametsuchi>`__.

Ogólną definicję konsensusu znajdziesz `pod tym linkiem
<glossary.html#consensus>`_.


Synchronizer
------------

Jest częścią `konsensusu <#consensus>`__. Dodaje brakujące bloki do
`łańcuchów peerów <glossary.html#peer>`__ (pobiera je od innych
peerów, aby zachować spójność).

Ametsuchi Blockstore
--------------------

*Ametsuchi* () to magazyn niebiańskich skarbów w japońskiej
mitologii — tutaj, komponent magazynu Irohy, który przechowuje
bloki i stan generowany z bloków, zwany `World State View
<#world-state-view>`__. `Klient
<glossary.html#client>`__ nie ma sposobu bezpośredniej interakcji
z Ametsuchi.


World State View
----------------

WSV odzwierciedla aktualny stan systemu i można go traktować jako
migawkę. Na przykład WSV przechowuje informacje o ilości
`aktywów <glossary.html#asset>`__, które `konto
<glossary.html#account>`__ ma w danym momencie, ale nie zawiera
żadnych informacji o historii przepływu `transakcji
<glossary.html#transaction>`__. Wysyłanie transakcji za pomocą
biblioteki Python
=================

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
