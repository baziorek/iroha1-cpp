Integracja HL Burrow
====================

Jako opiekunowie Irohy otrzymaliśmy wiele pytań i próśb
o wsparcie niestandardowych smart-kontraktów od naszych
użytkowników. Aby zapewnić im większą swobodę w
realizowaniu ich potrzeb biznesowych, zintegrowaliśmy
HL Burrow EVM — kolejny wspaniały projekt z rodziny
Hyperledger — z Irohą.

.. note:: W kontekście Irohy HL Burrow dostarcza
   Ethereum Virtual Machine, które może uruchamiać
   kontrakty inteligentne Solidity. Dołożyliśmy
   wszelkich starań, aby zapewnić najlepsze możliwe
   doświadczenie użytkownika — a aby używać go z
   Irohą, wystarczy dodać `flagę CMake podczas
   kompilacji Irohy <../build/index.html#cmake-parameters>`_
   i zacznie działać od razu.

   Istnieją oficjalne obrazy dockera zbudowane z
   `Hyperledger Burrow <https://hyperledger.github.io/burrow/#/>`_.
   Zamień ``hyperledger/iroha:latest`` na
   ``hyperledger/iroha-burrow:latest``, aby ich używać.

Możesz przeczytać o języku kontraktów inteligentnych
Solidity `tutaj
<https://solidity.readthedocs.io/>`_, jeśli jesteś
nowy w tym języku.

Jak to działa
-------------

Dla tej integracji utworzyliśmy specjalne
`polecenie Call Engine
<../develop/api/commands.html#call-engine>`_ w Iroha
oraz specjalne `zapytanie Engine Receipts
<../develop/api/queries.html#engine-receipts>`_ do
pobierania wyników polecenia.

Polecenie
^^^^^^^^^

W poleceniu możesz:

**Utworzyć nowe konto kontraktu w EVM**

Jeśli *callee* w `CallEngine
<../develop/api/commands.html#call-engine>`_ nie jest
określony, a parametr *input* zawiera jakiś bytecode,
tworzone jest nowe konto kontraktu.

**Wywołać metodę wcześniej wdrożonego kontraktu**

Jeśli *callee* jest określony, input jest traktowany
jako zakodowany ABI selektor metody kontraktu callee,
po którym następują argumenty.

.. hint:: To bardzo podobne do wdrożenia kontraktu lub
   wywołania funkcji kontraktu w Ethereum, w
   zależności od zawartości pola `data` komunikatu
   wywołania `eth_sendTransaction
   <https://github.com/ethereum/wiki/wiki/JSON-RPC#eth_sendtransaction>`_.
   Szczegóły zobacz w `specyfikacji
   ABI <https://solidity.readthedocs.io/en/v0.6.5/abi-spec.html>`_.

Zapytanie
^^^^^^^^^

Aby zapytać o wynik polecenia `CallEngine
<../develop/api/commands.html#call-engine>`_, należy
użyć `zapytania Engine Receipts
<../develop/api/queries.html#engine-receipts>`_.
Wyjście jakichkolwiek obliczeń wewnątrz EVM nie będzie
dostępne dla wywołującego, dopóki nie zostanie
zapisane w ledgerze (tj. blok zawierający odpowiednią
transakcję Iroha zostanie zatwierdzony). Wśród innych
`danych <../develop/api/queries.html#response-structure>`_,
zapytanie *EngineReceipts* zwróci tablicę wpisów
logów wygenerowanych w EVM podczas wykonania
*CallEngine*.

.. hint:: Typowym sposobem dla deweloperów dApps,
   aby pozwolić zainteresowanym stronom zobaczyć
   wynik wykonania kontraktu, jest emisja zdarzenia z
   jakimiś danymi przed opuszczeniem funkcji kontraktu,
   tak aby te dane zostały zapisane w *Event Log*.
   `Żółty papier Ethereum
   <https://ethereum.github.io/yellowpaper/paper.pdf>`_
   definiuje wpis logu jako 3-krotkę zawierającą
   adres emiterów, tablicę 32-bajtowych tematów i
   tablicę bajtów pewnych danych.

Przekazywanie Ordering i argumentów, które są listą
wartości, odbywa się przez wysłanie ciągu JSON z
parametrami. Więcej przykładów zobacz w przykładach.

Uruchamianie natywnych poleceń Iroha w EVM
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Dzięki integracji HL Burrow możesz również używać
natywnych poleceń do zmiany stanu Irohy.

Mechanizm integracji Burrow EVM daje deweloperom
aplikacji Iroha narzędzie zdolne do bezpośredniego
działania na stanie Irohy z kodu kontraktów
inteligentnych, zapewniając tym samym fundament dla
programowalnych rozszerzeń logiki biznesowej
wbudowanego systemu poleceń Irohy. Warunkowe
transfery aktywów, opłaty transakcyjne, aktywa
niepodzielne itp. to tylko kilka przykładów takich
rozszerzeń. Trik polega na tym, że model danych
Irohy jest dość inny niż model Ethereum. Na przykład
w Ethereum istnieje tylko jeden rodzaj wbudowanego
aktywa (``Eth``), więc pobranie salda konta w kontekście
EVM oznacza po prostu zwrócenie właściwości salda
konta. W Iroha natomiast konto może mieć wiele aktywów
lub nie mieć ich wcale, więc każda funkcja zwracająca
saldo konta musi przyjąć co najmniej jeden dodatkowy
argument — ID aktywa. Ta sama logika dotyczy
transferowania/wysyłania aktywów z konta na konto.

Jako rozwiązanie tego problemu niedopasowania modeli
danych wprowadzamy tak zwane Service Contracts w
Burrow, które "znają" model danych Irohy i udostępniają
API do interakcji ze stanem Irohy (zapytania o salda,
transfery aktywów itp.).

.. note:: Możesz sprawdzić `dokumentację
   Burrow <https://wiki.hyperledger.org/display/burrow/Burrow+-+The+Boring+Blockchain>`_
   po więcej informacji o Natives i dispatcherach
   zewnętrznych.

Schematycznie interakcja między różnymi częściami
systemu wygląda następująco:

.. image:: ../../image_assets/burrow/natives.svg

.. attention::
   Z perspektywy Burrow EVM taki Service Contract jest
   hostowany w natywnym zewnętrznym VM i można go
   wywołać przez te same interfejsy, jakby został
   wdrożony pod specjalnym adresem w samym EVM.
   Te metody są używane konkretnie dla integracji
   Irohy, więc adres Service Contract można znaleźć
   tylko podczas pracy z nim przez Irohę.

Bieżące wydanie opakowania Iroha EVM zawiera pojedynczy
kontrakt usługi wdrożony pod adresem
``A6ABC17819738299B3B2C1CE46D55C74F04E290C`` (ostatnie
20 bajtów hasha *keccak256* ciągu *ServiceContract*),
który udostępnia następujące polecenia i zapytania
Irohy:

.. list-table:: Zintegrowane polecenia/zapytania
   :widths: 3 35 35
   :header-rows: 1

   * - Lp.
     - Sygnatura udostępnionych metod API
     - Argumenty
   * - 1
     - `addAssetQuantity(string,string) <../develop/api/commands.html#add-asset-quantity>`_
     - asset_id, amount
   * - 2
     - `addPeer(string,string) <../develop/api/commands.html#add-peer>`_
     - address, peer_key
   * - 3
     - `addSignatory(string,string) <../develop/api/commands.html#add-signatory>`_
     - account_id, public_key
   * - 4
     - `appendRole(string,string) <../develop/api/commands.html#append-role>`_
     - account_id, role_name
   * - 5
     - `createAccount(string,string,string) <../develop/api/commands.html#create-account>`_
     - account_name, domain_id, public_key
   * - 6
     - `createAsset(string,string,string) <../develop/api/commands.html#create-asset>`_
     - asset_name, domain_id, precision
   * - 7
     - `createDomain(string,string) <../develop/api/commands.html#create-domain>`_
     - domain_id, default_role
   * - 8
     - `detachRole(string,string) <../develop/api/commands.html#detach-role>`_
     - account_id, role_name
   * - 9
     - `removePeer(string) <../develop/api/commands.html#remove-peer>`_
     - public_key
   * - 10
     - `removeSignatory(string,string) <../develop/api/commands.html#remove-signatory>`_
     - account_id, public_key
   * - 11
     - `setAccountDetail(string,string,string) <../develop/api/commands.html#set-account-detail>`_
     - account_id, key, value
   * - 12
     - `setAccountQuorum(string,string) <../develop/api/commands.html#set-account-quorum>`_
     - account_id, quorum
   * - 13
     - `subtractAssetQuantity(string,string) <../develop/api/commands.html#subtract-asset-quantity>`_
     - asset_id, amount
   * - 14
     - `transferAsset(string,string,string,string,string) <../develop/api/commands.html#transfer-asset>`_
     - src_account_id, dest_account_id, asset_id, description, amount
   * - 15
     - `getAccount(string) <../develop/api/queries.html#get-account>`_
     - account_id
   * - 16
     - `getBlock(string) <../develop/api/queries.html#get-block>`_
     - height
   * - 17
     - `getSignatories(string) <../develop/api/queries.html#get-signatories>`_
     - account_id
   * - 18
     - `getAssetBalance(string,string) <../develop/api/queries.html#get-account-assets>`_
     - account_id, asset_id
   * - 19
     - `getAccountDetail() <../develop/api/queries.html#get-account-detail>`_
     -
   * - 20
     - `getAssetInfo(string) <../develop/api/queries.html#get-asset-info>`_
     - asset_id
   * - 21
     - `getRoles() <../develop/api/queries.html#get-roles>`_
     -
   * - 22
     - `getRolePermissions(string) <../develop/api/queries.html#get-role-permissions>`_
     - role_id
   * - 23
     - `getPeers() <../develop/api/queries.html#get-peers>`_
     -


.. hint:: Z perspektywy dewelopera wywołanie funkcji
  natywnego kontraktu nie różni się od wywołania
  metody dowolnego innego kontraktu inteligentnego,
  pod warunkiem że znany jest adres latter:

	bytes memory payload = abi.encodeWithSignature("getAssetBalance(string,string)", "myacc@test", "coin#test");

	(bool success, bytes memory ret) = address(0xA6ABC17819738299B3B2C1CE46D55C74F04E290C).delegatecall(payload);

Tutaj używany jest specjalny rodzaj wywołań
wiadomości EVM — **delegatecall**, który pozwala
kontraktowi dynamicznie ładować i uruchamiać kod z
innego adresu w czasie wykonywania we własnym
kontekście wykonania.

.. seealso:: Teraz przejdźmy do przykładów
  `użycia <burrow_example.html>`_
