HL Burrow EVM Kullanım Örnekleri
================================

Bu bölüm, Iroha blok zincirinin üzerinde bir EVM'de
akıllı sözleşmelerin nasıl dağıtılıp
çalıştırılabileceğine dair birkaç örnek
gösterir.

Iroha ile etkileşim için `Python Iroha
istemcisini
<https://iroha.readthedocs.io/en/main/getting_started/python-guide.html>`_
kullanacağız. Iroha düğümünün yerel 50051
portunda dinlediğini varsayarsak, istemci kodu
şöyle görünecektir:

.. code-block:: python

	import os
	from iroha import Iroha, IrohaCrypto, IrohaGrpc

	iroha = Iroha('admin@test')
	net = IrohaGrpc('127.0.0.1:50051')

	admin_key = os.getenv(ADMIN_PRIVATE_KEY, IrohaCrypto.private_key())
	# Code for preparing and sending transaction

Durum 1. Hesaplama çalıştırma ve veri depolama
----------------------------------------------

İlk örnek olarak Solidity belgelerindeki `Subcurrency
<https://solidity.readthedocs.io/en/latest/introduction-to-smart-contracts.html#subcurrency-example>`_
akıllı sözleşmesini alacağız. Sözleşme kodu
aşağıdadır (okuyucu gerekirse her satırın ne
anlama geldiğini anlamak için orijinal belgelere
başvurabilir):

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

Başlamak için yukarıdaki kaynak kodu bytecode'a
derlememiz gerekir. Bunun için tam teşekküllü
Solidity derleyicisini veya web tabanlı `Remix
IDE <https://remix.ethereum.org>`_'yi
kullanabiliriz.

Bytecode'u aldıktan sonra, sözleşmeyi EVM'e
dağıtacak bir işlemi Python Iroha istemcisinden
gönderebiliriz:

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


Bu sözleşmenin mint metodunu çağırmak için aynı
*CallEngine* komutunu, input parametresi fonksiyon
imzasının *keccak256* hash'inin ilk 4 baytı olan
metod seçicisini içeren şekilde göndeririz:

``keccak256(‘mint(address,uint256)’) == ‘40c10f19’``

fonksiyon argümanları sözleşme ABI kurallarına göre
kodlanmış olarak birleştirilir — ilk fonksiyon
argümanı *address* türündedir, yani 20 bayt uzunluğunda
tams sayı.

Sözleşme sahibinin (*admin@test* Iroha hesabı)
1000 coin basıp bunları kendisine atamak istediğini
varsayalım. Python kütüphanesini kullanarak *admin@test*
ile karşılık gelen EVM adresini almak için şunu
kullanabiliriz:

.. code-block:: python

	import sha3
	k = sha3.keccak_256()
	k.update(b'admin@test')
	print(hexlify(k.digest()[12:32]).zfill(64))

Bu şekilde şunu elde ederiz:

``000000000000000000000000f205c4a929072dd6e7fc081c2a78dbc79c76070b``

Yani son 20 bayt keccak256'dır, sola sıfırla 32 bayta
doldurulmuştur.


*amount* argümanı, onaltılık olarak kodlanmış bir
*uint256* sayısıdır (yine sola doldurulmuş):

``00000000000000000000000000000000000000000000000000000000000003e8``

Tüm argüman dizesi, yukarıdaki üç parçanın birleştirilmiş
birleşimidir.


Hepsini bir araya getirdiğimizde, *Coin* sözleşmesinin
*mint* fonksiyonunu çağırmak için şu istemci kodunu
elde ederiz:

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

*send* fonksiyonunu çağırmak tam olarak aynı şekilde
yapılır.

Daha önce açıklandığı gibi log'a kaydedilen bir Sent
olayı yayınlayan send fonksiyonunun son satırına
dikkat edin:

.. code-block:: solidity

	emit Sent(msg.sender, receiver, amount);


Durum 2. Iroha durumuyla etkileşim
-----------------------------------

Daha önce Iroha durumu ile etkileşime girmeyen bir
sözleşmenin örneğine baktık. Ancak Iroha blok
zincirinin üzerinde çalışabilecek çoğu gerçek hayat
uygulamasında (işlem işlemeyle özel iş mantığı veya
işlem ücretleri alma gibi) Iroha durumu ile
etkileşim kurabilmek vazgeçilmezdir. Bu bölümde bir
EVM akıllı sözleşmesinin içinden Iroha hesap
bakiyelerinin nasıl sorgulanabileceğine dair bir
örnek ele alacağız (sorgu oluşturucunun ilgili izinlere
sahip olması koşuluyla).

Bunu yapmak için örnek bir sözleşme kodu:

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

Yapıcıda, Iroha durumuyla etkileşim için birden fazla
API açığa çıkaran `ServiceContract
<burrow.html#running-native-iroha-commands-in-evm>`_'ın
EVM adresini başlatıyoruz. Bu API'ler hem Iroha
durumunu sorgulamak hem de değiştirmek için
kullanılabilir. Çoğu Iroha komutu ve sorgusu
entegre edilmiştir. Bu sözleşme Iroha *ServiceContract*
API'sinin *getAssetBalance*, *createAccount*, *addAsset*
ve *transferAsset* metotlarını çağırır.

Bytecode'u almak için yukarıdaki sözleşmeyi tam teşekküllü
bir Solidity derleyicisi veya web tabanlı *Remix
IDE* kullanarak derlememiz gerekir. Şimdi, sözleşmeyi
EVM'e dağıtmak ve ayrıca sözleşmenin farklı
fonksiyonlarını çağırmak için Python Iroha
istemcisinden işlemler gönderebiliriz. Sözleşme
yukarıda gösterildiği gibi benzer bir şekilde
dağıtılır. Dağıtılmış bir sözleşmenin fonksiyonunu
çağırmak için fonksiyon imzası ve argümanları
`ABI spesifikasyonuna
<https://solidity.readthedocs.io/en/v0.6.5/abi-spec.html>`_
göre kodlanmalıdır.

Dağıtılmış bir sözleşmenin fonksiyonunu çağıran örnek
bir Python kodu:

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

String türündeki, dinamik türdeki bir argüman şu
şekilde kodlanır: Öncelikle argümanların bloğunun
başından itibaren bayt cinsinden ölçülen argümanın
konum kısmını sağlarız, bu daha sonra sola doğru 32
bayta doldurulur. Argümanın veri kısmı, bayt dizisinin
element cinsinden uzunluğuyla başlar, bu da sola
doğru 32 bayta doldurulur. Ardından dizinin UTF-8
kodlaması, sağa doğru 32 bayta doldurulur. Bu,
örnekteki fonksiyonların yardımıyla
gerçekleştirilebilir.

Daha fazla örnek ve kodun nasıl çalıştığı için
`tutarda  <https://github.com/baziorek/iroha1-cpp/tree/iroha1-main/example/burrow_integration>`_
ziyaret edebilirsiniz.
