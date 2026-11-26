.. _configuration:

============
Konfiguracja
============

.. toctree::
      :maxdepth: 1

      torii-tls.rst
      db.rst

W tej sekcji zrozumiemy, jak skonfigurować Irohę.
Niektóre parametry konfiguracji muszą być takie same
we wszystkich węzłach (są oznaczone \*), a niektóre mogą
się różnić. Przyjrzyjmy się
``example/config.sample``

.. note:: Od v1.2 ``irohad`` można konfigurować również
   przez zmienne środowiskowe, nie tylko przez plik
   konfiguracyjny.

Zaczniemy od pliku konfiguracyjnego, a potem
zobaczymy, jak Irohę można skonfigurować za pomocą
`zmiennych środowiskowych <#environment-variables>`_.

.. literalinclude:: ../../../example/config.sample
   :language: json


Jak widać, plik konfiguracyjny jest prawidłową strukturą
``json``. Przejdźmy linię po linii i zrozumiejmy, co
oznacza każdy parametr w formacie pliku konfiguracyjnego.

Parametry zależne od wdrożenia
==============================

- ``block_store_path`` (opcjonalny) ustawia ścieżkę do
  folderu, w którym przechowywane są bloki. Jeśli ten
  parametr nie zostanie podany, bloki będą przechowywane
  w bazie danych.
- ``torii_port`` ustawia port dla komunikacji zewnętrznej.
  Tutaj wysyłane są zapytania i transakcje.
- ``internal_port`` ustawia port dla komunikacji
  wewnętrznej: serwis porządkowania, konsensus i
  block loader.
- ``database`` (opcjonalny) służy do ustawienia
  konfiguracji bazy danych (zobacz poniżej)
- ``pg_opt`` (opcjonalny) to **przestarzały** sposób
  ustawiania poświadczeń PostgreSQL: nazwa hosta, port,
  nazwa użytkownika, hasło i nazwa bazy danych.
  Wszystkie dane oprócz nazwy bazy danych są obowiązkowe.
  Jeśli nazwa bazy danych nie zostanie podana, używana
  jest domyślna, czyli ``iroha_default``.
- ``log`` to opcjonalny parametr kontrolujący
  szczegółowość i format wyjścia logów (zobacz poniżej).
- ``utility_service`` (opcjonalny) endpoint dla zadań
  konserwacyjnych. Jeśli obecny, musi zawierać adres
  ``ip`` i ``port``, do którego się powiąże.
  Przykład użycia endpointu konserwacyjnego zobacz
  w `dokumentacji shepherd <../maintenance/shepherd.html>`_.
- ``metrics`` (opcjonalny) endpoint do monitorowania
  metryk Irohy. Serwer HTTP Prometheus nasłuchuje na
  tym endpoincie. Jeśli obecny, musi odpowiadać formatowi
  "[addr]:<port>" i może to być na przykład
  "127.0.0.1:8080", "9090" lub ":1234".
  Nieprawidłowe wartości po cichu wyłączają serwer metryk
  Prometheus. Istnieją również opcje wiersza poleceń
  ``--metrics_port`` i ``--metrics_addr``, które
  nadpisują ten parametr.
- ``healthcheck_port`` (opcjonalny) endpoint dla
  healthcheck Irohy. Wysłanie żądania na ten endpoint
  w postaci ``http://<host>:<healthcheck_port>/healthcheck``
  zwróci informacje o statusie węzła: bieżące zużycie
  pamięci (``memory_consumption``), bieżącą liczbę bloków
  (``last_block_round``), bieżącą liczbę rund odrzuceń
  (``last_reject_round``), czy węzeł w danym momencie
  synchronizuje informacje ze zdalnym węzłem
  (``is_syncing``) oraz czy węzeł jest obecnie uruchomiony
  (``status``).

Istnieje również opcjonalny parametr ``torii_tls_params``,
który można dołączyć do konfiguracji, aby włączyć
wsparcie TLS dla komunikacji z klientem.

Tam ``port`` to port TCP, na którym zostanie powiązany
serwer TLS, a ``key_pair_path`` to ścieżka do pary kluczy
w formacie takim, że dodanie ``.crt`` daje ścieżkę do
certyfikatu w kodowaniu PEM, a dodanie ``.key`` daje
ścieżkę do klucza prywatnego w kodowaniu PEM dla tego
certyfikatu (np. jeśli ``key_pair_path`` to
``"/path/to/the/keypair"``, Iroha będzie szukać
certyfikatu w ``"/path/to/the/keypair.crt"`` i klucza w
``"/path/to/the/keypair.key"``)

.. warning::
   Pole konfiguracyjne ``pg_opt`` jest przestarzałe,
   prosimy używać sekcji ``database``!

   Sekcja ``database`` nadpisuje ``pg_opt``, gdy obie
   są podane w konfiguracji.

   Zarówno ``pg_opt``, jak i ``database`` są
   opcjonalne, ale co najmniej jeden musi być podany.

Pola sekcji ``database``:

- ``host`` — host używany do połączenia PostgreSQL
- ``port`` — port używany do połączenia PostgreSQL
- ``user`` — użytkownik używany do połączenia PostgreSQL
- ``password`` — hasło używane do połączenia PostgreSQL
- ``working database`` — nazwa bazy danych, która będzie
  używana do przechowywania world state view i
  opcjonalnie bloków.
- ``maintenance database`` — nazwa bazy danych, która
  będzie używana do utrzymania bazy roboczej. Na przykład,
  gdy Iroha musi utworzyć lub usunąć swoją bazę roboczą,
  musi użyć innej bazy, aby połączyć się z PostgreSQL.

Parametry zależne od środowiska
===============================

- ``max_proposal_size`` \* to maksymalna liczba transakcji,
  która może znajdować się w jednej propozycji, a w
  rezultacie również w pojedynczym bloku. Zmieniając tę
  wartość, definiujesz rozmiar potencjalnego bloku. Na
  start możesz zostać przy ``10``. Jednak zalecamy
  zwiększenie tej liczby, jeśli masz dużo transakcji na
  sekundę.

    **Ten parametr wpływa na wydajność.** Zwiększ ten
    parametr, jeśli Twoja sieć ma dużą liczbę transakcji.
    Jeśli zwiększysz ``max_proposal_size`` z powodu
    zwiększonego przepływu, możesz zwiększyć go
    niezależnie. Zwiększając ten parametr możesz poprawić
    wydajność, ale pamiętaj, że w pewnym momencie
    zwiększanie tej wartości może prowadzić do
    pogorszenia wydajności.

- ``vote_delay`` \* to czas oczekiwania w milisekundach
  przed wysłaniem głosu do następnego peera. Optymalna
  wartość zależy w dużej mierze od liczby peerów Iroha w
  sieci (większa liczba węzłów wymaga dłuższego
  ``vote_delay``). ** Zdecydowanie zalecamy ustawienie
  go na co najmniej jedną sekundę — w przeciwnym razie,
  gdy niektórzy peerzy nie są łatwo osiągalni, łańcuch
  bloków będzie rósł bardzo wolno lub w ogóle przestanie
  rosnąć.**

    **Ten parametr wpływa tylko na mechanizm
    konsensusu.** Jeśli Twoja sieć jest szybka — wszystko
    w porządku i ten parametr nie wpływa na nią zbytnio.
    Ale jeśli Twoja sieć jest wolniejsza, zwiększ go,
    aby dać peerom więcej czasu na odpowiedź.

- ``mst_enable`` włącza lub wyłącza transport sieciowy
  transakcji wielopodpisowych w Iroha.
  Zauważ, że silnik MST działa zawsze dla każdego peera,
  nawet gdy flaga jest ustawiona na ``false``.
  Flaga jedynie pozwala na udostępnianie informacji o
  transakcjach MST wśród peerów.

- ``mst_expiration_time`` to opcjonalny parametr
  określający okres czasu, w którym nie w pełni
  podpisana transakcja (lub partia) jest uważana za
  wygasłą (w minutach). Wartość domyślna to 1440.

- ``proposal_creation_timeout`` (wcześniej nazwany
  ``max_rounds_delay``)\* to opcjonalny parametr
  określający maksymalny czas (w milisekundach), który
  serwis porządkowania czeka podczas składania
  propozycji — tj. zbierania transakcji do
  ``max_proposal_size`` — zanim odpowie na żądanie
  propozycji. Wartość domyślna to 3000.
  Drugi, pochodny timeout — sieciowy deadline dla
  RPC ``RequestProposal`` między peerami — jest
  obliczany automatycznie jako ``2 ×
  proposal_creation_timeout`` i nie jest konfigurowalny
  osobno.

    **Ten parametr wpływa na zużycie zasobów.** Gdy
    możesz spodziewać się, że Iroha pozostanie bezczynna
    przez dłuższe okresy i chcesz zaoszczędzić zasoby,
    zwiększ tę wartość — spowoduje to rzadsze sprawdzanie
    nowych transakcji przez Irohę. NB: pierwsza
    transakcja po okresie bezczynności może być nieco
    opóźniona z tego powodu. Drugi i kolejne bloki będą
    przetwarzane szybciej.

- ``stale_stream_max_rounds`` to opcjonalny parametr
  określający maksymalną liczbę rund, przez które
  strumień statusu pozostaje otwarty, gdy nie jest
  raportowana żadna aktualizacja statusu. Wartość
  domyślna to 2. Zwiększenie tej wartości zmniejsza
  liczbę ponownych połączeń klienta potrzebnych do
  śledzenia transakcji, jeśli z jakiegoś powodu nie
  jest ona aktualizowana nowymi rundami. Jednak duże
  wartości zwiększają średnią liczbę połączonych
  klientów w każdej rundzie.

    Zaleca się ograniczenie tego parametru, aby upewnić
    się, że węzeł nie jest przeciążony strumieniami.

- ``initial_peers`` to opcjonalny parametr określający
  listę peerów, których węzeł użyje po uruchomieniu
  zamiast peerów z bloku genesis. Może być przydatne,
  gdy dodajesz nowy węzeł do sieci, w której większość
  początkowych peerów mogła stać się złośliwa. Lista
  peerów powinna być podana jako tablica JSON:

.. code-block:: javascript

  "initial_peers": [
    {
      "address": "127.0.0.1:10001",
      "public_key": "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
    }
  ]

- ``max_past_created_hours``: opcjonalny parametr
  określający, ile godzin w przeszłości od bieżącego
  czasu (mierzonego na peerze) może być ustawione
  `created_time` transakcji. Wartość domyślna to
  ``"24"`` godziny. Ta wartość musi być taka sama na
  wszystkich peerach, w przeciwnym razie może po cichu
  spowodować zatrzymanie produkcji bloków przez sieć.

Przykład dobrych praktyk
------------------------

Przy równomiernym rozkładzie uzyskaliśmy całkiem dobre
wyniki — 300k transakcji wysłanych w 5 minut.
Commit trwał od 2 sekund do 2 minut.
**Zwróć uwagę, że wyniki zawsze zależą od liczby peerów
w Twojej sieci, jej szybkości i parametrów hostów, na
których działają peerzy.**

Oto konfiguracja, której użyliśmy:

.. code-block:: javascript

  "max_proposal_size" : 10000,
  "vote_delay" : 1000,
  "mst_enable" : true,
  "mst_expiration_time": 1440,
  "proposal_creation_timeout": 500,
  "stale_stream_max_rounds": 100000


Zmienne środowiskowe
====================

Innym sposobem konfiguracji Irohy jest użycie zmiennych
środowiskowych. Plik konfiguracyjny i zmienne
środowiskowe można łączyć. **Parametry podane w pliku
konfiguracyjny, jeśli obecne, nadpiszą te ustawione
przez środowisko.**

Oto kilka przykładów, jak parametry będą wyglądać
w

Unix
----

.. code-block:: bash

  export IROHA_BLOCK_STORE_PATH=/tmp/block_store/
  export IROHA_TORII_PORT=50051
  export IROHA_INTERNAL_PORT=10001
  export IROHA_PG_OPT="host=172.19.0.2 port=5432 user=iroha password=helloworld"
  export IROHA_MAX_PROPOSAL_SIZE=10
  export IROHA_VOTE_DELAY=5000
  export IROHA_MST_ENABLE=false
  export IROHA_MST_EXPIRATION_TIME=1440
  export IROHA_PROPOSAL_CREATION_TIMEOUT=3000
  export IROHA_CRYPTO_PROVIDERS_0_KEY=p1
  export IROHA_CRYPTO_PROVIDERS_0_CRYPTO_TYPE=ed25519_sha3_256
  export IROHA_CRYPTO_PROVIDERS_0_PRIVATE_KEY=cc5013e43918bd0e5c4d800416c88bed77892ff077929162bb03ead40a745e88
  export IROHA_CRYPTO_PROVIDERS_0_TYPE=default
  export IROHA_CRYPTO_SIGNER=p1

Windows
-------

.. code-block:: bat

  setx IROHA_BLOCK_STORE_PATH C:\block_store
  setx IROHA_TORII_PORT 50051
  setx IROHA_INTERNAL_PORT 10001

PowerShell
----------

.. code-block:: bash

  $Env:IROHA_BLOCK_STORE_PATH="C:\block_store"
  $Env:IROHA_TORII_PORT="50051"
  $Env:IROHA_INTERNAL_PORT="10001"

Nazwy parametrów
----------------

Jak widać, nazwy parametrów nie są takie same jak w pliku
konfiguracyjnym.

Są one tworzone ze struktury konfiguracji, na początku
dodawana jest stała etykieta IROHA, a wszystko jest
pisane wielkimi literami i łączone znakiem _.
Przyjrzyjmy się bliżej, jak są zbudowane:

**Z prostymi wartościami tekstowymi**

W pliku konfiguracyjnym:

.. code-block:: javascript

  "block_store_path": "/tmp/block_store/"

W zmiennych środowiskowych:

.. code-block:: bash

  IROHA_BLOCK_STORE_PATH=/tmp/block_store/


**Z tablicami**

Tablice są indeksowane od 0 i powinny być w kolejności
direct bez pomijania żadnych liczb:

W pliku konfiguracyjnym:

.. code-block:: javascript

  "initial_peers": [
    {
      "address": "127.0.0.1:10001",
      "public_key": "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
    },
    {
      "address": "127.0.0.1:10002",
      "public_key": "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308920"
    }
  ]

W zmiennych środowiskowych:

.. code-block:: bash

  IROHA_INITIAL_PEERS_0_ADDRESS=127.0.0.1:10001
  IROHA_INITIAL_PEERS_0_PUBLIC_KEY=bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929
  IROHA_INITIAL_PEERS_1_ADDRESS=127.0.0.1:10002
  IROHA_INITIAL_PEERS_1_PUBLIC_KEY=bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308920

**Słowniki z kluczami zdefiniowanymi przez użytkownika**

Klucze słowników dostarczane przez użytkownika są nieco
bardziej skomplikowane: klucz i wartość są ustawiane w
osobnych zmiennych. Można to zilustrować na przykładzie
konfiguracji dostawców kryptografii:

Dostawcy kryptografii
=====================

Obecnie HL Iroha obsługuje jeden typ dostawcy o nazwie
``default`` — zawiera on wbudowanych dostawców
kryptografii, takich jak biblioteka kryptograficzna HL
Iroha (z typem kryptograficznym ``ed25519_sha3_256``) i
biblioteka HL Ursa, dla której Iroha obecnie obsługuje
typ kryptograficzny ``ed25519_sha2_256``.

Oba z nich przyjmują jedynie ``crypto_type`` i
``private_key`` jako parametry.

.. note::  Pracujemy nad dodaniem innych typów,
   w tym ``hsm`` — dla modułów bezpieczeństwa
   sprzętowego — będą one mieć inny zestaw parametrów,
   które zostaną dodane tutaj po wydaniu.

Konfiguracja dostawców kryptografii
-----------------------------------

Aby skonfigurować obecnie dostępnych dostawców
kryptografii, musisz zdefiniować dostawców, którzy
mogą być używani na peerze (zobacz ``p1`` i ``p2``),
a następnie wybrać ``signer``, który będzie używany
do podpisywania wiadomości na tym węźle:

W pliku konfiguracyjnym:

.. code-block:: javascript

  "crypto": {
    "providers": {
      "p1": {
        "crypto_type": "ed25519_sha3_256",
        "private_key": "cc5013e43918bd0e5c4d800416c88bed77892ff077929162bb03ead40a745e88",
        "type": "default"
      },
      "p2": {
        "crypto_type": "ed25519_sha2_256",
        "private_key": "7bab70e95cb585ea052c3aeb27de0afa9897ba5746276aa1c25310383216ceb860eb82baacbc940e710a40f21f962a3651013b90c23ece31606752f298c38d90",
        "type": "default"
      }
    },
    "signer": "p1"
  }

W zmiennych środowiskowych:

.. code-block:: bash

  IROHA_CRYPTO_PROVIDERS_0_KEY=p1
  IROHA_CRYPTO_PROVIDERS_0_CRYPTO_TYPE=ed25519_sha3_256
  IROHA_CRYPTO_PROVIDERS_0_PRIVATE_KEY=cc5013e43918bd0e5c4d800416c88bed77892ff077929162bb03ead40a745e88
  IROHA_CRYPTO_PROVIDERS_0_TYPE=default
  IROHA_CRYPTO_PROVIDERS_1_KEY=p2
  IROHA_CRYPTO_PROVIDERS_1_CRYPTO_TYPE=ed25519_sha2_256
  IROHA_CRYPTO_PROVIDERS_1_PRIVATE_KEY=7bab70e95cb585ea052c3aeb27de0afa9897ba5746276aa1c25310383216ceb860eb82baacbc940e710a40f21f962a3651013b90c23ece31606752f298c38d90
  IROHA_CRYPTO_PROVIDERS_1_TYPE=default
  IROHA_CRYPTO_SIGNER=p1


Logowanie
=========

W Iroha logowanie można dostosować tak szczegółowo, jak
chcesz. Każdy komponent ma własną konfigurację logowania
z właściwościami dziedziczonymi od swojego rodzica,
które można nadpisać przez plik konfiguracyjny.
Oznacza to, że wszystkie loggery komponentów są
zorganizowane w drzewo z pojedynczym korzeniem. Odpowiednia
sekcja pliku konfiguracyjnego zawiera nadpisujące
wartości:

W pliku konfiguracyjnym:

.. code-block:: javascript

  "log": {
    "level": "trace",
    "patterns": {
      "debug": "don't panic, it's %v.",
      "error": "MAMA MIA! %v!"
    },
    "children": {
      "KeysManager": {
        "level": "trace"
      },
      "Irohad": {
        "children": {
          "Storage": {
            "level": "trace",
            "patterns": {
              "debug": "thread %t: %v."
            }
          }
        }
      }
    }
  }

W zmiennych środowiskowych:

.. code-block:: bash

  IROHA_LOG_LEVEL=trace
  IROHA_LOG_PATTERNS_0_KEY=debug
  IROHA_LOG_PATTERNS_0="don't panic, it's %v."
  IROHA_LOG_PATTERNS_1_KEY=error
  IROHA_LOG_PATTERNS_1="MAMA MIA! %v!"
  IROHA_LOG_CHILDREN_0_KEY=KeysManager
  IROHA_LOG_CHILDREN_0_LEVEL=trace
  IROHA_LOG_CHILDREN_1_KEY=Irohad
  IROHA_LOG_CHILDREN_1_CHILDREN_0_KEY=Storage
  IROHA_LOG_CHILDREN_1_CHILDREN_0_LEVEL=trace
  IROHA_LOG_CHILDREN_1_CHILDREN_0_PATTERNS_0_KEY=debug
  IROHA_LOG_CHILDREN_1_CHILDREN_0_PATTERNS_0="thread %t: %v."

Każda część tej sekcji konfiguracji jest opcjonalna.

- ``level`` ustawia szczegółowość. Dostępne wartości (w
  malejącej kolejności szczegółowości):

  - ``trace`` - drukuj wszystko
  - ``debug``
  - ``info``
  - ``warning``
  - ``error``
  - ``critical`` - drukuj tylko krytyczne wiadomości

- ``patterns`` kontroluje formatowanie każdego ciągu
  logu dla różnych poziomów szczegółowości. Każda wartość
  nadpisuje również mniej szczegółowe poziomy. Więc w
  powyższym przykładzie wzorzec "don't panic" dotyczy
  również poziomów info i warning, a wzorzec poziomu
  trace jest jedynym, który nie jest zainicjalizowany w
  konfiguracji (zostanie ustawiony na domyślną
  hardkodowaną wartość).

.. note::  Nawet jeśli dla pojedynczego komponentu
   podano wiele wzorców, ten komponent użyje tylko
   jednego wzorca — tego, który odpowiada wybranemu
   poziomowi logowania. Jednak wzorce będą dziedziczone
   i mogą być używane w loggerach potomnych.

- ``children`` opisuje nadpisania węzłów potomnych.
  Klucze to nazwy komponentów, a wartości mają tę samą
  składnię i semantykę co konfiguracja logu korzenia.
