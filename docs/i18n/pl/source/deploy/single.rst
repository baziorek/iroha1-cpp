==================================
Uruchamianie pojedynczej instancji
==================================

Zazwyczaj ludzie chcą uruchomić Irohę lokalnie, aby wypróbować
API i zbadać jej możliwości. Można to zrobić w środowisku
lokalnym lub kontenerowym (Docker). Zbadamy oba możliwe
przypadki, ale aby uprościć wdrożenie komponentów peera,
*zaleca się posiadanie Dockera zainstalowanego na maszynie*.

Środowisko lokalne
------------------

Przez środowisko lokalne rozumiemy proces demona i Postgres
wdrożone bez żadnych kontenerów. Może to być pomocne w
przypadkach, gdy zabieganie z Dockerem nie jest preferowane —
zazwyczaj szybkie eksplorowanie funkcji.

Uruchamianie serwera postgres
"""""""""""""""""""""""""""""

Aby uruchomić serwer postgres lokalnie, powinieneś sprawdzić
stronę postgres `website
<https://www.postgresql.org/docs/current/static/server-start.html>`__
i postępować zgodnie z opisem. Zazwyczaj serwer postgres
uruchamia się automatycznie przy starcie systemu, ale warto to
sprawdzić w konfiguracji systemu.

Serwer bazy danych Postgres można zainicjalizować i uruchomić
ręcznie bez zwykłej integracji systemowej:

.. code-block::shell

   initdb ~/iroha/nodeX_db/
   ## Start server in background, logs will appear in current console
   postgres -D ~/iroha/nodaX_db/ -p5433 &
   createuser -s iroha_user -p5433

Wybrany port ``5433`` (domyślnie 5432) i użytkownik bazy
``iroha_user`` są używane przez irohad do łączenia się z bazą.
(Zobacz `Parametry konfiguracyjne
<../configure/index.html>`_, aby uzyskać więcej informacji.)
Baza maintenance ``postgres`` jest tworzona domyślnie, ale jeśli
z jakiegoś powodu potrzebna jest inna nazwa, utwórz ją:

.. code-block::shell

   createdb iroha_mainteance -p5433


Uruchamianie demona Iroha (irohad)
""""""""""""""""""""""""""""""""""""""

Jest lista warunków wstępnych, które powinieneś spełnić przed
przystąpieniem:

* Serwer Postgres działa
* Binarka demona Iroha `irohad` jest zbudowana i dostępna w
  systemie
* Pliki bloku genesis i konfiguracji zostały utworzone
* Plik konfiguracyjny używa prawidłowych ustawień połączenia
  postgres
* Para kluczy dla peera została wygenerowana
* To pierwszy raz, gdy uruchamiasz Irohę na tym peerze i chcesz
  utworzyć nowy łańcuch

.. Hint:: Masz coś, co nie zgadza się z listą założeń? Odwołaj
  się do sekcji :ref:`deploy_troubles`.

W przypadku spełnionych założeń jedyne, co pozostaje, to
uruchomienie procesu demona z następującymi parametrami:

+---------------+----------------------------------------------------------------+
| Parametr      | Znaczenie                                                      |
+===============+================================================================+
| config        | plik konfiguracyjny zawierający połączenie postgres i wartości |
|               | do strojenia systemu                                           |
| genesis_block | początkowy blok w ledgerze                                     |
| keypair_name  | nazwy plików kluczy prywatnego i publicznego bez rozszerzenia, |
|               | używane przez peera do podpisywania bloków                     |
+---------------+----------------------------------------------------------------+

.. Attention:: Określenie nowego bloku genesis przez
  `--genesis_block` przy istniejących już blokach w ledgerze
  wymaga ustawienia flagi `--overwrite_ledger`. W przeciwnym
  razie demon się nie uruchomi.

Przykład polecenia powłoki uruchamiającego demona Iroha to:

.. code-block:: shell

    irohad --config example/config.sample --genesis_block example/genesis.block --keypair_name example/node0

.. Note:: jeśli uruchamiasz Irohę zbudowaną z `wsparciem HL Ursa
  <../integrations/index.html#hyperledger-ursa>`_, pobierz
  przykładowe klucze i blok genesis z `example/ursa-keys/`

.. Attention:: Jeśli zatrzymałeś demona i chcesz użyć istniejącego
  łańcucha — nie powinieneś przekazywać parametru bloku genesis.


Docker
------

Aby uruchomić peer Iroha jako pojedynczą instancję w Dockerze,
powinieneś najpierw pobrać obraz Irohy:

.. code-block:: shell

    docker pull hyperledger/iroha:1.6.0

.. note:: ``hyperledger/iroha:1.6.0`` to ostatni oficjalny obraz
  Iroha 1 upstream — **nie** używaj ``:latest`` (przestarzałego
  builda z 2022) ani tagów ``:develop``/``:main`` (teraz
  wskazują na buildy Irohy 2).

Następnie musisz utworzyć środowisko, aby obraz działał bez
problemów:

Tworzenie sieci docker
""""""""""""""""""""""

Kontenery Postgres i Iroha powinny działać w tej samej sieci
wirtualnej, aby były dla siebie dostępne. Utwórz sieć, wpisując
następujące polecenie (możesz użyć dowolnej nazwy, ale w
przykładzie używamy *iroha-network*):

.. code-block:: shell

    docker network create iroha-network

Uruchamianie Postgresql w kontenerze
""""""""""""""""""""""""""""""""""""

Podobnie uruchom serwer postgres, dołączając go do utworzonej
wcześniej sieci i udostępniając porty do komunikacji:

.. code-block:: shell

    docker run --name some-postgres \
    -e POSTGRES_USER=postgres \
    -e POSTGRES_PASSWORD=mysecretpassword \
    -p 5432:5432 \
    --network=iroha-network \
    -d postgres:9.5

Tworzenie wolumenu dla przechowywania bloków
""""""""""""""""""""""""""""""""""""""""""""

Zanim uruchomimy demona iroha w kontenerze, powinniśmy utworzyć
trwałe wolumen do przechowywania plików bloków łańcucha. Robi się
to następującym poleceniem:

.. code-block:: shell

    docker volume create blockstore

Uruchamianie demona iroha w kontenerze docker
"""""""""""""""""""""""""""""""""""""""""""""""""""""""

Jest lista założeń, które powinieneś przejrzeć przed
przystąpieniem:

* Serwer Postgres działa w tej samej sieci docker
* Istnieje folder zawierający plik konfiguracyjny i parę kluczy
  dla pojedynczego węzła
* To pierwszy raz, gdy uruchamiasz Irohę na tym peerze i chcesz
  utworzyć nowy łańcuch

Jeśli są spełnione, możesz przejść do następnego polecenia:

.. code-block:: shell

    docker run --name iroha \
    # External port
    -p 50051:50051 \
    # Folder with configuration files
    -v ~/Developer/iroha/example:/opt/iroha_data \
    # Blockstore volume
    -v blockstore:/tmp/block_store \
    # Postgres settings
    -e POSTGRES_HOST='some-postgres' \
    -e POSTGRES_PORT='5432' \
    -e POSTGRES_PASSWORD='mysecretpassword' \
    -e POSTGRES_USER='postgres' \
    # Node keypair name
    -e KEY='node0' \
    # Docker network name
    --network=iroha-network \
    hyperledger/iroha:1.6.0

.. note:: ``hyperledger/iroha:1.6.0`` to ostatni oficjalny obraz
  Iroha 1 upstream — nie używaj ``:latest`` (przestarzałego
  builda z 2022).

.. note:: Przy **pierwszym starcie** Iroha przebudowuje WSV z
  blockstore, co jest żądne pamięci — przy dużych ledgerach może
  to zabić kontener przez OOM (zobacz
  `issue #1833 <https://github.com/hyperledger-iroha/iroha/issues/1833>`_).
  Kontenery Docker nie mają domyślnie limitu pamięci, ale na
  **Docker Desktop** (macOS/Windows) cała VM ma limit pamięci —
  zwiększ go w *Settings → Resources* lub przekaż ``--memory`` do
  ``docker run`` na hostach Linux.
