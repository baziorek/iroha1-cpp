.. _wsv-storage:

=====================
Layout storage WSV
=====================

Iroha może przechowywać `World State View (WSV)
<../concepts_architecture/glossary.html#wsv>`__ w dwóch zamiennych
backendach: **PostgreSQL** (domyślny) i **RocksDB** (głównie dla
wdrożeń jednowęzłowych i testów). Przechowują dokładnie te same dane
logiczne, ale w zupełnie różnych layoutach fizycznych — w Postgresie
nie ma w ogóle "kluczy", a RocksDB to pojedynczy płaski key-value store,
w którym każdy rekord jest adresowany ręcznie zbudowanym kluczem
tekstowym.

Ta strona wyjaśnia, jak czytać oba layouty, np. przy inspekcji bazy
przez ``ldb``/``psql``.

Backend PostgreSQL
==================

Postgres używa znormalizowanego schematu relacyjnego — jedna tabela na
rodzaj encji, łączone kluczami obcymi. Nie ma kodowania kluczy:
rekordy znajdują się przez zapytania SQL budowane w
``irohad/ametsuchi/impl/postgres_wsv_command.cpp`` i
``postgres_wsv_query.cpp``.

Główne tabele (tworzone w ``irohad/main/impl/pg_connection_init.cpp``):

.. csv-table::
    :header: "Tabela", "Zawartość"
    :widths: 35, 65

    "``domain``", "domeny i ich role domyślne"
    "``asset``", "aktywa i ich precyzja"
    "``account``", "konta"
    "``account_has_asset``", "salda aktywów per konto"
    "``account_has_signatory``", "sygnatariusze per konto"
    "``account_has_roles``", "role dołączone do kont"
    "``account_has_grantable_permissions``", "uprawnienia, które konto może nadawać"
    "``role`` / ``role_has_permissions``", "role i ich bitfieldy uprawnień"
    "``signatory``", "klucze publiczne sygnatariuszy"
    "``peer`` / ``sync_peer``", "walidujące i synchronizujące peery"
    "``tx_positions`` / ``tx_status_by_hash``", "pozycje i statusy transakcji"
    "``setting``", "ustawienia węzła (np. limit rozmiaru opisu)"
    "``engine_calls`` / ``burrow_*``", "potwierdzenia CallEngine i storage Burrow (EVM)"

Backend RocksDB
===============

RocksDB trzyma wszystko w jednym drzewie LSM. Klucz każdego rekordu to
``/``-delimitowany string zbudowany z jednoliterowych segmentów
*mnemonicznych*, dzięki czemu całe poddrzewo można przeskanować jednym
zakresem prefiksowym.

Layout jest zdefiniowany raz w ``irohad/ametsuchi/impl/rocksdb_common.hpp``
— jego blok komentarza zawiera pełne drzewo katalogów, legendę
mnemonik i działające przykłady; same format stringów leży w
przestrzeni nazw ``fmtstrings`` tego samego nagłówka i są wypełniane
przez ``dbCall`` w ``rocksdb_common.cpp``.

Anatomia klucza
---------------

Klucz to ścieżka segmentów rozdzielonych ``/``::

    wD/<domain_id>/a/<account_name>/x/<asset_id>

Czytając go legendą z ``rocksdb_common.hpp``:

.. csv-table::
    :header: "Segment", "Znaczenie"
    :widths: 15, 85

    "``w``", "korzeń WSV (wszystkie klucze stanu świata zaczynają się od niego)"
    "``D``", "katalog domen"
    "``a``", "katalog kont wewnątrz domeny"
    "``x``", "katalog aktywów wewnątrz konta"
    "``<asset_id>``", "samo aktywo — wartością jest saldo"

Więcej przykładów z nagłówka:

- ``wta/<account>/T/<timestamp>/`` — transakcje konta po znaczniku
  czasu (``GetAccountTransactions``)
- ``wD/<domain>/a/<account>/x`` — salda aktywów konta
  (``GetAccountAssets``)
- ``wr/<role_name>`` — rola → bitfield uprawnień
- ``wu/<tx_hash>`` — status transakcji po hashu
- ``wn/M/<pubkey>`` — adres peeru po kluczu publicznym

Dwa poziomy strukturalne mają znaczenie przy czytaniu surowych kluczy:

- **Katalogi** kończą się ``/`` i grupują rekordy; iteracja prefiksu
  zwraca wszystko poniżej (np. wszystkie konta domeny).
- **Pola/pliki** to klucze liściowe; część niesie wartość, inne działają
  jako markery (np. ``.../S/<pubkey>`` oznacza sygnatariusza — wartość
  jest pusta).

Liczniki i singletony używają dedykowanych kluczy na poziomie pliku,
jak ``.../O/q`` (kworum konta), ``.../O/V`` (liczba transakcji) i
``.../Z`` (liczba peerów) — pełną listę znajdziesz w legendzie nagłówka.

.. note::
    Kompletna legenda mnemonik (``s`` store, ``w`` WSV, ``n`` sieć,
    ``t`` transakcje, ``D`` domena, ``a`` konta, ``x`` aktywa, ``r``
    role, ``S`` sygnatariusze, ``d`` detale, ``g`` uprawnienia
    nadawalne, ``O`` opcje, ``P``/``T`` pozycja/znacznik czasu,
    ``e``/``E``/``W``/``R``/``y``/``Y``/``A`` storage wywołań
    silnika, …) jest udokumentowana w bloku komentarza na górze
    ``irohad/ametsuchi/impl/rocksdb_common.hpp`` — traktuj ją jako
    autorytatywną referencję przy dekodowaniu kluczy.

Czy Postgres i RocksDB generują klucze inaczej?
===============================================

Tak — zasadniczo. Postgres nie ma w ogóle kluczy tekstowych: tożsamość
określają wiersze i kolumny tabel, a cały dostęp idzie przez
parametryzowany SQL w ``postgres_wsv_command.cpp`` /
``postgres_wsv_query.cpp``. RocksDB natomiast koduje te same encje w
hierarchiczne ścieżki tekstowe przez szablony ``fmtstrings`` z
``rocksdb_common.hpp``, wykonywane przez helper ``dbCall`` ze
sprawdzeniami istnienia ``kMustExist`` / ``kMustNotExist``.

Oba backendy implementują te same interfejsy ``CommandExecutor`` /
``QueryExecutor``, więc wybór jest przezroczysty dla reszty węzła —
różni się tylko inspekcja na dysku i w sieci.

Porównywanie zawartości WSV między węzłami
------------------------------------------

Repozytorium zawiera małe narzędzie
``test/tool/test_tool_iroha_wsv_diff.sh``, które porównuje world state
view dwóch instancji Irohy — przydatne do sprawdzenia, że peery
zbiegnęły się do tego samego stanu po synchronizacji lub migracji.
Wymaga ``jq`` i ``psql`` (``postgresql-client``) na maszynie, na
której jest uruchamiane, i działa z backendem PostgreSQL.

Powiązane
---------

- `Przewodnik migracji RocksDB ↔ Postgres <../maintenance/migration-rocksdb.html>`__
- `Glosariusz: WSV <../concepts_architecture/glossary.html#wsv>`__
- Źródło prawdy dla layoutu kluczy:
  ``irohad/ametsuchi/impl/rocksdb_common.hpp``
