.. _node-docker:

======================================
Węzeł Iroha z Dockerem
======================================

W tym przewodniku utworzymy bardzo podstawową sieć Irohy, uruchomimy ją,
stworzymy kilka transakcji i sprawdzimy dane zapisane w ledgerze.

Wymagania wstępne
-----------------------------------------------------------------------
Potrzebujesz maszyny z zainstalowanym ``Docker``. Instrukcję instalacji
znajdziesz na `stronie Dockera <https://docs.docker.com/get-docker/>`_.

.. note:: Oczywiście możesz zbudować Irohę od zera, zmodyfikować jej kod
  i uruchomić dostosowany węzeł! Jeśli jesteś ciekawy jak — sprawdź
  sekcję :ref:`build-guide`. W tym przewodniku użyjemy standardowej
  dystrybucji Irohy dostępnej jako obraz docker.

Uruchomienie węzła Iroha
-------------------------

.. raw:: html

  <script id="asciicast-345137" src="https://asciinema.org/a/345137.js" async></script>

Tworzenie sieci Docker
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Do działania Iroha wymaga bazy ``PostgreSQL`` albo ``RocksDB``.
Jeśli preferujesz ``PostgreSQL``, potrzebujesz kontenera z nią
uruchomionego obok Irohy.

Stwórzmy sieć Docker, żeby kontenery Postgresa i Irohy mogły działać w
tej samej sieci wirtualnej i komunikować się. Nazwiemy ją
``iroha-network``, ale możesz użyć dowolnej nazwy:

.. code-block:: shell

  docker network create iroha-network

Uruchomienie kontenera PostgreSQL
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note:: Nie musisz uruchamiać tego kontenera, jeśli planujesz używać
  Irohy z RocksDB, a nie PostgreSQL. Pomiń ten krok.

Uruchommy ``PostgreSQL`` w kontenerze, dołączmy go do utworzonej
wcześniej sieci i udostępnijmy porty do komunikacji:

.. code-block:: shell

  docker run --name some-postgres \
  -e POSTGRES_USER=postgres \
  -e POSTGRES_PASSWORD=mysecretpassword \
  -p 5432:5432 \
  --network=iroha-network \
  -d postgres:9.5 \
  -c 'max_prepared_transactions=100'

.. note:: Jeśli masz już Postgresa na hoście na domyślnym porcie (5432),
  wybierz inny wolny port, np. 5433: ``-p 5433:5432``

Tworzenie blockstore
^^^^^^^^^^^^^^^^^^^^

Przed uruchomieniem kontenera Irohy możemy utworzyć trwały wolumen na
pliki przechowujące bloki łańcucha:

.. code-block:: shell

  docker volume create blockstore

Przygotowanie plików konfiguracyjnych
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note:: Dla prostoty utworzymy w tym przewodniku sieć z tylko jednym
  węzłem. O uruchamianiu kilku peerów przeczytasz w :ref:`deploy-guide`.

Teraz musimy skonfigurować naszą sieć Irohy. Obejmuje to utworzenie
pliku konfiguracyjnego, generowanie par kluczy użytkowników, zapis
listy peerów i utworzenie genesis blocka. Nie bój się — przygotowaliśmy
przykładową konfigurację, więc możesz od razu testować węzeł.

Aby zdobyć te pliki, sklonuj to repozytorium albo skopiuj je ręcznie
(klonowanie jest szybsze):

.. note:: Ten przewodnik dotyczy forka utrzymywanego przez społeczność
  `baziorek/iroha1-cpp <https://github.com/baziorek/iroha1-cpp>`_,
  który utrzymuje Irohę 1 w stanie pozwalającym na budowanie na
  nowoczesnych toolchainach. Repozytorium upstream
  `hyperledger/iroha <https://github.com/hyperledger/iroha>`__ ma
  ``main`` z Irohą 2 (przepisanie w Rust) — źródła C++ są na gałęzi
  ``iroha1-main``.

.. code-block:: shell

  git clone -b iroha1-main https://github.com/baziorek/iroha1-cpp --depth=1

.. hint:: Opcja ``--depth=1`` pobiera tylko najnowszy commit, oszczędzając
  czas i pasmo. Jeśli chcesz pełną historię commitów, pomiń tę opcję.

Jest przewodnik jak ustawić parametry pod swoje środowisko i oczekiwane
obciążenie: :ref:`configuration`. Na razie nie musimy tego robić.

Uruchomienie kontenera Irohy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Prawie gotowi do uruchomienia kontenera Irohy. Potrzebujesz tylko znać
ścieżkę do plików konfiguracyjnych (z poprzedniego kroku). Uruchommy
węzeł Irohy w kontenerze Docker:

.. code-block:: shell

  docker run --name iroha \
  -d \
  -p 50051:50051 \
  -v $(pwd)/iroha1-cpp/example:/opt/iroha_data \
  -v blockstore:/tmp/block_store \
  --network=iroha-network \
  -e KEY='node0' \
  hyperledger/iroha:1.6.0

.. note:: Obraz ``hyperledger/iroha:1.6.0`` to ostatni oficjalny build
  upstream Irohy 1. **Nie używaj** tagu ``:latest`` — to nieaktualny
  obraz z 2022. Ten fork nie publikuje własnego obrazu runtime Dockera;
  dla builda forka użyj pakietów release'owych (deb/rpm/AppImage) lub
  buduj ze źródeł (zobacz :ref:`build-guide`). CI forka publikuje natomiast
  obraz **buildera** pod adresem
  ``ghcr.io/baziorek/iroha1-cpp:iroha1-main`` do kompilacji Irohy w
  Dockerze — zobacz :ref:`build-guide`.

.. note:: Jeśli używasz Irohy z RocksDB, dodaj ``--entrypoint irohad``
  do parametrów, żeby pominąć sprawdzanie kontenera PostgreSQL.
.. note:: Aby używać `metryk <../maintenance/metrics.html>`_ z Dockerem,
  ustaw też port ``-p7001:7001`` i edytuj plik ``config.docker`` na
  ``0.0.0.0:7001``.

Jeśli węzeł wystartował pomyślnie, zobaczysz id kontenera w konsoli,
w której go uruchomiłeś.

Przyjrzyjmy się, co robi ta komenda:

- ``docker run --name iroha \`` tworzy kontener ``iroha``
- ``-d \`` uruchamia kontener w tle
- ``-p 50051:50051 \`` udostępnia port do komunikacji z klientem
  (użyjemy go później)
- ``-v TWOJA_ŚCIEŻKA_DO_PLIKÓW_CONF:/opt/iroha_data \`` — tak
  przekazujemy pliki konfiguracyjne do kontenera Dockera. Katalog
  example jest wskazany w bloku kodu powyżej.
- ``-v blockstore:/tmp/block_store \`` dodaje trwały storage bloków
  (wolumen Dockera) do kontenera, żeby bloki nie ginęły po zatrzymaniu
  kontenera
- ``--network=iroha-network \`` dodaje nasz kontener do utworzonej
  wcześniej ``iroha-network`` na potrzeby komunikacji z serwerem
  PostgreSQL
- ``-e KEY='node0' \`` — tu podaj nazwę klucza identyfikującego węzeł,
  pozwalającą mu potwierdzać operacje. Klucze powinny leżeć w katalogu
  z plikami konfiguracyjnymi.
- ``hyperledger/iroha:1.6.0`` — referencja do obrazu wskazująca na
  ostatni upstreamowy release Irohy 1 (1.6.0)

Logi sprawdzisz przez ``docker logs iroha``.

Możesz wypróbować jeden z przykładowych przewodników, aby wysłać
transakcje do Irohy i zapytać o jej stan.
