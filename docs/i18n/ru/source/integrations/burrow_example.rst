Примеры использования HL Burrow EVM
====================================

В этом разделе приведено несколько примеров того, как можно развёртывать и запускать смарт-контракты в EVM поверх блокчейна Iroha.

Для взаимодействия с Iroha мы будем использовать `Python-клиент Iroha <https://iroha.readthedocs.io/en/main/getting_started/python-guide.html>`_. Предполагая, что нода Iroha слушает локальный порт 50051, код клиента будет выглядеть примерно так:

.. code-block:: python

	import os
	from iroha import Iroha, IrohaCrypto, IrohaGrpc

	iroha = Iroha('admin@test')
	net = IrohaGrpc('127.0.0.1:50051')

	admin_key = os.getenv(ADMIN_PRIVATE_KEY, IrohaCrypto.private_key())
	# Code for preparing and sending transaction

Пример 1. Выполнение вычислений и хранение данных
-------------------------------------------------

В качестве первого примера мы возьмём смарт-контракт `Subcurrency <https://solidity.readthedocs.io/en/latest/introduction-to-smart-contracts.html#subcurrency-example>`_ из документации Solidity.
Код контракта приведён ниже (при необходимости читатель может обратиться к оригинальной документации, чтобы понять, что означает каждая строка кода контракта):

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

Для начала нам нужно скомпилировать приведённый выше исходный код в байткод.
Для этого можно использовать либо полнофункциональный компилятор Solidity, либо веб-инструмент `Remix IDE <https://remix.ethereum.org>`_.

Получив байткод, теперь мы можем отправить транзакцию из Python-клиента Iroha, которая развернёт контракт в EVM:

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


Чтобы вызвать метод *mint* этого контракта, мы отправляем ту же команду *CallEngine*, где параметр *input* содержит селектор метода – первые 4 байта *keccak256*-хэша сигнатуры функции:

``keccak256(‘mint(address,uint256)’) == ‘40c10f19’``

конкатенированный с аргументами функции, закодированными согласно правилам ABI контракта – первый аргумент функции имеет тип *address*, то есть целое число длиной 20 байт.

Допустим, владелец контракта (аккаунт Iroha *admin@test*) хочет отчеканить 1000 монет и назначить их себе.
Чтобы получить EVM-адрес, соответствующий *admin@test*, используя Python-библиотеку, можно сделать так:

.. code-block:: python

	import sha3
	k = sha3.keccak_256()
	k.update(b'admin@test')
	print(hexlify(k.digest()[12:32]).zfill(64))

В результате получим:

``000000000000000000000000f205c4a929072dd6e7fc081c2a78dbc79c76070b``

То есть последние 20 байт — это keccak256, дополненный нулями слева до 32 байт.


Аргумент *amount* — это число *uint256*, закодированное в hex (также дополненное нулями слева):

``00000000000000000000000000000000000000000000000000000000000003e8``

Вся строка аргументов — это конкатенация трёх приведённых выше частей.


Собрав всё вместе, мы получим следующий код клиента для вызова функции *mint* контракта *Coin*:

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

Вызов функции *send* выполняется точно так же.

Обратите внимание на последнюю строку функции send, которая эмитирует событие Sent, записываемое в журнал, как описано выше:

.. code-block:: solidity

	emit Sent(msg.sender, receiver, amount);


Пример 2. Взаимодействие с состоянием Iroha
-------------------------------------------

Ранее мы рассмотрели пример контракта, который не взаимодействовал с состоянием Iroha.
Однако в большинстве реальных приложений, которые можно представить работающими поверх блокчейна Iroha (например, пользовательская бизнес-логика при обработке транзакций или взимание комиссий за транзакции и т.п.), возможность взаимодействовать с состоянием Iroha незаменима.
В этом разделе мы рассмотрим пример того, как можно запрашивать балансы аккаунтов Iroha (при условии, что у создателя запроса есть соответствующие разрешения) изнутри смарт-контракта EVM.


Вот пример кода контракта для этого:

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

В конструкторе мы инициализируем EVM-адрес `ServiceContract <burrow.html#running-native-iroha-commands-in-evm>`_, который предоставляет множество API для взаимодействия с состоянием Iroha.
Эти API можно использовать как для запроса, так и для изменения состояния Iroha. Интегрировано большинство команд и запросов Iroha.
Этот контракт вызывает методы *getAssetBalance*, *createAccount*, *addAsset* и *transferAsset* API Iroha *ServiceContract*.

Нам нужно скомпилировать приведённый выше контракт, чтобы получить байткод, используя полнофункциональный компилятор Solidity или веб-инструмент *Remix IDE*.
Теперь мы можем отправлять транзакции из Python-клиента Iroha для развёртывания контракта в EVM, а также для вызова различных функций контракта.
Контракт развёртывается аналогично тому, как показано выше. Чтобы вызвать функцию развёрнутого контракта, сигнатура функции и её аргументы должны быть закодированы в соответствии с `ABI-спецификацией <https://solidity.readthedocs.io/en/v0.6.5/abi-spec.html>`_.

Вот пример Python-кода, вызывающего функцию развёрнутого контракта:

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

Аргумент типа string, являющийся динамическим типом, кодируется следующим образом:
Сначала мы указываем часть расположения аргумента, измеренную в байтах от начала блока аргументов, которая затем дополнена нулями слева до 32 байт. Часть данных аргумента начинается с длины байтового массива в элементах, также дополненной нулями слева до 32 байт. Затем UTF-8-кодировка строки, дополненная справа до 32 байт.
Этого можно достичь с помощью функций в примере.

Больше примеров и информация о том, как работает код, доступны `здесь  <https://github.com/baziorek/iroha1-cpp/tree/iroha1-main/example/burrow_integration>`_.
