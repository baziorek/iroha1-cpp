Przykłady użycia HL Burrow EVM
===============================

Ta sekcja demonstruje kilka przykładów, jak można
wdrażać i uruchamiać kontrakty inteligentne w EVM
na wierzchu łańcucha bloków Iroha.

Do interakcji z Irohą będziemy używać `klienta Python
Iroha <https://iroha.readthedocs.io/en/main/getting_started/python-guide.html>`_.
Zakładając, że węzeł Iroha nasłuchuje na lokalnym
porcie 50051, kod klienta będzie wyglądać mniej więcej
tak:

.. code-block:: python

	import os
	from iroha import Iroha, IrohaCrypto, IrohaGrpc

	iroha = Iroha('admin@test')
	net = IrohaGrpc('127.0.0.1:50051')

	admin_key = os.getenv(ADMIN_PRIVATE_KEY, IrohaCrypto.private_key())
	# Code for preparing and sending transaction

Przypadek 1. Uruchamianie obliczeń i przechowywanie
danych
-----------------

Jako pierwszy przykład weźmiemy kontrakt
inteligentny `Subcurrency
<https://solidity.readthedocs.io/en/latest/introduction-to-smart-contracts.html#subcurrency-example>`_
z dokumentacji Solidity. Kod kontraktu jest następujący
(czytelnik może w razie potrzeby odwołać się do
oryginalnej dokumentacji, aby zrozumieć, co oznacza
each linia kodu kontraktu):

.. code-block:: solidity

    contract Coin {
        // The keyword "public" makes variables
        // accessible from other contracts
        address public minter;
        mapping (address => uint) public balances;

        // Events allow clients to react to specific
        // contract changes you declare
        event Sent(address from, address to, uint amount);

        // Constructor code is only run when the contract
        // is created
        constructor() public {
            minter = msg.sender;
        }

        // Sends an amount of newly created coins to an address
        // Can only be called by the contract creator
        function mint(address receiver, uint amount) public {
            require(msg.sender == minter);
            require(amount < 1e60);
            balances[receiver] += amount;
        }

        // Sends an amount of existing coins
        // from any caller to an address
        function send(address receiver, uint amount) public {
            require(amount <= balances[msg.sender], "Insufficient balance.");
            balances[msg.sender] -= amount;
            balances[receiver] += amount;
            emit Sent(msg.sender, receiver, amount);
        }
    }

Na początek musimy skompilować powyższy kod źródłowy do
bytecode. W tym celu możemy użyć pełnoprawnego
kompilatora Solidity lub `Remix IDE opartego na
sieci <https://remix.ethereum.org>`_.

Mając bytecode, możemy teraz wysłać transakcję z
klienta Python Iroha, która wdroży kontrakt do EVM:

.. code-block:: python

	import os
	from iroha import Iroha, IrohaCrypto, IrohaGrpc

	iroha = Iroha('admin@test')
	net = IrohaGrpc('127.0.0.1:50051')

	admin_key = os.getenv(ADMIN_PRIVATE_KEY, IrohaCrypto.private_key())
	bytecode = ("608060405234801561001057600080fd5b50336000806101000a81548173ffff”
	            "ffffffffffffffffffffffffffffffffffff021916908373ffffffffffffffff"
	            ...
	            "030033")

	tx = iroha.transaction([
	    iroha.command('CallEngine', caller='admin@test', input=bytecode)
	])
	IrohaCrypto.sign_transaction(tx, admin_key)

	net.send_tx(tx)
	for status in net.tx_status_stream(tx):
	    print(status)


Aby wywołać metodę mint tego kontraktu, wysyłamy to
samo polecenie *CallEngine* z parametrem input
zawierającym selektor metody — pierwsze 4 bajty hasha
*keccak256* sygnatury funkcji:

``keccak256(‘mint(address,uint256)’) == ‘40c10f19’``

połączony z argumentami funkcji zakodowanymi zgodnie
z regułami ABI kontraktu – pierwszy argument funkcji
ma typ *address*, czyli 20-bajtową liczbę całkowitą.

Powiedzmy, że właściciel kontraktu (konto Iroha
*admin@test*) chce wybić 1000 monet i przypisać je
sobie. Aby uzyskać adres EVM odpowiadający
*admin@test* za pomocą biblioteki Python, możemy
użyć:

.. code-block:: python

	import sha3
	k = sha3.keccak_256()
	k.update(b'admin@test')
	print(hexlify(k.digest()[12:32]).zfill(64))

W ten sposób otrzymamy:

``000000000000000000000000f205c4a929072dd6e7fc081c2a78dbc79c76070b``

Zatem ostatnie 20 bajtów to keccak256, dopełnione
zerami z lewej do 32 bajtów.


Argument *amount* to liczba *uint256* zakodowana w
formacie szesnastkowym (również dopełniona z lewej):

``00000000000000000000000000000000000000000000000000000000000003e8``

Cały ciąg argumentów to konkatenacja trzech powyższych
fragmentów połączonych razem.


Zestawiając to wszystko, otrzymamy następujący kod
klienta do wywołania funkcji *mint* kontraktu *Coin*:

.. code-block:: python

	import os
	from iroha import Iroha, IrohaCrypto, IrohaGrpc

	iroha = Iroha('admin@test')
	net = IrohaGrpc('127.0.0.1:50051')

	admin_key = os.getenv(ADMIN_PRIVATE_KEY, IrohaCrypto.private_key())
	params = ("40c10f19”                                                          # selector
	          "000000000000000000000000f205c4a929072dd6e7fc081c2a78dbc79c76070b"  # address
	          "00000000000000000000000000000000000000000000000000000000000003e8"  # amount
	         )

	tx = iroha.transaction([
	    iroha.command('CallEngine', callee='ServiceContract', input=params)
	])
	IrohaCrypto.sign_transaction(tx, admin_key)

	net.send_tx(tx)
	for status in net.tx_status_stream(tx):
	    print(status)

Wywołanie funkcji *send* odbywa się dokładnie tak samo.

Zwróć uwagę na ostatnią linię funkcji send, która
emituje zdarzenie Sent, które jest zapisywane w logu
jak opisano wcześniej:

.. code-block:: solidity

	emit Sent(msg.sender, receiver, amount);


Przypadek 2. Interakcja ze stanem Irohy
---------------------------------------

Wcześniej przyjrzeliśmy się przykładowi kontraktu,
który nie wchodził w interakcję ze stanem Irohy.
Jednak w większości rzeczywistych aplikacji, które
można sobie wyobrazić działające na wierzchu łańcucha
bloków Iroha (takich jak niestandardowa logika
biznesowa w przetwarzaniu transakcji lub pobieranie
opłat transakcyjnych itp.), możliwość interakcji ze
stanem Irohy jest niezastąpiona. W tej sekcji
rozważymy przykład, jak można zapytać o salda kont
Iroha (pod warunkiem, że twórca zapytania ma
odpowiednie uprawnienia) z wnętrza kontraktu
inteligentnego EVM.

Oto przykładowy kod kontaktu, aby to zrobić:

.. code-block:: solidity

	contract Iroha {
		address public serviceContractAddress;

		event Created(string indexed name, string indexed domain);
		event Transferred(string indexed source, string indexed destination, string amount);
		event Added(string indexed asset, string amount);


		// Initializing service contract address in constructor
		constructor(){
			serviceContractAddress = 0xA6Abc17819738299B3B2c1CE46d55c74f04E290C;
		}

		// Creates an iroha ccount
		function createAccount(string memory name, string memory domain, string memory key) public  returns (bytes memory result) {
			bytes memory payload = abi.encodeWithSignature(
				"createAccount(string,string,string)",
				name,
				domain,
				key);
			(bool success, bytes memory ret) = address(serviceContractAddress).delegatecall(payload);
			require(success, "Error calling service contract function");
			emit Created(name, domain);
			result = ret;
		}

		//Transfers asset from one iroha account to another
		function transferAsset(string memory src, string memory dst, string memory asset, string memory description, string memory amount) public returns (bytes memory result) {
			bytes memory payload = abi.encodeWithSignature(
				"transferAsset(string,string,string,string,string)",
				src,
				dst,
				asset,
				description,
				amount);
			(bool success, bytes memory ret) = address(serviceContractAddress).delegatecall(payload);
			require(success, "Error calling service contract function");

			emit Transferred(src, dst, amount);
			result = ret;
		}
		// Adds asset to iroha account
		function addAsset(string memory asset, string memory amount) public returns (bytes memory result) {
			bytes memory payload = abi.encodeWithSignature(
				"addAsset(string,string)",
				asset,
				amount);
			(bool success, bytes memory ret) = address(serviceContractAddress).delegatecall(payload);
			require(success, "Error calling service contract function");

			emit Added(asset, amount);
			result = ret;
		}
		//Queries balance of an iroha account
		function queryBalance(string memory _account, string memory _asset) public returns (bytes memory result) {
			bytes memory payload = abi.encodeWithSignature(
				"getAssetBalance(string,string)",
				_account,
				_asset);
			(bool success, bytes memory ret) = address(serviceContractAddress).delegatecall(payload);
			require(success,"Error calling service contract function ");
			result = ret;
		}
	}

W konstruktorze inicjalizujemy adres EVM
`ServiceContract <burrow.html#running-native-iroha-commands-in-evm>`_, który
udostępnia wiele API do interakcji ze stanem Irohy.
Te API mogą być używane zarówno do zapytań, jak i
modyfikacji stanu Irohy. Większość poleceń i
zapytań Irohy została zintegrowana. Ten kontrakt
wywołuje metody *getAssetBalance*, *createAccount*,
*addAsset* i *transferAsset* API *ServiceContract*
Irohy.

Musimy skompilować powyższy kontrakt, aby uzyskać
bytecode, używając pełnoprawnego kompilatora Solidity
lub *Remix IDE* opartego na sieci. Teraz możemy
wysyłać transakcje z klienta Python Iroha, aby
wdrożyć kontrakt do EVM i wywołać różne funkcje
kontraktu. Kontrakt jest wdrażany w podobny sposób,
jak pokazano powyżej. Aby wywołać funkcję wdrożonego
kontraktu, sygnaturę funkcji i jej argumenty należy
zakodować zgodnie ze
`specyfikacją ABI <https://solidity.readthedocs.io/en/v0.6.5/abi-spec.html>`_.

Oto przykładowy kod Python, który wywołuje funkcję
wdrożonego kontraktu:

.. code-block:: python

	def add_asset(address):
		params = get_first_four_bytes_of_keccak(
			b"addAsset(string,string)"
		)
		no_of_param = 2
		for x in range(no_of_param):
			params = params + left_padded_address_of_param(
				x, no_of_param
			)
		params = params + argument_encoding("coin#test")  # asset id
		params = params + argument_encoding("500")  # amount of asset
		tx = iroha.transaction(
			[
				iroha.command("CallEngine", caller=ADMIN_ACCOUNT_ID, callee=address, input=params)
			]
		)
		IrohaCrypto.sign_transaction(tx, ADMIN_PRIVATE_KEY)
		response = net.send_tx(tx)
		for status in net.tx_status_stream(tx):
			print(status)

	def make_number_hex_left_padded(number: str, width: int = 64):
		number_hex = "{:x}".format(number)
		return str(number_hex).zfill(width)


	def left_padded_address_of_param(param_index: int, number_of_params: int, width: int = 64):
		"""Specifies the position of each argument according to Contract ABI specifications."""
		bits_offset = 32 * number_of_params
		bits_per_param = 64
		bits_for_the_param = bits_offset + bits_per_param * param_index
		return make_number_hex_left_padded(bits_for_the_param, width)


	def argument_encoding(arg):
		"""Encodes the argument according to Contract ABI specifications."""
		encoded_argument = str(hex(len(arg)))[2:].zfill(64)
		encoded_argument = (
			encoded_argument + arg.encode("utf8").hex().ljust(64, "0").upper()
		)
		return encoded_argument


	def get_first_four_bytes_of_keccak(function_signature: str):
		"""Generates the first 4 bytes of the keccak256 hash of the function signature. """
		k = keccak.new(digest_bits=256)
		k.update(function_signature)
		return k.hexdigest()[:8]

Argument typu string, typu dynamicznego, jest kodowany w
następujący sposób: Najpierw podajemy część lokalizacji
argumentu mierzoną w bajtach od początku bloku
argumentów, która jest następnie dopełniana z lewej do
32 bajtów. Część danych argumentu zaczyna się od
długości tablicy bajtów w elementach, również
dopełnianej z lewej do 32 bajtów. Następnie kodowanie
UTF-8 ciągu, dopełniane po prawej do 32 bajtów. Można
to osiągnąć za pomocą funkcji w przykładzie.

Więcej przykładów i wyjaśnienie działania kodu znajdziesz
`tutaj <https://github.com/baziorek/iroha1-cpp/tree/iroha1-main/example/burrow_integration>`_.
