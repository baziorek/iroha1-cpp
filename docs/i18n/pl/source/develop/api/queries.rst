Zapytania
=========

Zapytanie to żądanie dotyczące określonej części World State View — najnowszego stanu blockchaina.
Zapytanie nie może modyfikować zawartości łańcucha, a odpowiedź jest zwracana
każdemu klientowi natychmiast po przetworzeniu zapytania przez peer.

Walidacja
^^^^^^^^^

Walidacja wszystkich zapytań obejmuje:

- znacznik czasu — nie powinien pochodzić z przeszłości (konfigurowalne w `konfiguracji Iroha <../../configure/index.html#environment-specific-parameters>`_) ani z przyszłości (zakres 5 minut dodany do czasu peera)
- podpis twórcy zapytania — używany do sprawdzenia tożsamości twórcy zapytania
- licznik zapytania — sprawdzany, aby był zwiększany przy każdym kolejnym zapytaniu od twórcy zapytania
- role — w zależności od roli twórcy zapytania: zakres stanu dostępny do zapytania może dotyczyć tego samego konta, konta w domenie, całego łańcucha lub w ogóle być niedozwolony

Paginacja wyników
^^^^^^^^^^^^^^^^^

Niektóre zapytania obsługują `TxPaginationMeta`, co pozwala dostosować i sortować wyniki zapytania na różne sposoby, co może być użyteczne w trakcie tworzenia oprogramowania.
Paginacja działa razem z parametrami porządkowania, podobnie do `ORDER BY w języku SQL <https://www.postgresql.org/docs/12/sql-select.html#SQL-ORDERBY>`_ — "wiersze wyniku są sortowane według określonego wyrażenia (w Iroha – pola Field). Jeśli dwa wiersze są równe według najbardziej po lewej strony wyrażenia, są porównywane według następnego wyrażenia i tak dalej."

Oto jak określa się „wyrażenie":

.. code-block:: proto

    enum Field {
        kCreatedTime = 0;
        kPosition = 1;
    }

Istnieją 2 podstawy porządkowania — według czasu utworzenia oraz w zależności od numeru bloku.

Dla każdego pola Field istnieje kierunek rosnący i malejący:

.. code-block:: proto

    enum Direction {
        kAscending = 0;
        kDescending = 1;
    }

Teraz samo porządkowanie:

.. code-block:: proto

    message Ordering {
        message FieldOrdering {
          Field field = 1;
          Direction direction = 2;
        }
        repeated FieldOrdering sequence = 1;
    }

Po określeniu porządkowania można wykonać paginację:

.. literalinclude:: ../../../../shared_model/schema/queries.proto
    :language: proto
    :start-at: message TxPaginationMeta {
    :end-before: message AssetPaginationMeta {


Co jest dodawane do struktury żądania w przypadku paginacji
------------------------------------------------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Rozmiar strony", "rozmiar strony, która ma zostać zwrócona przez zapytanie; jeśli odpowiedź zawiera mniej transakcji niż rozmiar strony, w odpowiedzi hash kolejnej transakcji będzie pusty", "page_size > 0", "5"
    "Hash pierwszej transakcji", "hash pierwszej transakcji na stronie. Jeśli to pole nie jest ustawione — zwracane są pierwsze transakcje", "hash w formacie hex", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
    "ordering", "w jaki sposób wyniki powinny być uporządkowane (przed zastosowaniem paginacji)", "zobacz pola poniżej", "zobacz pola poniżej"
    "ordering.sequence", "specyfikacja porządkowania, jak w SQL ORDER BY", "sekwencja pól i kierunków", "[{kCreatedTime, kAscending}, {kPosition, kDescending}]"
    "Czas pierwszej transakcji", "czas pierwszej transakcji w wyniku zapytania. Jeśli to pole nie jest ustawione — zwracane są transakcje zaczynające się od pierwszej", "typ Google Protocol Buffer Timestamp", "0001-01-01T00:00:00Z <= first tx time <= 9999-12-31T23:59:59.999999999Z"
    "Czas ostatniej transakcji", "czas ostatniej transakcji w wyniku zapytania. Jeśli to pole nie jest ustawione — zwracane są transakcje aż do ostatniej", "typ Google Protocol Buffer Timestamp", "0001-01-01T00:00:00Z <= last tx time <= 9999-12-31T23:59:59.999999999Z"
    "Wysokość pierwszej transakcji", "wysokość bloku pierwszej transakcji w wyniku zapytania. Jeśli to pole nie jest ustawione — zwracane są transakcje zaczynające się od wysokości 1", "first tx height > 0", "4"
    "Wysokość ostatniej transakcji", "wysokość bloku ostatniej transakcji w wyniku zapytania. Jeśli to pole nie jest ustawione — zwracane są transakcje aż do ostatniej", "last tx height > 0", "6"

Potwierdzenia silnika
^^^^^^^^^^^^^^^^^^^^^

Cel
---

Pobranie potwierdzenia (receipt) polecenia CallEngine.
Analogiczne do wywołania API eth.GetTransactionReceipt z Ethereum JSON RPC API.
Umożliwia dostęp do logu zdarzeń utworzonego podczas obliczeń wewnątrz EVM.

Schemat żądania
---------------

.. code-block:: proto

   message GetEngineReceipts {
       string tx_hash = 1;     // hex string
   }

Struktura żądania
-----------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Hash transakcji", "hash transakcji zawierającej polecenie CallEngine", "hash w formacie hex", "5241f70cf3adbc180199c1d2d02db82334137aede5f5ed35d649bbbc75ab2634"

Schemat odpowiedzi
------------------

.. code-block:: proto

    message EngineReceiptsResponse {
        repeated EngineReceipt engine_receipt = 1;
    }
    message EngineReceipt {
        int32 command_index = 1;
        string caller = 2;
        oneof opt_to_contract_address {
            CallResult call_result = 3;
            string contract_address = 4;
        }
        repeated EngineLog logs = 5;
    }
    message CallResult {
        string callee = 1;
        string result_data = 2;
    }
    message EngineLog {
        string address = 1;         // hex string
        string data = 2;            // hex string
        repeated string topics = 3; // hex string
    }

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "command_index", "Indeks polecenia CallEngine w transakcji", "liczba całkowita nieujemna", "0"
    "caller", "konto wywołujące smart contract", "<account_name>@<domain_id>", "admin@test"
    "call_result.callee", "adres wywoływanego kontraktu", "ciąg 20-bajtowy w reprezentacji hex", "0000000000000000000000000000000000000000"
    "call_result.result_data", "wartość zwrócona przez kontrakt", "ciąg w reprezentacji hex", "00"
    "contract_address", "adres EVM nowo wdrożonego kontraktu", "ciąg 20-bajtowy w reprezentacji hex", "7C370993FD90AF204FD582004E2E54E6A94F2651"
    "logs", "Tablica logów zdarzeń EVM utworzonych podczas wykonania smart contract.", "zobacz poniżej", "zobacz poniżej"
    "logs.[].address", "adres EVM wywołującego kontrakt", "ciąg 20-bajtowy w reprezentacji hex", "577266A3CE7DD267A4C14039416B725786605FF4"
    "logs.[].data", "zalogowane dane", "ciąg hex", "0000000000000000000000007203DF5D7B4F198848477D7F9EE080B207E544DD000000000000000000000000000000000000000000000000000000000000006D"
    "logs.[].topics", "temat logu jak w Ethereum", "ciągi 32-bajtowe", "[3990DB2D31862302A685E8086B5755072A6E2B5B780AF1EE81ECE35EE3CD3345, 000000000000000000000000969453762B0C739DD285B31635EFA00E24C25628]"


Możliwe błędy walidacji stanowych
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "2", "Brak takich uprawnień", "Twórca zapytania nie ma żadnych uprawnień do pobrania potwierdzenia wywołania silnika", "Przyznaj niezbędne uprawnienie"
    "3", "Nieprawidłowe podpisy", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"

Pobierz konto
^^^^^^^^^^^^^

Cel
---

Celem zapytania pobierzania konta jest uzyskanie stanu konta.

Schemat żądania
---------------

.. code-block:: proto

    message GetAccount {
        string account_id = 1;
    }

Struktura żądania
-----------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "id konta, którego stan ma zostać pobrany", "<account_name>@<domain_id>", "alex@morgan"

Schemat odpowiedzi
------------------

.. code-block:: proto

    message AccountResponse {
        Account account = 1;
        repeated string account_roles = 2;
    }

    message Account {
        string account_id = 1;
        string domain_id = 2;
        uint32 quorum = 3;
        string json_data = 4;
    }


Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "id konta", "<account_name>@<domain_id>", "alex@morgan"
    "ID domeny", "domena, w której konto zostało utworzone", "RFC1035 [#f1]_, RFC1123 [#f2]_ ", "morgan"
    "Quorum", "liczba sygnatariuszy potrzebna do podpisania transakcji, aby była ona ważna", "0 < quorum ≤ 128", "5"
    "Dane JSON", "informacje o koncie w postaci par klucz-wartość", "JSON", "{ genesis: {name: alex} }"

Możliwe błędy walidacji stanowych
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie udało się pobrać konta", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak takich uprawnień", "Twórca zapytania nie ma żadnych uprawnień do pobrania konta", "Przyznaj niezbędne uprawnienie: indywidualne, globalne lub domenowe"
    "3", "Nieprawidłowe podpisy", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"

Pobierz blok
^^^^^^^^^^^^

Cel
---

Celem zapytania pobierzania bloku jest uzyskanie konkretnego bloku, używając jego wysokości jako identyfikatora

Schemat żądania
---------------

.. code-block:: proto

    message GetBlock {
        uint64 height = 1;
    }


Struktura żądania
-----------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Wysokość", "wysokość bloku do pobrania", "0 < height < 2^64", "42"

Schemat odpowiedzi
------------------

.. code-block:: proto

    message BlockResponse {
        Block block = 1;
    }

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Block", "pobrany blok", "struktura bloku", "block"

Możliwe błędy walidacji stanowych
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie udało się pobrać bloku", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak takich uprawnień", "Twórca zapytania nie ma uprawnienia do pobrania bloku", "Przyznaj uprawnienie `can_get_block <permissions.html#can-get-blocks>`__"
    "3", "Nieprawidłowe podpisy", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"
    "3", "Nieprawidłowa wysokość", "Podana wysokość nie jest uint_64 lub jest większa niż wysokość ledgera", "Sprawdź wysokość i spróbuj ponownie"

.. note::
    Kod błędu 3 jest niejednoznaczny dla tego zapytania.
    Oznacza on albo nieprawidłowych sygnatariuszy, albo nieprawidłową wysokość.
    Użyj tej metody z `height = 1` (pierwszy blok zawsze istnieje), aby sprawdzić nieprawidłowych sygnatariuszy.

Pobierz sygnatariuszy
^^^^^^^^^^^^^^^^^^^^^

Cel
---

Celem zapytania pobierania sygnatariuszy jest uzyskanie sygnatariuszy, którzy pełnią rolę tożsamości konta.

Schemat żądania
---------------

.. code-block:: proto

    message GetSignatories {
        string account_id = 1;
    }

Struktura żądania
-----------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "id konta, dla którego mają zostać pobrani sygnatariusze", "<account_name>@<domain_id>", "alex@morgan"

Schemat odpowiedzi
------------------

.. code-block:: proto

    message SignatoriesResponse {
        repeated bytes keys = 1;
    }

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Keys", "tablica kluczy publicznych", "`ed25519 <https://ed25519.cr.yp.to>`_", "292a8714694095edce6be799398ed5d6244cd7be37eb813106b217d850d261f2"

Możliwe błędy walidacji stanowych
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie udało się pobrać sygnatariuszy", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak takich uprawnień", "Twórca zapytania nie ma żadnych uprawnień do pobrania sygnatariuszy", "Przyznaj niezbędne uprawnienie: indywidualne, globalne lub domenowe"
    "3", "Nieprawidłowe podpisy", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"

Pobierz transakcje
^^^^^^^^^^^^^^^^^^

Cel
---

GetTransactions służy do pobierania informacji o transakcjach na podstawie ich hashy.

.. note:: To zapytanie jest ważne wtedy i tylko wtedy, gdy wszystkie żądane hashe są poprawne: odpowiednie transakcje istnieją, a użytkownik ma uprawnienie do ich pobrania

Schemat żądania
---------------

.. code-block:: proto

    message GetTransactions {
        repeated bytes tx_hashes = 1;
    }

Struktura żądania
-----------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Hashy transakcji", "tablica hashy", "tablica z 32-bajtowymi hashami", "{hash1, hash2…}"

Schemat odpowiedzi
------------------

.. code-block:: proto

    message TransactionsResponse {
        repeated Transaction transactions = 1;
    }

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Transactions", "tablica transakcji", "Zatwierdzone transakcje", "{tx1, tx2…}"

Możliwe błędy walidacji stanowych
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie udało się pobrać transakcji", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak takich uprawnień", "Twórca zapytania nie ma żadnych uprawnień do pobrania transakcji", "Przyznaj niezbędne uprawnienie: indywidualne, globalne lub domenowe"
    "3", "Nieprawidłowe podpisy", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"
    "4", "Nieprawidłowy hash", "Co najmniej jeden z podanych hashy nie istnieje na liście transakcji użytkownika lub twórca zapytania nie ma uprawnień, aby go zobaczyć", "Sprawdź podane hashe i spróbuj ponownie"

Pobierz oczekujące transakcje
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cel
---

GetPendingTransactions służy do pobierania listy oczekujących (nie w pełni podpisanych) `transakcji wielopodpisowych <../../concepts_architecture/glossary.html#multisignature-transactions>`_
lub `partii transakcji <../../concepts_architecture/glossary.html#batch-of-transactions>`__ wyemitowanych przez konto twórcy zapytania.

.. note:: To zapytanie używa `paginacji <#result-pagination>`_ dla szybszych i wygodniejszych odpowiedzi na zapytania. Przeczytaj o niej i określ paginację przed wysłaniem żądania zapytania, a także `strukturę żądania <#what-is-added-to-the-request-structure-in-case-of-pagination>`_.

Schemat żądania
---------------

.. code-block:: proto

    message GetPendingTransactions {
        TxPaginationMeta pagination_meta = 1;
    }

Wszystkie transakcje wielopodpisowe (oczekujące) użytkownika, częściowo podpisane, mogą być poddane zapytaniu.
Maksymalna liczba transakcji zawartych w odpowiedzi może być ograniczona polem **page_size**.
Wszystkie oczekujące transakcje są przechowywane, dopóki nie zbiorą wystarczającej liczby podpisów lub nie wygaśną.
Wzajemna kolejność oczekujących transakcji lub partii transakcji jest zachowana dla danego użytkownika.
Pozwala to użytkownikowi na sekwencyjne odpytywanie wszystkich transakcji — strona po stronie.
Każda odpowiedź może zawierać odwołanie do następnej partii lub transakcji, która może zostać odpytywana.
Rozmiar strony może być większy niż rozmiar następującej partii (w transakcjach).
W takim przypadku zwrócone zostanie kilka partii lub transakcji.
Podczas nawigacji po stronach następna partia może zebrać brakujące podpisy przed odpytywaniem.
Spowoduje to nieudaną stanową odpowiedź zapytania z powodu brakującego hashy partii.

Przykład
--------

Jeśli istnieją dwie oczekujące partie po trzy transakcje każda, a użytkownik odpytuje oczekujące transakcje
z rozmiarem strony 5, w odpowiedzi znajdą się transakcje pierwszej partii, a także odwołanie
(hash pierwszej transakcji i rozmiar partii, nawet jeśli w rzeczywistości jest to pojedyncza transakcja) do drugiej partii
również zostanie określone.
Transakcje drugiej partii nie są uwzględnione w pierwszej odpowiedzi, ponieważ partii nie można podzielić
na kilka części i tylko kompletne partie mogą być zawarte w odpowiedzi.

Schemat odpowiedzi
------------------

.. code-block:: proto

    message PendingTransactionsPageResponse {
        message BatchInfo {
            string first_tx_hash = 1;
            uint32 batch_size = 2;
        }
        repeated Transaction transactions = 1;
        uint32 all_transactions_size = 2;
        BatchInfo next_batch_info = 3;
    }

Struktura odpowiedzi
--------------------

Odpowiedź zawiera listę `oczekujących transakcji <../../concepts_architecture/glossary.html#pending-transactions>`_,
liczbę wszystkich zapisanych oczekujących transakcji użytkownika
oraz informacje wymagane do odpytania kolejnej strony (jeśli istnieje).

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

        "Transactions", "tablica oczekujących transakcji", "Oczekujące transakcje", "{tx1, tx2…}"
        "All transactions size", "liczba zapisanych transakcji", "all_transactions_size >= 0", "0"
        "Next batch info", "Odwołanie do następnej strony — ta wiadomość może nie być ustawiona w odpowiedzi", "", ""
        "First tx hash", "hash pierwszej transakcji w następnej partii",  "hash w formacie hex", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
        "Batch size", "Minimalny rozmiar strony wymagany do pobrania następnej partii", "batch_size > 0", "3"

Pobierz oczekujące transakcje (przestarzałe)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. warning::
  Zapytanie bez parametrów jest teraz przestarzałe i zostanie usunięte w następnym większym wydaniu Iroha (2.0).
  Zamiast tego prosimy użyć nowej wersji zapytania: `Pobierz oczekujące transakcje <#get-pending-transactions>`__.

Cel
---

GetPendingTransactions służy do pobierania listy oczekujących (nie w pełni podpisanych) `transakcji wielopodpisowych <../../concepts_architecture/glossary.html#multisignature-transactions>`_
lub `partii transakcji <../../concepts_architecture/glossary.html#batch-of-transactions>`__ wyemitowanych przez konto twórcy zapytania.

Schemat żądania
---------------

.. code-block:: proto

    message GetPendingTransactions {
    }

Schemat odpowiedzi
------------------

.. code-block:: proto

    message TransactionsResponse {
        repeated Transaction transactions = 1;
    }

Struktura odpowiedzi
--------------------

Odpowiedź zawiera listę `oczekujących transakcji <../../concepts_architecture/glossary.html#pending-transactions>`_.

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

        "Transactions", "tablica oczekujących transakcji", "Oczekujące transakcje", "{tx1, tx2…}"

Możliwe błędy walidacji stanowych
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie udało się pobrać oczekujących transakcji", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak takich uprawnień", "Twórca zapytania nie ma żadnych uprawnień do pobrania oczekujących transakcji", "Przyznaj niezbędne uprawnienie: indywidualne, globalne lub domenowe"
    "3", "Nieprawidłowe podpisy", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"

Pobierz transakcje konta
^^^^^^^^^^^^^^^^^^^^^^^^

Cel
---

W przypadku, gdy potrzebna jest lista transakcji na konto, można utworzyć zapytanie `GetAccountTransactions`.

.. note:: To zapytanie używa `paginacji <#result-pagination>`_ dla szybszych i wygodniejszych odpowiedzi na zapytania. Przeczytaj o niej i określ paginację przed wysłaniem żądania zapytania, a także `strukturę żądania <#what-is-added-to-the-request-structure-in-case-of-pagination>`_.

Schemat żądania
---------------

.. code-block:: proto

    message GetAccountTransactions {
        string account_id = 1;
        TxPaginationMeta pagination_meta = 2;
    }

Struktura żądania
-----------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "id konta, z którego mają zostać pobrane transakcje", "<account_name>@<domain_id>", "makoto@soramitsu"

Schemat odpowiedzi
------------------

.. code-block:: proto

    message TransactionsPageResponse {
        repeated Transaction transactions = 1;
        uint32 all_transactions_size = 2;
        oneof next_page_tag {
            string next_tx_hash = 3;
        }
    }

Możliwe błędy walidacji stanowych
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie udało się pobrać transakcji konta", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak takich uprawnień", "Twórca zapytania nie ma żadnych uprawnień do pobrania transakcji konta", "Przyznaj niezbędne uprawnienie: indywidualne, globalne lub domenowe"
    "3", "Nieprawidłowe podpisy", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"
    "4", "Nieprawidłowy hash paginacji", "Podany hash nie występuje w żadnej z transakcji użytkownika", "Upewnij się, że hash jest poprawny, i spróbuj ponownie"
    "5", "Nieprawidłowe id konta", "Użytkownik z takim id konta nie istnieje", "Upewnij się, że id konta jest poprawne"

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Transactions", "tablica transakcji dla danego konta", "Zatwierdzone transakcje", "{tx1, tx2…}"
    "All transactions size", "całkowita liczba transakcji utworzonych przez dane konto", "", "100"
    "Next transaction hash", "hash wskazujący na następną transakcję po ostatniej transakcji na stronie. Pusty, jeśli strona zawiera ostatnią transakcję dla danego konta", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"

Pobierz transakcje aktywów konta
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cel
---

Zapytanie `GetAccountAssetTransactions` zwraca wszystkie transakcje powiązane z danym kontem i aktywem.

.. note:: To zapytanie używa `paginacji <#result-pagination>`_ dla szybszych i wygodniejszych odpowiedzi na zapytania. Przeczytaj o niej i określ paginację przed wysłaniem żądania zapytania, a także `strukturę żądania <#what-is-added-to-the-request-structure-in-case-of-pagination>`_.

Schemat żądania
---------------

.. code-block:: proto

    message GetAccountAssetTransactions {
        string account_id = 1;
        string asset_id = 2;
        TxPaginationMeta pagination_meta = 3;
    }

Struktura żądania
-----------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "id konta, z którego mają zostać pobrane transakcje", "<account_name>@<domain_id>", "makoto@soramitsu"
    "ID aktywa", "id aktywa w celu filtrowania transakcji zawierających to aktywo", "<asset_name>#<domain_id>", "jpy#japan"

Schemat odpowiedzi
------------------

.. code-block:: proto

    message TransactionsPageResponse {
        repeated Transaction transactions = 1;
        uint32 all_transactions_size = 2;
        oneof next_page_tag {
            string next_tx_hash = 3;
        }
    }

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Transactions", "tablica transakcji dla danego konta i aktywa", "Zatwierdzone transakcje", "{tx1, tx2…}"
    "All transactions size", "całkowita liczba transakcji dla danego konta i aktywa", "", "100"
    "Next transaction hash", "hash wskazujący na następną transakcję po ostatniej transakcji na stronie. Pusty, jeśli strona zawiera ostatnią transakcję dla danego konta i aktywa", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"

Możliwe błędy walidacji stanowych
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie udało się pobrać transakcji aktywów konta", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak takich uprawnień", "Twórca zapytania nie ma żadnych uprawnień do pobrania transakcji aktywów konta", "Przyznaj niezbędne uprawnienie: indywidualne, globalne lub domenowe"
    "3", "Nieprawidłowe podpisy", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"
    "4", "Nieprawidłowy hash paginacji", "Podany hash nie występuje w żadnej z transakcji użytkownika", "Upewnij się, że hash jest poprawny, i spróbuj ponownie"
    "5", "Nieprawidłowe id konta", "Użytkownik z takim id konta nie istnieje", "Upewnij się, że id konta jest poprawne"
    "6", "Nieprawidłowe id aktywa", "Aktywo z takim id aktywa nie istnieje", "Upewnij się, że id aktywa jest poprawne"

Pobierz aktywa konta
^^^^^^^^^^^^^^^^^^^^

Cel
---

Aby uzyskać stan wszystkich aktywów na koncie (saldo), można użyć zapytania `GetAccountAssets`.

Schemat żądania
---------------

.. code-block:: proto

    message AssetPaginationMeta {
        uint32 page_size = 1;
        oneof opt_first_asset_id {
            string first_asset_id = 2;
        }
    }

    message GetAccountAssets {
        string account_id = 1;
        AssetPaginationMeta pagination_meta = 2;
    }

Struktura żądania
-----------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "id konta, z którego ma zostać pobrane saldo", "<account_name>@<domain_id>", "makoto@soramitsu"
    AssetPaginationMeta.page_size, "Żądany rozmiar strony. Liczba aktywów w odpowiedzi nie przekroczy tej wartości. Jeśli odpowiedź została obcięta, id aktywa bezpośrednio następującego po zwróconych zostanie podane w next_asset_id.", 0 < page_size < 32 bit unsigned int max (4294967296), 100
    AssetPaginationMeta.first_asset_id, "Żądany początek strony. Jeśli pole nie jest ustawione, zwracana jest pierwsza strona.", name#domain, my_asset#my_domain

Schemat odpowiedzi
------------------
.. code-block:: proto

    message AccountAssetResponse {
        repeated AccountAsset account_assets = 1;
        uint32 total_number = 2;
        oneof opt_next_asset_id {
            string next_asset_id = 3;
        }
    }

    message AccountAsset {
        string asset_id = 1;
        string account_id = 2;
        string balance = 3;
    }

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Asset ID", "identyfikator aktywa używany do sprawdzenia salda", "<asset_name>#<domain_id>", "jpy#japan"
    "Account ID", "konto, które ma to saldo", "<account_name>@<domain_id>", "makoto@soramitsu"
    "Balance", "saldo aktywa", "Nie mniejsze niż 0", "200.20"
    total_number, liczba aktywów spełniających zapytanie bez limitów strony, 0 < total_number < 32 bit unsigned int max (4294967296), 100500
    next_asset_id, id aktywa bezpośrednio następującego po bieżącej stronie, name#domain, my_asset#my_domain

.. note::
   Jeśli rozmiar strony jest równy lub większy niż liczba aktywów spełniających pozostałe żądane kryteria, next asset id nie będzie ustawiony w odpowiedzi.
   W przeciwnym razie zawiera wartość, której klienci powinni użyć jako first asset id, jeśli chcą pobrać następną stronę.


Możliwe błędy walidacji stanowych
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie udało się pobrać aktywów konta", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak takich uprawnień", "Twórca zapytania nie ma żadnych uprawnień do pobrania aktywów konta", "Przyznaj niezbędne uprawnienie: indywidualne, globalne lub domenowe"
    "3", "Nieprawidłowe podpisy", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"
    "4", "Nieprawidłowe metadane paginacji", "Nieprawidłowy rozmiar strony lub nieistniejące pierwsze aktywo", "Ustaw prawidłowy rozmiar strony i upewnij się, że id aktywa jest prawidłowe, lub pozostaw first asset id nieokreślone"

Szczegóły konta
^^^^^^^^^^^^^^^

Cel
---

Aby uzyskać szczegóły konta, można użyć zapytania `GetAccountDetail`. Szczegóły konta to pary klucz-wartość, pogrupowane według kategorii writerów. Writerami są konta, które dodały odpowiednie szczegóły konta. Przykład takiej struktury:

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18,
            "hobbies": "crypto"
        },
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

Tutaj można zobaczyć cztery szczegóły konta - "age", "hobbies" i "sports" - dodane przez dwóch writerów - "account@a_domain" i "account@b_domain". Wszystkie te szczegóły dotyczą, oczywiście, tego samego konta.

Schemat żądania
---------------

.. code-block:: proto

    message AccountDetailRecordId {
        string writer = 1;
        string key = 2;
    }

    message AccountDetailPaginationMeta {
        uint32 page_size = 1;
        AccountDetailRecordId first_record_id = 2;
    }

    message GetAccountDetail {
        oneof opt_account_id {
          string account_id = 1;
        }
        oneof opt_key {
          string key = 2;
        }
        oneof opt_writer {
          string writer = 3;
        }
        AccountDetailPaginationMeta pagination_meta = 4;
    }

.. note::
    Zwróć uwagę, że wszystkie pola poza metadanymi stronicowania są opcjonalne.
    Powody takiego stanu rzeczy są opisane poniżej.

.. warning::
    Metadane stronicowania mogą nie występować w żądaniu ze względów zgodności, jednak takie zachowanie jest przestarzałe i powinno być unikane.

Struktura żądania
-----------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

        "ID konta", "id konta, z którego pobrać szczegóły", "<account_name>@<domain_id>", "account@domain"
        "Klucz", "klucz, pod którym pobrać szczegóły", "string", "age"
        "Writer", "id konta writera", "<account_name>@<domain_id>", "account@domain"
        AccountDetailPaginationMeta.page_size, "Żądany rozmiar strony. Liczba rekordów w odpowiedzi nie przekroczy tej wartości. Jeśli odpowiedź została skrócona, id rekordu bezpośrednio następującego po zwróconych zostanie podane w next_record_id.", 0 < page_size < 32 bit unsigned int max (4294967296), 100
        AccountDetailPaginationMeta.first_record_id.writer, żądany początek strony według writera, name#domain, my_asset#my_domain
        AccountDetailPaginationMeta.first_record_id.key, żądany początek strony według klucza, string, age

.. note::
    Podając id pierwszego rekordu wystarczy podać atrybuty (writer, key), które są nieustawione w głównym zapytaniu.

Schemat odpowiedzi
------------------

.. code-block:: proto

    message AccountDetailResponse {
        string detail = 1;
        uint64 total_number = 2;
        AccountDetailRecordId next_record_id = 3;
    }

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

        "Detail", "pary klucz-wartość ze szczegółami konta", "JSON", "zobacz poniżej"
        total_number, liczba rekordów pasujących do zapytania bez limitów strony, 0 < total_number < 32 bit unsigned int max (4294967296), 100
        next_record_id.writer, konto writera rekordu bezpośrednio następującego po bieżącej stronie, <account_name>@<domain_id>, pushkin@lyceum.tsar
        next_record_id.key, klucz rekordu bezpośrednio następującego po bieżącej stronie, string, "cold and sun"

Możliwe błędy walidacji stanowej
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not get account detail", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca zapytania nie ma żadnych uprawnień do uzyskania szczegółów konta", "Przyznaj niezbędne uprawnienie: indywidualne, globalne lub domenowe"
    "3", "Invalid signatures", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"
    "4", "Invalid pagination metadata", "Nieprawidłowy rozmiar strony lub nieistniejący pierwszy rekord", "Ustaw prawidłowy rozmiar strony i upewnij się, że id pierwszego rekordu jest prawidłowe, lub zostaw id pierwszego rekordu nieokreślone"

Przykłady użycia
----------------

Ponownie rozważmy przykład szczegółów z początku i zobaczmy, jak różne warianty zapytań `GetAccountDetail` zmienią wynikową odpowiedź.

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18,
            "hobbies": "crypto"
        },
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

**account_id nie jest ustawione**

Jeśli account_id nie jest ustawione - inne pola mogą być puste lub nie - zostanie automatycznie zastąpione kontem twórcy zapytania, co doprowadzi do jednego z kolejnych przypadków.

**ustawione jest tylko account_id**

W tym przypadku zostaną zwrócone wszystkie szczegóły tego konta, co da następującą odpowiedź:

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18,
            "hobbies": "crypto"
        },
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

**ustawione są account_id i key**

Tutaj zostaną zwrócone szczegóły dodane przez wszystkich writerów pod danym kluczem. Na przykład, jeśli poprosimy o klucz "age", otrzymamy taką odpowiedź:

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18
        },
        "account@b_domain": {
            "age": 20
        }
    }

**ustawione są account_id i writer**

Teraz odpowiedź zawiera wszystkie szczegóły tego konta dodane przez jednego konkretnego writera. Na przykład, jeśli poprosimy o writera "account@b_domain", otrzymamy:

.. code-block:: json

    {
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

**ustawione są account_id, key i writer**

Wreszcie, jeśli wszystkie trzy pola są ustawione, wynik zawiera szczegóły dodane przez konkretnego writera pod konkretnym kluczem. Na przykład, jeśli poprosimy o klucz "age" i writera "account@a_domain", otrzymamy:

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18
        }
    }

Pobieranie informacji o aktywie
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cel
---

Aby uzyskać informacje o danym aktywie (jak na razie - jego precyzję), użytkownik może wysłać zapytanie `GetAssetInfo`.

Schemat żądania
---------------

.. code-block:: proto

    message GetAssetInfo {
        string asset_id = 1;
    }

Struktura żądania
-----------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID aktywa", "id aktywa, którego dotyczy żądana informacja", "<asset_name>#<domain_id>", "jpy#japan"


Schemat odpowiedzi
------------------

.. code-block:: proto

    message Asset {
        string asset_id = 1;
        string domain_id = 2;
        uint32 precision = 3;
    }

.. note::
    Zwróć uwagę, że z powodu znanego problemu nie otrzymasz żadnego wyjątku, jeśli podasz nieprawidłową wartość precyzji.
    Prawidłowy zakres to: 0 <= precision <= 255

Możliwe błędy walidacji stanowej
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not get asset info", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca zapytania nie ma żadnych uprawnień do uzyskania informacji o aktywie", "Przyznaj niezbędne uprawnienie: indywidualne, globalne lub domenowe"
    "3", "Invalid signatures", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID aktywa", "identyfikator aktywa używany do sprawdzania salda", "<asset_name>#<domain_id>", "jpy#japan"
    "ID domeny", "domena powiązana z tym aktywem", "RFC1035 [#f1]_, RFC1123 [#f2]_", "japan"
    "Precyzja", "liczba cyfr po przecinku", "0 <= precision <= 255", "2"

Pobieranie ról
^^^^^^^^^^^^^^

Cel
---

Aby uzyskać istniejące role w systemie, użytkownik może wysłać zapytanie `GetRoles` do sieci Iroha.

Schemat żądania
---------------

.. code-block:: proto

    message GetRoles {
    }

Schemat odpowiedzi
------------------

.. code-block:: proto

    message RolesResponse {
        repeated string roles = 1;
    }

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Role", "tablica utworzonych ról w sieci", "zbiór ról w systemie", "{MoneyCreator, User, Admin, …}"

Możliwe błędy walidacji stanowej
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not get roles", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca zapytania nie ma żadnych uprawnień do uzyskania ról", "Przyznaj niezbędne uprawnienie: indywidualne, globalne lub domenowe"
    "3", "Invalid signatures", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"

Pobieranie uprawnień roli
^^^^^^^^^^^^^^^^^^^^^^^^^

Cel
---

Aby uzyskać dostępne uprawnienia per rola w systemie, użytkownik może wysłać zapytanie `GetRolePermissions` do sieci Iroha.

Schemat żądania
---------------

.. code-block:: proto

    message GetRolePermissions {
        string role_id = 1;
    }

Struktura żądania
-----------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID roli", "rola, dla której pobrać uprawnienia", "istniejąca rola w systemie", "MoneyCreator"

Schemat odpowiedzi
------------------

.. code-block:: proto

    message RolePermissionsResponse {
        repeated string permissions = 1;
    }

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Uprawnienia", "tablica uprawnień powiązanych z rolą", "ciąg uprawnień powiązanych z rolą", "{can_add_asset_qty, …}"

Możliwe błędy walidacji stanowej
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not get role permissions", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca zapytania nie ma żadnych uprawnień do uzyskania uprawnień roli", "Przyznaj niezbędne uprawnienie: indywidualne, globalne lub domenowe"
    "3", "Invalid signatures", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"

.. [#f1] https://www.ietf.org/rfc/rfc1035.txt
.. [#f2] https://www.ietf.org/rfc/rfc1123.txt


Pobieranie peerów
^^^^^^^^^^^^^^^^^

Cel
---

Zapytanie zwracające listę peerów w sieci Iroha.

Schemat żądania
---------------

.. code-block:: proto

    message GetPeers {
    }

Schemat odpowiedzi
------------------

.. code-block:: proto

    message Peer {
        string address = 1;
        string peer_key = 2; // hex string
    }

    message PeersResponse {
        repeated Peer peers = 1;
    }

Struktura odpowiedzi
--------------------

Zwracana jest lista peerów z ich adresami i kluczami publicznymi.

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Peerzy", "tablica peerów z sieci", "niepusta lista peerów", "{Peer{""peer.domain.com"", ""292a8714694095edce6be799398ed5d6244cd7be37eb813106b217d850d261f2""}, …}"

Możliwe błędy walidacji stanowej
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not get peers", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca zapytania nie ma wystarczających uprawnień do pobierania peerów", "Dodaj rolę z uprawnieniem can_get_blocks lub can_get_peers"
    "3", "Invalid signatures", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"

.. warning::

    Obecnie zapytanie Get Peers używa uprawnienia "can_get_blocks" ze względów zgodności.
    W późniejszej wersji zostanie to zmienione na "can_get_peers" przy kolejnym głównym wydaniu Irohy.

Pobieranie commitów
^^^^^^^^^^^^^^^^^^^

Cel
---

Aby uzyskać nowe bloki zaraz po ich zacommitowaniu, użytkownik może wywołać wywołanie RPC `FetchCommits` do sieci Iroha.

Schemat żądania
---------------

Nie są potrzebne żadne argumenty żądania


Schemat odpowiedzi
------------------

.. code-block:: proto

    message BlockQueryResponse {
        oneof response {
            BlockResponse block_response = 1;
            BlockErrorResponse block_error_response = 2;
        }
    }

    message BlockResponse {
        Block block = 1;
    }

    message BlockErrorResponse {
        string message = 1;
    }

Zwraca strumień `BlockQueryResponse`.

Struktura odpowiedzi
--------------------

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Blok", "blok Irohy", "tylko zacommitowane bloki", "{ 'block_v1': ....}"

Możliwe błędy walidacji stanowej
---------------------------------

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not get block streaming", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca zapytania nie ma żadnych uprawnień do pobierania bloków", "Przyznaj uprawnienie `can_get_block <permissions.html#can-get-blocks>`__"
    "3", "Invalid signatures", "Podpisy tego zapytania nie przeszły walidacji", "Dodaj więcej podpisów i upewnij się, że podpisy zapytania są podzbiorem sygnatariuszy konta"

.. note::
    `BlockErrorResponse` zawiera tylko pole `message`.
    W przypadku błędu walidacji stanowej będzie ono miało wartość "stateful invalid".
    `GetBlock <#get-block>`__ wymaga tego samego uprawnienia `can_get_block <permissions.html#can-get-blocks>`__.
    Dlatego można go użyć z `height = 1` (pierwszy blok jest zawsze obecny), aby sprawdzić nieprawidłowych sygnatariuszy lub brak uprawnień.

Przykład
--------
Możesz sprawdzić przykład użycia tego zapytania tutaj:
https://github.com/x3medima17/twitter
