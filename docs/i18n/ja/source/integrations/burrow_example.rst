HL Burrow EVM の使用例
====================================

このセクションでは、Iroha ブロックチェーンの上で EVM を使ってスマートコントラクトをデプロイし、実行する方法のいくつかの例を示します。

Iroha とのやり取りには `Python Iroha クライアント <https://iroha.readthedocs.io/en/main/getting_started/python-guide.html>`_ を使用します。Iroha ノードがローカルポート 50051 で待ち受けていると仮定すると、クライアントコードは次のようなものになります。

このページの例を通じて、トランザクションの組み立て、署名、送信、そして結果の確認までを Python クライアントだけで行う流れを順を追って学ぶことができます。各ステップで使用するコマンドは Iroha の CallEngine であり、スマートコントラクトのデプロイや実行はすべてこのコマンドを通じて行われます。

.. code-block:: python

	import os
	from iroha import Iroha, IrohaCrypto, IrohaGrpc

	iroha = Iroha('admin@test')
	net = IrohaGrpc('127.0.0.1:50051')

	admin_key = os.getenv(ADMIN_PRIVATE_KEY, IrohaCrypto.private_key())
	# Code for preparing and sending transaction

ケース 1：計算の実行とデータの保存
---------------------------------------------

最初の例として、Solidity ドキュメントにある `Subcurrency <https://solidity.readthedocs.io/en/latest/introduction-to-smart-contracts.html#subcurrency-example>`_ スマートコントラクトを取り上げます。
コントラクトのコードは次のとおりです（各行が何を意味するかについては、必要に応じて元のドキュメントを参照してください）。

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

まず最初に、上記のソースコードをバイトコードにコンパイルする必要があります。
そのためには、本格的な Solidity コンパイラを使用することも、Web ベースの `Remix IDE <https://remix.ethereum.org>`_ を使用することもできます。
コンパイル結果として得られるのは、EVM が直接実行できるバイトコードの 16 進数文字列です。以降の手順では、このバイトコードを CallEngine コマンドの input パラメータに渡すことで、コントラクトを EVM 内に展開します。

バイトコードを入手したら、Python の Iroha クライアントからトランザクションを送信して、コントラクトを EVM にデプロイできます。
ここで重要なのは、*caller* パラメータにトランザクションを送信するアカウント ID を、*input* パラメータにコンパイル済みバイトコードを指定するという点です。callee を指定しないことで、Iroha は新しいコントラクトアカウントの作成としてこの呼び出しを解釈します。

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


このコントラクトの mint メソッドを呼び出すには、同じ *CallEngine* コマンドを、input パラメータにメソッドセレクタを含めて送信します。メソッドセレクタとは、関数シグネチャの *keccak256* ハッシュの先頭 4 バイトのことです。

``keccak256(‘mint(address,uint256)’) == ‘40c10f19’``

これに、コントラクトの ABI ルールに従ってエンコードされた関数引数を連結します。最初の関数引数は *address* 型、つまり 20 バイト長の整数です。

例えば、コントラクトの所有者（*admin@test* の Iroha アカウント）が自分自身に 1000 枚のコインを mint して割り当てたいとします。
*admin@test* に対応する EVM アドレスを Python ライブラリを使って取得するには、次のようにします。

.. code-block:: python

	import sha3
	k = sha3.keccak_256()
	k.update(b'admin@test')
	print(hexlify(k.digest()[12:32]).zfill(64))

このようにすると、次の値が得られます。

``000000000000000000000000f205c4a929072dd6e7fc081c2a78dbc79c76070b``

つまり、末尾の 20 バイトが keccak256 の値であり、32 バイトになるよう左側にゼロでパディングされています。EVM のアドレスは、この 32 バイト表現の下位 20 バイトに相当します。


*amount* 引数は 16 進数でエンコードされた *uint256* 数値です（同様に左側にパディングされています）。

``00000000000000000000000000000000000000000000000000000000000003e8``

引数文字列全体は、上記 3 つの部分を順に連結したものになります。関数セレクタが先頭に来て、その後にエンコード済みの引数が並ぶという構成は、EVM でコントラクトのメソッドを呼び出すときの標準的な形式です。


すべてをまとめると、*Coin* コントラクトの *mint* 関数を呼び出すためのクライアントコードは次のようになります。

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

*send* 関数の呼び出しもまったく同じ方法で行います。
違いは、*callee* に ServiceContract を指定する点と、input に送信したい引数をセレクタ付きでエンコードして渡す点だけです。

なお、send 関数の最後の行では、先ほど説明したようにログに記録される Sent イベントが発行されていることに注意してください。

.. code-block:: solidity

	emit Sent(msg.sender, receiver, amount);


ケース 2：Iroha の状態との連携
------------------------------------

ここまで、Iroha の状態とやり取りしないコントラクトの例を見てきました。
しかし、Iroha ブロックチェーンの上で動作するほとんどの実際のアプリケーション（トランザクション処理におけるカスタムビジネスロジックやトランザクション手数料の徴収など）を考えると、Iroha の状態とやり取りできることは不可欠です。
このセクションでは、EVM スマートコントラクトの内部から Iroha アカウントの残高を照会する方法（クエリの作成者が適切な権限を持っていることが前提）の例を検討します。
Iroha の状態へのアクセスは、先ほど紹介したサービスコントラクトを介して行われます。スマートコントラクト側からは、delegatecall を使ってサービスコントラクトの公開 API を呼び出すだけで、残高の照会やアカウント作成、資産の追加や移転といった Iroha の操作を実行できます。


そのためのコントラクトのサンプルコードは次のとおりです。

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

コンストラクタでは、Iroha の状態とやり取りするための複数の API を公開している `サービスコントラクト <burrow.html#iroha-native-commands-evm>`_ の EVM アドレスを初期化しています。
これらの API は Iroha の状態の照会にも変更にも使用できます。Iroha のコマンドとクエリのほとんどが統合されています。
このコントラクトは、Iroha の *ServiceContract* API の *getAssetBalance*、*createAccount*、*addAsset*、*transferAsset* メソッドを呼び出しています。

上記のコントラクトをコンパイルしてバイトコードを得るには、本格的な Solidity コンパイラまたは Web ベースの *Remix IDE* を使用します。
これで、Python の Iroha クライアントからトランザクションを送信して、コントラクトを EVM にデプロイしたり、コントラクトのさまざまな関数を呼び出したりできます。
コントラクトのデプロイは、上記と同様の方法で行われます。デプロイ済みコントラクトの関数を呼び出すには、関数シグネチャとその引数を `ABI 仕様 <https://solidity.readthedocs.io/en/v0.6.5/abi-spec.html>`_ に従ってエンコードする必要があります。

デプロイ済みコントラクトの関数を呼び出す Python コードの例を次に示します。

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

string 型の引数という動的型は、次のようにエンコードされます。
まず、引数ブロックの先頭からのバイト単位で測った引数の位置部分を提供し、それを 32 バイトに左側パディングします。次に、引数のデータ部分は要素数で表したバイト配列の長さで始まり、これも 32 バイトに左側パディングされます。その後に文字列の UTF-8 エンコーディングが続き、右側を 32 バイトにパディングします。
これは、この例に含まれる関数を使えば実現できます。エンコードのルールを正しく適用することで、呼び出し先のコントラクト側が引数を正しく解釈できるようになります。

さらに多くの例やこのコードの動作については、`こちら <https://github.com/baziorek/iroha1-cpp/tree/iroha1-main/example/burrow_integration>`_ を参照してください。
実際のプロジェクトでは、デプロイしたコントラクトのアドレスを記録しておき、必要に応じてそのアドレスを callee に指定して各関数を呼び出す形になります。
