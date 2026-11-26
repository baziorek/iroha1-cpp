HL Burrow 連携
=====================

Iroha のメンテナーとして、私たちはユーザーの皆様からカスタムスマートコントラクト対応に関する多くの質問や要望をいただいてきました。
そして皆様が業務上のニーズをより自由に実現できるよう、Hyperledger グリーンハウスのもう一つの素晴らしいプロジェクトである HL Burrow EVM を Iroha に統合しました。

.. note:: Iroha の文脈において、HL Burrow は Solidity スマートコントラクトを実行できる Ethereum Virtual Machine を提供します。
	私たちは可能な限り最高のユーザー体験を提供できるよう最善を尽くしました。そして Iroha でこれを利用するには、ビルド時に `CMake フラグ <../build/index.html#cmake-parameters>`_ を 1 つ追加するだけでよく、あとはすぐに動作を開始します。

	HL Burrow 連携を組み込んでビルドされた `Hyperledger Burrow <https://hyperledger.github.io/burrow/#/>`_ の公式 Docker イメージも存在します。それらを利用するには、`hyperledger/iroha:latest` を `hyperledger/iroha-burrow:latest` に置き換えてください。

この言語に初めて触れる方のために、Solidity スマートコントラクト言語については `こちら <https://solidity.readthedocs.io/>`_ を参照してください。

仕組み
------------

この統合のために、Iroha には特別な `Call Engine コマンド <../develop/api/commands.html#call-engine>`_ を用意しました。また、このコマンドの結果を取得するための特別な `Engine Receipts クエリ <../develop/api/queries.html#engine-receipts>`_ も用意しています。

コマンド
^^^^^^^^^^^

このコマンドでは、以下のことが可能です。

**EVM 内に新しいコントラクトアカウントを作成する**

`CallEngine <../develop/api/commands.html#call-engine>`_ の *callee* が指定されておらず、*input* パラメータにバイトコードが含まれている場合、新しいコントラクトアカウントが作成されます。

**以前にデプロイされたコントラクトのメソッドを呼び出す**

*callee* が指定されている場合、入力は呼び出し先コントラクトのメソッドの ABI エンコードされたセレクタに続く引数として扱われます。

.. hint:: これは Ethereum でコントラクトをデプロイしたりコントラクト関数を呼び出したりするのと非常によく似ています。`eth_sendTransaction <https://github.com/ethereum/wiki/wiki/JSON-RPC#eth_sendtransaction>`_ メッセージコールの `data` フィールドの内容によって動作が決まります。
	詳細については `ABI 仕様 <https://solidity.readthedocs.io/en/v0.6.5/abi-spec.html>`_ を参照してください。

クエリ
^^^^^^^^^

`CallEngine <../develop/api/commands.html#call-engine>`_ コマンドの実行結果をクエリするには、`Engine Receipts クエリ <../develop/api/queries.html#engine-receipts>`_ を使用します。
EVM 内で行われたあらゆる計算の出力は、その結果が台帳に書き込まれる（つまり、その Iroha トランザクションを含むブロックがコミットされる）まで、呼び出し元からは利用できません。
その他の `data <../develop/api/queries.html#response-structure>`_ の中でも、*EngineReceipts* クエリは *CallEngine* の実行中に EVM 内で生成されたログエントリの配列を返します。

.. hint:: dApps の開発者がコントラクト実行の結果を関心のある関係者に見せる一般的な方法は、コントラクト関数を終了する前にデータを含むイベントを発行し、そのデータが *Event Log* に書き込まれるようにすることです。
	`Ethereum イエローペーパー <https://ethereum.github.io/yellowpaper/paper.pdf>`_ では、ログエントリを発行者のアドレス、32 バイト長のトピックの配列、およびデータのバイト配列からなる 3 つ組として定義しています。

配列や複数の値をとる引数を渡すには、パラメータを含む JSON 文字列を送信します。
より詳細な使用例については例のページを参照してください。

.. _iroha-native-commands-evm:

EVM での Iroha ネイティブコマンドの実行
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

HL Burrow 連携を利用すると、Iroha の状態を変更するネイティブコマンドも使用できます。

Burrow EVM の統合メカニズムにより、Iroha アプリケーション開発者はスマートコントラクトのコードから Iroha の状態に直接作用できるツールを手に入れ、組み込みの Iroha コマンドシステムをプログラム可能なビジネスロジック拡張の基盤として活用できます。
条件付きの資産移転、トランザクション手数料、非代替性トークンなどは、このような拡張のほんの一例にすぎません。
ここで難しいのは、Iroha のデータモデルが Ethereum のデータモデルとかなり異なっている点です。
例えば、Ethereum には組み込みの資産の種類が `Eth` しかないため、EVM のコンテキストでアカウント残高を取得することは、単にアカウントの残高プロパティを返すことを意味します。
一方 Iroha では、アカウントは複数の資産を持つことも、まったく資産を持たないこともあります。そのため、アカウント残高を返す関数は、少なくとも資産 ID という追加の引数を 1 つ必要とします。
アカウント間で資産を移転・送信する場合も同じロジックが適用されます。

このデータモデルの不一致問題への解決策として、Burrow にはいわゆるサービスコントラクト（Service Contract）を導入しました。これは Iroha のデータモデルを「認識」しており、Iroha の状態とやり取りするための API（残高の照会、資産の移転など）を公開します。

.. note:: ネイティブ機能と外部ディスパッチャの詳細については `Burrow ドキュメント <https://wiki.hyperledger.org/display/burrow/Burrow+-+The+Boring+Blockchain>`_ を参照してください。

システムの各部分間の相互作用は、概念的には次のようになります。

.. image:: ../../image_assets/burrow/natives.svg

.. attention::
	Burrow EVM の観点から見ると、このようなサービスコントラクトはネイティブ外部 VM 内にホストされており、EVM 内の特別なアドレスにデプロイされたかのように同じインターフェース経由で呼び出すことができます。
	これらのメソッドは特に Iroha 連携のために使用されるため、サービスコントラクトのアドレスは Iroha 経由で作業している場合にのみ見つけることができます。

現在の Iroha EVM ラッパーのリリースには、アドレス `A6ABC17819738299B3B2C1CE46D55C74F04E290C` （文字列 *ServiceContract* の *keccak256* ハッシュの下位 20 バイト）にデプロイされた単一のサービスコントラクトが含まれており、以下の Iroha コマンドとクエリを公開しています。

.. list-table:: 統合されたコマンド/クエリ
   :widths: 3 35 35
   :header-rows: 1

   * - 番号
     - 公開される API メソッドのシグネチャ
     - 引数
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


.. hint:: 開発者の観点から見ると、後者のアドレスが既知であれば、ネイティブコントラクトの関数を呼び出すことは、他のスマートコントラクトのメソッドを呼び出すのとまったく変わりません。

	bytes memory payload = abi.encodeWithSignature("getAssetBalance(string,string)", "myacc@test", "coin#test");

	(bool success, bytes memory ret) = address(0xA6ABC17819738299B3B2C1CE46D55C74F04E290C).delegatecall(payload);

ここでは特別な種類の EVM メッセージコール、**delegatecall** が使用されています。これはコントラクトが実行時に異なるアドレスからコードを動的に読み込み、自身の実行コンテキストで実行できるようにするものです。

.. seealso:: では、次に `使用例 <burrow_example.html>`_ のページに進みましょう









