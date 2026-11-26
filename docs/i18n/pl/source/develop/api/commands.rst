Komendy
=======

Komenda zmienia stan, zwany World State View, wykonując działanie na encji (aktywie, koncie) w systemie.
Każda komenda powinna być dołączona do transakcji w celu wykonania działania.

Kody błędów
-----------

Każda komenda może zakończyć się niepowodzeniem z numerycznym ``error_code`` zgłoszonym w statusie transakcji.
Kody ``0``, ``1`` i ``2`` mają to samo znaczenie dla wszystkich komend
(``0`` = powodzenie, ``1`` = błąd wewnętrzny, ``2`` = brak uprawnień);
kody ``>= 3`` są specyficzne dla komend.

Kanoniczne definicje znajdują się w ``shared_model/interfaces/command_error_codes.hpp``,
który jest współdzielony przez wykonawców komend (backendy Postgres i RocksDB)
oraz przez biblioteki klienckie. Poniższa tabela zawiera podsumowanie kodów
specyficznych dla poszczególnych komend, ułatwiające szybkie odniesienie się;
następnie odtwarzana jest pełna treść nagłówka. Jeśli tabela i
nagłówek kiedykolwiek się nie zgadzają, **nagłówek jest źródłem prawdy** — to
on zawiera kody, które wykonawcy faktycznie emitują.

.. list-table:: Kody błędów komend (``error_code`` w statusie transakcji)
   :header-rows: 1
   :widths: 30 10 60

   * - Komenda
     - Kod
     - Znaczenie
   * - *dowolna komenda*
     - 0
     - powodzenie
   * - *dowolna komenda*
     - 1
     - błąd wewnętrzny
   * - *dowolna komenda*
     - 2
     - brak uprawnień
   * - Dodaj ilość aktywa
     - 3
     - brak takiego aktywa (lub zbyt mała precyzja)
   * - Dodaj ilość aktywa
     - 4
     - przepełnienie wynikowej ilości
   * - Dodaj podpisującego
     - 3
     - brak takiego konta
   * - Dodaj podpisującego
     - 4
     - podpisujący już istnieje
   * - Dołącz rolę
     - 3
     - brak takiego konta
   * - Dołącz rolę
     - 4
     - brak takiej roli
   * - Wywołaj silnik
     - 3
     - wywołanie silnika kontraktu nie powiodło się
   * - Porównaj i ustaw szczegół konta
     - 3
     - brak takiego konta
   * - Porównaj i ustaw szczegół konta
     - 4
     - oczekiwana wartość nie pasuje
   * - Utwórz konto
     - 3
     - brak takiej domeny (brak roli domyślnej)
   * - Utwórz konto
     - 4
     - konto już istnieje
   * - Utwórz aktywo
     - 3
     - brak takiej domeny
   * - Utwórz aktywo
     - 4
     - aktywo już istnieje
   * - Utwórz domenę
     - 3
     - domena już istnieje
   * - Utwórz domenę
     - 4
     - brak roli domyślnej
   * - Utwórz rolę
     - 3
     - rola już istnieje
   * - Odłącz rolę
     - 3
     - brak takiego konta
   * - Odłącz rolę
     - 4
     - rola nie jest dołączona do konta
   * - Odłącz rolę
     - 5
     - rola nie istnieje
   * - Przyznaj uprawnienie
     - 3
     - brak takiego konta
   * - Usuń węzeł
     - 3
     - brak takiego węzła
   * - Usuń węzeł
     - 4
     - usunięcie spowodowałoby brak węzłów
   * - Usuń podpisującego
     - 3
     - brak takiego konta
   * - Usuń podpisującego
     - 4
     - klucz publiczny nie jest podpisującym
   * - Usuń podpisującego
     - 5
     - usunięcie naruszyłoby kworum
   * - Cofnij uprawnienie
     - 3
     - brak takiego konta
   * - Ustaw szczegół konta
     - 3
     - brak takiego konta
   * - Ustaw kworum konta
     - 3
     - brak takiego konta
   * - Ustaw kworum konta
     - 4
     - konto nie ma podpisujących
   * - Ustaw kworum konta
     - 5
     - kworum przekracza liczbę podpisujących
   * - Odejmij ilość aktywa
     - 3
     - brak takiego aktywa na koncie
   * - Odejmij ilość aktywa
     - 4
     - odejmowanie uczyniłoby ilość ujemną
   * - Przenieś aktywo
     - 3
     - brak takiego konta źródłowego
   * - Przenieś aktywo
     - 4
     - brak takiego konta docelowego
   * - Przenieś aktywo
     - 5
     - brak takiego aktywa
   * - Przenieś aktywo
     - 6
     - niewystarczające saldo
   * - Przenieś aktywo
     - 7
     - przepełnienie ilości
   * - Przenieś aktywo
     - 8
     - zbyt długi opis

Komendy nie wymienione powyżej (``AddPeer``, ``SetSettingValue``,
``AddSyncPeer``, ``RemoveSyncPeer``) emitują wyłącznie uniwersalne kody
``0``–``2``. Namespaces błędów ``AddSyncPeer``/``RemoveSyncPeer`` istnieją w
nagłówku dla funkcji sync peer, ale żadna komenda widoczna dla klienta
nie emituje ich jeszcze.

Kody ``1007`` i ``1008`` są emitowane wyłącznie przez wykonawcę komend
RocksDB (testowy backend): ``1007`` = uprawnienie już ustawione
(``grant_permission``), ``1008`` = pusty klucz publiczny (``remove_peer``).
Backend Postgres nie ma równoważnych kodów.

.. literalinclude:: ../../../../shared_model/interfaces/command_error_codes.hpp
   :language: cpp
   :caption: shared_model/interfaces/command_error_codes.hpp

Dodaj ilość aktywa
------------------------

Cel
^^^

Celem komendy dodania ilości aktywa jest zwiększenie ilości aktywa na koncie twórcy transakcji.
Przykładowym przypadkiem użycia jest zwiększenie liczby modyfikowalnego aktywa w systemie, które może służyć jako roszczenie wobec towaru (np. pieniędzy, złota itp.).

Schemat
^^^^^^^

.. code-block:: proto

    message AddAssetQuantity {
        string asset_id = 1;
        string amount = 2;
        optional string description = 3;
    }

.. note::
    Należy zauważyć, że z powodu znanego problemu nie otrzymasz żadnego wyjątku, jeśli przekażesz nieprawidłową wartość precyzji.
    Poprawny zakres to: 0 <= precision <= 255


Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID aktywa", "id aktywa", "<asset_name>#<domain_id>", "usd#morgan"
    "Ilość", "dodatnia ilość aktywa do dodania", "> 0", "200.02"
    "Opis", "opis transakcji", "Maksymalna długość opisu (ustawiana w genesis block, domyślnie 100*1024)", "Mint assets"

Walidacja
^^^^^^^^^

1. Aktywo i konto powinny istnieć
2. Precyzja dodawanej ilości powinna być równa precyzji aktywa
3. Twórca transakcji powinien mieć rolę z uprawnieniami do wydawania aktywów

Możeliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie możono dodać ilości aktywa", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak uprawnień", "Twórca komendy nie ma uprawnienia do dodania ilości aktywa", "Przyznaj niezbędne uprawnienie"
    "3", "Brak takiego aktywa", "Nie można znaleźć aktywa o takiej nazwie lub precyzji", "Upewnij się, że id aktywa i precyzja są poprawne"
    "4", "Przepełnienie sumy", "Wynikowa ilość aktywa jest większa, niż system może obsłużyć", "Upewnij się, że wynikowa ilość jest mniejsza niż 2^256 / 10^asset_precision"

Dodaj węzeł
-------------------

Cel
^^^

Celem komendy dodania węzła jest zapisanie w rejestrze faktu dodania węzła do sieci węzłów.
Po zatwierdzeniu transakcji z AddPeer komponenty konsensusu i synchronizacji zaczną go używać.
Możesz również `dowiedzieć się więcej o komendzie Add Peer <../../maintenance/add_peer.html>`_.

Schemat
^^^^^^^

.. literalinclude:: ../../../../shared_model/schema/primitive.proto
    :language: proto
    :start-at: message Peer {
    :end-before: message AccountDetailRecordId {

.. code-block:: proto

    message AddPeer {
        Peer peer = 1;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 10, 30

    "Adres", "rozwiązywalny adres w sieci (IPv4, IPv6, nazwa domenowa itp.)", "powinien być rozwiązywalny", "192.168.1.1:50541"
    "Klucz węzła", "publiczny klucz węzła, używany w algorytmie konsensusu do podpisywania głosów, wiadomości commit i reject", "publiczny klucz ed25519", "292a8714694095edce6be799398ed5d6244cd7be37eb813106b217d850d261f2"

Walidacja
^^^^^^^^^

1. Klucz węzła jest unikalny (nie ma innego węzła z takim kluczem publicznym)
2. Twórca transakcji ma rolę z uprawnieniem CanAddPeer
3. Taki adres sieciowy nie został już dodany

Możeliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie możono dodać węzła", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak uprawnień", "Twórca komendy nie ma uprawnienia do dodania węzła", "Przyznaj niezbędne uprawnienie"

Dodaj podpisującego
----------------------

Cel
^^^

Celem komendy dodania podpisującego jest dodanie identyfikatora do konta.
Takim identyfikatorem jest klucz publiczny innego urządzenia lub klucz publiczny innego użytkownika.

Schemat
^^^^^^^

.. code-block:: proto

    message AddSignatory {
        string account_id = 1;
        bytes public_key = 2;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "Konto, do którego ma zostać dodany podpisujący", "<account_name>@<domain_id>", "makoto@soramitsu"
    "Klucz publiczny", "Podpisujący do dodania do konta", "publiczny klucz ed25519", "359f925e4eeecfdd6aa1abc0b79a6a121a5dd63bb612b603247ea4f8ad160156"

Walidacja
^^^^^^^^^

Dwa przypadki:

    Przypadek 1. Twórca transakcji chce dodać podpisującego do własnego konta, mając uprawnienie CanAddSignatory

    Przypadek 2. Uprawnienie CanAddSignatory zostało przyznane twórcy transakcji

Możeliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie możono dodać podpisującego", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak uprawnień", "Twórca komendy nie ma uprawnienia do dodania podpisującego", "Przyznaj niezbędne uprawnienie"
    "3", "Brak takiego konta", "Nie można znaleźć konta, do którego ma zostać dodany podpisujący", "Upewnij się, że id konta jest poprawne"
    "4", "Podpisujący już istnieje", "Konto ma już takiego podpisującego dołączonego", "Wybierz innego podpisującego"

Dołącz rolę
--------------

Cel
^^^

Celem komendy dołączenia roli jest awansowanie konta do pewnej utworzonej roli w systemie, gdzie rola jest zbiorem uprawnień, które konto musi posiadać do wykonania działania (komendy lub zapytania).

Schemat
^^^^^^^

.. code-block:: proto

    message AppendRole {
       string account_id = 1;
       string role_name = 2;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "id konta, do którego ma zostać dołączona rola", "już istniejące", "makoto@soramitsu"
    "Nazwa roli", "nazwa już utworzonej roli", "już istniejące", "MoneyCreator"

Walidacja
^^^^^^^^^

1. Rola powinna istnieć w systemie
2. Twórca transakcji powinien mieć uprawnienia do dołączenia roli (CanAppendRole)
3. Konto, które dołącza rolę, ma w swoich rolach zbór uprawnień, który jest nadzbiorem dołączanej roli (innymi słowy nikt nie może dołączyć roli, która jest silniejsza niż ta twórcy transakcji)

Możeliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie możono dołączyć roli", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak uprawnień", "Twórca komendy nie ma uprawnienia do dołączenia roli", "Przyznaj niezbędne uprawnienie"
    "3", "Brak takiego konta", "Nie można znaleźć konta, do którego ma zostać dołączona rola", "Upewnij się, że id konta jest poprawne"
    "4", "Brak takiej roli", "Nie można znaleźć roli o takiej nazwie", "Upewnij się, że id roli jest poprawne"

Wywołaj silnik
-----------------

Cel
^^^

Celem komendy wywołania silnika jest wdrożenie nowego kontraktu w Iroha EVM lub wywołanie metody już istniejącego inteligentnego kontraktu.
Wykonanie inteligentnego kontraktu może potencjalnie zmodyfikować stan rejestru, pod warunkiem że transakcja zawierająca tę komendę zostanie zaakceptowana do bloku i blok zostanie zatwierdzony.

Schemat
^^^^^^^

.. code-block:: proto

    message CallEngine {
        string caller = 1;
        oneof opt_callee {
            string callee = 2;  // hex string
        }
        string input = 3;   // hex string
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"

    "Caller", "ID konta Iroha konta, w czyim imieniu uruchamiana jest komenda", "<account_name>@<domain_id>", "test@mydomain"
    "Callee", "adres EVM wdrożonego inteligentnego kontraktu", "ciąg 20 bajtów w reprezentacji szesnastkowej", "7C370993FD90AF204FD582004E2E54E6A94F2651"
    "Input", "Bytecode inteligentnego kontraktu dla nowo wdrażanych kontraktów lub zakodowany w ABI łańcuch selektora metody kontraktu, po którym następuje `zestaw jego argumentów <https://solidity.readthedocs.io/en/v0.6.5/abi-spec.html>`_", "Łańcuch szesnastkowy", "40c10f19000000000000000000000000969453762b0c739dd285b31635efa00e24c2562800000000000000000000000000000000000000000000000000000000000004d2"

Walidacja
^^^^^^^^^

1. Caller jest prawidłowym ID konta Iroha
2. Twórca transakcji ma rolę z uprawnieniem can_call_engine lub can_call_engine_on_my_behalf

Możeliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Silnik nie jest skonfigurowany", "Ten błąd oznacza, że Iroha została skompilowana bez Burrow EVM", "Zobacz sekcję `Build <../../build/index.html#main-parameters>`_ dokumentacji, aby poprawnie skompilować Irohę"
    "2", "Brak uprawnień", "Twórca komendy nie ma uprawnienia do wywołania silnika EVM", "Przyznaj niezbędne uprawnienie"
    "3", "Błąd CallEngine", "Wykonanie kodu w EVM nie powiodło się; przyczyną może być zarówno sam kod kontraktu, jak i zagnieżdżone wywołanie komend Iroha", "Wymagane jest zbadanie pierwotnej przyczyny błędu, aby zdiagnozować problem"

Utwórz konto
--------------

Cel
^^^

Celem komendy utworzenia konta jest utworzenie w systemie encji, która może wysyłać transakcje lub zapytania, przechowywać podpisujących, dane osobowe i identyfikatory.

Schemat
^^^^^^^

.. code-block:: proto

    message CreateAccount {
        string account_name = 1;
        string domain_id = 2;
        bytes public_key = 3;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Nazwa konta", "unikalna w domenie nazwa konta", "`[a-z_0-9]{1,32}`", "morgan_stanley"
    "ID domeny", "docelowa domena, z którą ma powstać relacja", "powinna zostać utworzona przed kontem", "america"
    "Klucz publiczny", "pierwszy klucz publiczny do dodania do konta", "publiczny klucz ed25519", "407e57f50ca48969b08ba948171bb2435e035d82cec417e18e4a38f5fb113f83"

Walidacja
^^^^^^^^^

1. Twórca transakcji ma uprawnienie do utworzenia konta
2. Domena przekazana jako domain_id została już utworzona w systemie
3. Taki klucz publiczny nie został dodany wcześniej jako pierwszy klucz publiczny konta ani dodany do konta multi-podpisowego

Możeliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie możono utworzyć konta", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak uprawnień", "Twórca komendy albo nie ma uprawnienia do utworzenia konta, albo próbuje utworzyć konto w domenie z większymi uprawnieniami niż ta, w której jest twórca", "Przyznaj niezbędne uprawnienie lub wybierz inną domenę"
    "3", "Brak takiej domeny", "Nie można znaleźć domeny o takiej nazwie", "Upewnij się, że id domeny jest poprawne"
    "4", "Konto już istnieje", "Konto o takiej nazwie już istnieje w tej domenie", "Wybierz inną nazwę"

Utwórz aktywo
--------------

Cel
^^^

Celem komendy utworzenia aktywa jest utworzenie nowego typu aktywa, unikalnego w domenie.
Aktywo jest policzalną reprezentacją towaru.

Schemat
^^^^^^^

.. code-block:: proto

    message CreateAsset {
        string asset_name = 1;
        string domain_id = 2;
        uint32 precision = 3;
    }

.. note::
    Należy zauważyć, że z powodu znanego problemu nie otrzymasz żadnego wyjątku, jeśli przekażesz nieprawidłową wartość precyzji.
    Poprawny zakres to: 0 <= precision <= 255

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Nazwa aktywa", "unikalna w domenie nazwa aktywa", "`[a-z_0-9]{1,32}`", "soracoin"
    "ID domeny", "docelowa domena, z którą ma powstać relacja", "RFC1035 [#f1]_, RFC1123 [#f2]_", "japan"
    "Precyzja", "liczba cyfr po przecinku/kropce", "0 <= precision <= 255", "2"

Walidacja
^^^^^^^^^

1. Twórca transakcji ma uprawnienie do tworzenia aktywów
2. Nazwa aktywa jest unikalna w domenie

Możeliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie możono utworzyć aktywa", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak uprawnień", "Twórca komendy nie ma uprawnienia do utworzenia aktywa", "Przyznaj niezbędne uprawnienie"
    "3", "Brak takiej domeny", "Nie można znaleźć domeny o takiej nazwie", "Upewnij się, że id domeny jest poprawne"
    "4", "Aktywo już istnieje", "Aktywo o takiej nazwie już istnieje", "Wybierz inną nazwę"

Utwórz domenę
--------------

Cel
^^^

Celem komendy utworzenia domeny jest utworzenie nowej domeny w sieci Iroha, która jest grupą kont.

Schemat
^^^^^^^

.. code-block:: proto

    message CreateDomain {
        string domain_id = 1;
        string default_role = 2;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID domeny", "ID utworzonej domeny", "unikalne, RFC1035 [#f1]_, RFC1123 [#f2]_", "japan05"
    "Rola domyślna", "rola dla każdego utworzonego użytkownika w domenie", "jedna z istniejących ról", "User"

Walidacja
^^^^^^^^^

1. ID domeny jest unikalne
2. Konto, które wysyła tę komendę w transakcji, ma rolę z uprawnieniem do tworzenia domeny
3. Rola, która będzie domyślnie przypisana utworzonemu użytkownikowi, istnieje w systemie

Możeliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie możono utworzyć domeny", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak uprawnień", "Twórca komendy nie ma uprawnienia do utworzenia domeny", "Przyznaj niezbędne uprawnienie"
    "3", "Domena już istnieje", "Domena o takiej nazwie już istnieje", "Wybierz inną nazwę domeny"
    "4", "Nie znaleziono roli domyślnej", "Nie znaleziono roli dostarczonej jako domyślna dla domeny", "Upewnij się, że podana rola istnieje, lub utwórz ją"

Utwórz rolę
--------------

Cel
^^^

Celem komendy utworzenia roli jest utworzenie nowej roli w systemie z zestawu uprawnień.
Łącząc różne uprawnienia w role, utrzymujący sieci węzłów Iroha mogą utworzyć dostosowany model bezpieczeństwa.

Schemat
^^^^^^^

.. code-block:: proto

    message CreateRole {
        string role_name = 1;
        repeated RolePermission permissions = 2;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Nazwa roli", "nazwa roli do utworzenia", "`[a-z_0-9]{1,32}`", "User"
    "RolePermission", "tablica już istniejących uprawnień", "zbiorowy zestaw przekazanych uprawnień jest w całości zawarty w zbiorze istniejących uprawnień", "{can_receive, can_transfer}"

Walidacja
^^^^^^^^^

1. Zbiorowy zestaw przekazanych uprawnień jest w całości zawarty w zbiorze istniejących uprawnień
2. Zestaw uprawnień nie jest pusty

Możeliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Nie możono utworzyć roli", "Wystąpił błąd wewnętrzny", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "Brak uprawnień", "Twórca komendy nie ma uprawnienia do utworzenia roli", "Przyznaj niezbędne uprawnienie"
    "3", "Rola już istnieje", "Rola o takiej nazwie już istnieje", "Wybierz inną nazwę roli"

Odłączanie roli
---------------

Cel
^^^

Celem komendy detach role (odłączania roli) jest odłączenie roli od zbioru ról konta.
Wykonując tę komendę, można zmniejszyć liczbę możliwych działań w systemie dla użytkownika.

Schemat
^^^^^^^

.. code-block:: proto

    message DetachRole {
        string account_id = 1;
        string role_name = 2;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "ID konta, z którego rola zostanie usunięta", "istniejące konto", "makoto@soramitsu"
    "Nazwa roli", "nazwa odłączanej roli", "istniejąca rola", "User"

Walidacja
^^^^^^^^^

1. Rola istnieje w systemie
2. Konto posiada taką rolę

Możliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not detach role", "Wystąpił wewnętrzny błąd", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca komendy nie ma uprawnień do odłączenia roli", "Przyznaj niezbędne uprawnienie"
    "3", "No such account", "Nie można znaleźć konta, z którego odłączyć rolę", "Upewnij się, że ID konta jest poprawne"
    "4", "No such role in account's roles", "Konto o takim ID nie ma roli o takiej nazwie", "Upewnij się, że para konto-rola jest poprawna"
    "5", "No such role", "Rola o takiej nazwie nie istnieje", "Upewnij się, że ID roli jest poprawne"

Przyznawanie uprawnień
----------------------

Cel
^^^

Celem komendy grant permission (przyznawania uprawnień) jest nadanie innemu kontu praw do wykonywania działań na koncie nadawcy transakcji (danie komuś prawa do robienia czegoś z moim kontem).

Schemat
^^^^^^^

.. code-block:: proto

    message GrantPermission {
        string account_id = 1;
        GrantablePermission permission = 2;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "ID konta, na rzecz którego przyznawane są prawa", "istniejące konto", "makoto@soramitsu"
    "Nazwa uprawnienia GrantablePermission", "nazwa uprawnienia grantable permission", "uprawnienie jest zdefiniowane", "CanTransferAssets"


Walidacja
^^^^^^^^^

1. Konto istnieje
2. Twórca transakcji ma prawo przyznania tego uprawnienia

Możliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not grant permission", "Wystąpił wewnętrzny błąd", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca komendy nie ma uprawnień do przyznania uprawnienia", "Przyznaj niezbędne uprawnienie"
    "3", "No such account", "Nie można znaleźć konta, na rzecz którego przyznać uprawnienie", "Upewnij się, że ID konta jest poprawne"

Usuwanie peera
---------------

Cel
^^^

Celem komendy remove peer (usuwania peera) jest zapisanie do rejestru (ledger) faktu usunięcia peera z sieci.
Po zatwierdzeniu transakcji z RemovePeer komponenty konsensusu i synchronizacji zaczną z niej korzystać.

Schemat
^^^^^^^

.. code-block:: proto

    message RemovePeer {
        bytes public_key = 1; // hex string
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 10, 30

    "Klucz publiczny", "klucz publiczny peera, używany w algorytmie konsensusu do podpisywania wiadomości głosowania", "klucz publiczny ed25519", "292a8714694095edce6be799398ed5d6244cd7be37eb813106b217d850d261f2"

Walidacja
^^^^^^^^^

1. W sieci znajduje się więcej niż jeden peer
2. Twórca transakcji ma rolę, która posiada uprawnienie CanRemovePeer
3. Peer powinien być wcześniej dodany do sieci

Możliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not remove peer", "Wystąpił wewnętrzny błąd", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca komendy nie ma uprawnień do usunięcia peera", "Przyznaj niezbędne uprawnienie"
    "3", "No such peer", "Nie można znaleźć peera o takim kluczu publicznym", "Upewnij się, że klucz publiczny jest poprawny"
    "4", "Network size does not allow to remove peer", "Po usunięciu peera sieć byłaby pusta", "Upewnij się, że sieć ma co najmniej dwóch peerów"

Usuwanie sygnatariusza
----------------------

Cel
^^^

Celem komendy remove signatory (usuwania sygnatariusza) jest usunięcie klucza publicznego powiązanego z tożsamością z konta.

Schemat
^^^^^^^

.. code-block:: proto

    message RemoveSignatory {
        string account_id = 1;
        bytes public_key = 2;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "ID konta, na rzecz którego przyznawane są prawa", "istniejące konto", "makoto@soramitsu"
    "Klucz publiczny", "Sygnatariusz do usunięcia", "klucz publiczny ed25519", "407e57f50ca48969b08ba948171bb2435e035d82cec417e18e4a38f5fb113f83"

Walidacja
^^^^^^^^^

1. Przy usuwaniu sygnatariusza należy sprawdzić, czy zachowany jest niezmiennik **size(signatories) >= quorum**
2. Sygnatariusz powinien być wcześniej dodany do konta

Dwa przypadki:

    Przypadek 1. Gdy twórca transakcji chce usunąć sygnatariusza ze swojego konta i ma uprawnienie CanRemoveSignatory

    Przypadek 2. Uprawnienie CanRemoveSignatory zostało przyznane twórcy transakcji

Możliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not remove signatory", "Wystąpił wewnętrzny błąd", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca komendy nie ma uprawnień do usunięcia sygnatariusza ze swojego konta", "Przyznaj niezbędne uprawnienie"
    "3", "No such account", "Nie można znaleźć konta, z którego usunąć sygnatariusza", "Upewnij się, że ID konta jest poprawne"
    "4", "No such signatory", "Nie można znaleźć sygnatariusza o takim kluczu publicznym", "Upewnij się, że klucz publiczny jest poprawny"
    "5", "Quorum does not allow to remove signatory", "Po usunięciu sygnatariusza konto zostałoby z mniejszą liczbą sygnatariuszy, niż pozwala na to jego quorum", "Zmniejsz quorum"

Wycofywanie uprawnień
---------------------

Cel
^^^

Celem komendy revoke permission (wycofywania uprawnień) jest odwołanie lub cofnięcie przyznanego uprawnienia innemu kontu w sieci.

Schemat
^^^^^^^

.. code-block:: proto

    message RevokePermission {
        string account_id = 1;
        GrantablePermission permission = 2;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: auto

        "ID konta", "ID konta, na rzecz którego przyznawane są prawa", "istniejące konto", "makoto@soramitsu"
        "Nazwa uprawnienia GrantablePermission", "nazwa uprawnienia grantable permission", "uprawnienie zostało przyznane", "CanTransferAssets"

Walidacja
^^^^^^^^^

Twórca transakcji powinien mieć wcześniej przyznane to uprawnienie docelowemu kontu.

Możliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not revoke permission", "Wystąpił wewnętrzny błąd", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca komendy nie ma uprawnień do wycofania uprawnienia", "Przyznaj niezbędne uprawnienie"
    "3", "No such account", "Nie można znaleźć konta, z którego wycofać uprawnienie", "Upewnij się, że ID konta jest poprawne"

Ustawianie szczegółów konta
---------------------------

Cel
^^^

Celem komendy set account detail (ustawiania szczegółów konta) jest ustawienie informacji klucz-wartość dla danego konta.

.. warning:: Jeśli dla danego klucza istniała już wartość w magazynie, zostanie ona zastąpiona nową wartością.

Schemat
^^^^^^^

.. code-block:: proto

    message SetAccountDetail{
        string account_id = 1;
        string key = 2;
        string value = 3;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "ID konta, na którym ustawiono informację klucz-wartość", "istniejące konto", "makoto@soramitsu"
    "Klucz", "klucz ustawianej informacji", "`[A-Za-z0-9_]{1,64}`", "Name"
    "Wartość", "wartość odpowiadającego klucza", "≤ 4096", "Makoto"

Walidacja
^^^^^^^^^

Trzy przypadki:

    Przypadek 1. Gdy twórca transakcji chce ustawić szczegół konta na koncie innej osoby, a twórca ma uprawnienie `can_set_detail <../api/permissions.html#can-set-detail>`_.

    Przypadek 2. Twórca transakcji otrzymał uprawnienie `can_set_my_account_detail <../api/permissions.html#can-set-my-account-detail>`_, które pozwala mu ustawiać szczegóły konta docelowego.

    Przypadek 3. Gdy posiadacz konta chce ustawić własne szczegóły konta – w tym przypadku nie jest potrzebne żadne uprawnienie.

Możliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not set account detail", "Wystąpił wewnętrzny błąd", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca komendy nie ma uprawnień do ustawienia szczegółów konta dla innego konta", "Przyznaj niezbędne uprawnienie"
    "3", "No such account", "Nie można znaleźć konta, na którym ustawić szczegóły konta", "Upewnij się, że ID konta jest poprawne"

Ustawianie quorum konta
-----------------------

Cel
^^^

Celem komendy set account quorum (ustawiania quorum konta) jest ustawienie liczby sygnatariuszy wymaganych do potwierdzenia tożsamości użytkownika tworzącego transakcję.
Przykładowy scenariusz użycia to ustawienie liczby różnych użytkowników korzystających z jednego konta do zatwierdzania transakcji.

Schemat
^^^^^^^

.. code-block:: proto

    message SetAccountQuorum {
        string account_id = 1;
        uint32 quorum = 2;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "ID konta, dla którego ustawiane jest quorum", "istniejące konto", "makoto@soramitsu"
    "Quorum", "liczba sygnatariuszy, którzy muszą być dołączeni do transakcji z tego konta", "0 < quorum ≤ zestaw kluczy publicznych przypisanych do konta ≤ 128", "5"

Walidacja
^^^^^^^^^

Przy ustawianiu quorum sprawdzane jest, czy zachowany jest niezmiennik **size(signatories) >= quorum**.

Dwa przypadki:

    Przypadek 1. Gdy twórca transakcji chce ustawić quorum dla swojego konta i ma uprawnienie CanRemoveSignatory

    Przypadek 2. Uprawnienie CanRemoveSignatory zostało przyznane twórcy transakcji

Możliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not set quorum", "Wystąpił wewnętrzny błąd", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca komendy nie ma uprawnień do ustawienia quorum dla swojego konta", "Przyznaj niezbędne uprawnienie"
    "3", "No such account", "Nie można znaleźć konta, dla którego ustawić quorum", "Upewnij się, że ID konta jest poprawne"
    "4", "No signatories on account", "Nie można znaleźć żadnych sygnatariuszy powiązanych z kontem", "Dodaj sygnatariuszy przed ustawieniem quorum"
    "5", "New quorum is incorrect", "Nowy rozmiar quorum jest mniejszy niż liczba sygnatariuszy konta", "Wybierz inną wartość lub dodaj więcej sygnatariuszy"

Odejmowanie ilości aktywów
--------------------------

Cel
^^^

Celem komendy subtract asset quantity (odejmowania ilości aktywów) jest odwrotność komendy AddAssetQuantity — zmniejszenie liczby aktywów na koncie twórcy transakcji.

Schemat
^^^^^^^

.. code-block:: proto

    message SubtractAssetQuantity {
        string asset_id = 1;
        string amount = 2;
        optional string description = 3;
    }

.. note::
    Zwróć uwagę, że z powodu znanego problemu nie otrzymasz żadnego wyjątku, jeśli podasz nieprawidłową wartość precyzji.
    Poprawny zakres to: 0 <= precision <= 255

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID aktywa", "ID aktywa", "<asset_name>#<domain_id>", "usd#morgan"
    "Ilość", "dodatnia ilość aktywa do odjęcia", "> 0", "200"
    "Opis", "opis transakcji", "Maksymalna długość opisu (ustawiana w bloku genesis, domyślnie 100*1024)", "Burn assets"

Walidacja
^^^^^^^^^

1. Aktywo i konto powinny istnieć
2. Precyzja dodanej ilości powinna być równa precyzji aktywa
3. Twórca transakcji powinien mieć rolę z uprawnieniami do odejmowania aktywów

Możliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not subtract asset quantity", "Wystąpił wewnętrzny błąd", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca komendy nie ma uprawnień do odejmowania ilości aktywów", "Przyznaj niezbędne uprawnienie"
    "3", "No such asset found", "Nie można znaleźć aktywa o takiej nazwie lub precyzji w aktywach konta", "Upewnij się, że nazwa aktywa i precyzja są poprawne"
    "4", "Not enough balance", "Saldo konta jest zbyt niskie, aby wykonać operację", "Dodaj aktywo do konta lub wybierz mniejszą wartość do odjęcia"

Transfer aktywów
----------------

Cel
^^^

Celem komendy transfer asset (transferu aktywów) jest współdzielenie aktywów w sieci peerów: w ten sposób, że konto źródłowe transferuje aktywa na konto docelowe.

Schemat
^^^^^^^

.. code-block:: proto

    message TransferAsset {
        string src_account_id = 1;
        string dest_account_id = 2;
        string asset_id = 3;
        string description = 4;
        string amount = 5;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta źródłowego", "ID konta, z którego aktywo ma zostać pobrane", "istniejące konto", "makoto@soramitsu"
    "ID konta docelowego", "ID konta, na które mają zostać wysłane aktywa", "istniejące konto", "alex@california"
    "ID aktywa", "ID aktywa do transferu", "istniejące konto", "usd#usa"
    "Opis", "Wiadomość do dołączenia do transferu", "Maksymalna długość opisu (ustawiana w bloku genesis, domyślnie 100*1024)", "here's my money take it"
    "Ilość", "ilość aktywa do transferu", "0 <= precision <= 255", "200.20"

Walidacja
^^^^^^^^^

1. Konto źródłowe posiada ten aktywo w relacji AccountHasAsset [#f1]_
2. Ilość jest liczbą dodatnią, a precyzja aktywa jest zgodna z definicją aktywa
3. Konto źródłowe ma wystarczającą ilość aktywa do transferu i nie jest zerowa
4. Konto źródłowe może transferować pieniądze, a konto docelowe może je otrzymywać (ich role mają te uprawnienia)
5. Długość opisu jest mniejsza niż 100*1024 (sto kilobajtów) i mniejsza niż wartość ustawienia 'MaxDescriptionSize', jeśli jest ustawione.

Możliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not transfer asset", "Wystąpił wewnętrzny błąd", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca komendy nie ma uprawnień do transferu aktywów ze swojego konta", "Przyznaj niezbędne uprawnienie"
    "3", "No such source account", "Nie można znaleźć konta o takim ID, z którego transferować pieniądze", "Upewnij się, że ID konta źródłowego jest poprawne"
    "4", "No such destination account", "Nie można znaleźć konta o takim ID, na które transferować pieniądze", "Upewnij się, że ID konta docelowego jest poprawne"
    "5", "No such asset found", "Nie można znaleźć takiego aktywa", "Upewnij się, że nazwa aktywa i precyzja są poprawne"
    "6", "Not enough balance", "Saldo konta źródłowego jest zbyt niskie, aby wykonać operację", "Dodaj aktywo do konta lub wybierz mniejszą wartość do transferu"
    "7", "Too much asset to transfer", "Wynikowa ilość aktywa konta docelowego przekroczyłaby dozwolone maksimum", "Upewnij się, że ostateczna wartość docelowa jest mniejsza niż 2^256 / 10^asset_precision"
    "8", "Too long description", "Zbyt długi opis", "Upewnij się, że długość opisu spełnia powyższe kryteria (lub po prostu skróć go)"

.. [#f1] https://www.ietf.org/rfc/rfc1035.txt
.. [#f2] https://www.ietf.org/rfc/rfc1123.txt

Porównaj i ustaw szczegół konta
-------------------------------

Cel
^^^

Celem komendy compare and set account detail (porównaj i ustaw szczegół konta) jest ustawienie informacji klucz-wartość dla danego konta, jeśli stara wartość zgadza się z przekazaną wartością.

Schemat
^^^^^^^

.. code-block:: proto

    message CompareAndSetAccountDetail{
        string account_id = 1;
        string key = 2;
        string value = 3;
        oneof opt_old_value {
            string old_value = 4;
        }
        bool check_empty = 5;
    }

.. note::
    Zwróć uwagę, że pole old_value jest opcjonalne.
    Jest tak, ponieważ para klucz-wartość może nie istnieć.

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "ID konta", "ID konta, na którym ustawiono informację klucz-wartość. Jeśli para klucz-wartość nie istnieje, zostanie utworzona", "istniejące konto", "artyom@soramitsu"
    "Klucz", "klucz ustawianej informacji", "`[A-Za-z0-9_]{1,64}`", "Name"
    "Wartość", "nowa wartość dla odpowiadającego klucza", "długość wartości ≤ 4096", "Artyom"
    "Stara wartość", "bieżąca wartość dla odpowiadającego klucza", "długość wartości ≤ 4096", "Artem"
    "check_empty", "jeśli true, puste old_value w komendzie musi pasować do nieistniejącej wartości w WSV; jeśli false, dowolne old_value w komendzie pasuje do nieistniejącej wartości w WSV (legacy)", "bool", "true"

Walidacja
^^^^^^^^^

Trzy przypadki:

    Przypadek 1. Gdy twórca transakcji chce ustawić szczegół konta na swoim koncie i ma uprawnienia GetMyAccountDetail / GetAllAccountsDetail / GetDomainAccountDetail

    Przypadek 2. Gdy twórca transakcji chce ustawić szczegół konta na koncie innej osoby i ma uprawnienia SetAccountDetail oraz GetAllAccountsDetail / GetDomainAccountDetail

    Przypadek 3. Uprawnienie SetAccountDetail zostało przyznane twórcy transakcji i ma on uprawnienie GetAllAccountsDetail / GetDomainAccountDetail

Możliwe błędy walidacji stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Nazwa błędu", "Opis", "Jak rozwiązać"

    "1", "Could not compare and set account detail", "Wystąpił wewnętrzny błąd", "Spróbuj ponownie lub skontaktuj się z deweloperami"
    "2", "No such permissions", "Twórca komendy nie ma uprawnień do ustawiania i odczytywania szczegółów konta dla tego konta", "Przyznaj niezbędne uprawnienie"
    "3", "No such account", "Nie można znaleźć konta, na którym ustawić szczegóły konta", "Upewnij się, że ID konta jest poprawne"
    "4", "No match values", "Stare wartości nie pasują", "Upewnij się, że stara wartość jest poprawna"

Ustawianie wartości ustawienia
------------------------------

Cel
^^^

Celem komendy set setting value (ustawiania wartości ustawienia) jest umożliwienie dostosowania do Twoich potrzeb.


Schemat
^^^^^^^

.. code-block:: proto

    message SetSettingValue {
        string key = 1;
        string value = 2;
    }

Struktura
^^^^^^^^^

.. csv-table::
    :header: "Pole", "Opis", "Ograniczenie", "Przykład"
    :widths: 15, 30, 20, 15

    "Klucz", "Klucz ustawienia", "lista możliwych ustawień", "MaxDescriptionSize"
    "Wartość", "Wartość ustawienia", "typ ustawienia", "255"


Walidacja
^^^^^^^^^

1. Komenda może być wykonana wyłącznie z bloku genesis

Lista możliwych ustawień
^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Klucz", "Ograniczenie wartości", "Opis"

    "MaxDescriptionSize", "Liczba całkowita bez znaku, 0 <= MaxDescriptionSize < 2^32", "Maksymalna długość opisu transakcji"
