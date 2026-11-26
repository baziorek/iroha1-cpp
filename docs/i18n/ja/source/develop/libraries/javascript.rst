JavaScript ライブラリ (メンテナンスされていません)
--------------------------------------------------

.. note::
   このページはアップストリームの npm パッケージ ``iroha-helpers``
   を説明しています。これは **2024年2月以降メンテナンスされて
   いません** (``iroha1-main`` ブランチの最終コミット) —
   Iroha 1 ノードとは引き続き動作しますが、更新はありません。
   新規プロジェクトではこのフォークの活発にメンテナンスされて
   いる :doc:`cpp` または :doc:`python` ライブラリを推奨します。

.. image:: https://img.shields.io/npm/v/iroha-helpers.svg

このライブラリは、JS プログラムから Hyperledger Iroha と
対話するのに役立つ関数を提供します。

インストール
^^^^^^^^^^^^
npm 経由

.. code:: sh

    $ npm i iroha-helpers

yarn 経由

.. code:: sh

    $ yarn add iroha-helpers

ソースコードは `iroha-javascript リポジトリ
<https://github.com/hyperledger-iroha/iroha-javascript/tree/iroha1-main>`__
にあります (``iroha1-main`` ブランチが Iroha 1 コード;
``main`` は Iroha 2)。

コマンド
^^^^^^^^
任意のコマンドを使用するには、最初の引数として
``commandOptions`` を渡す必要があります。

.. code-block:: javascript

  const commandOptions = {
    privateKeys: ['f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70'], // Array of private keys in hex format
    creatorAccountId: '', // Account id, ex. admin@test
    quorum: 1,
    commandService: null
  }

2 番目の引数として、コマンドが必要とするプロパティを
含むオブジェクトを渡す必要があります。

.. code-block:: javascript
  
  // Example usage of setAccountDetail

  const commandService = new CommandService_v1Client(
    '127.0.0.1:50051',
    grpc.credentials.createInsecure()
  )

  const adminPriv = 'f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70'

  commands.setAccountDetail({
    privateKeys: [adminPriv],
    creatorAccountId: 'admin@test',
    quorum: 1,
    commandService
  }, {
    accountId: 'admin@test',
    key: 'jason',
    value: 'statham'
  })

クエリ
^^^^^^
任意のクエリを使用するには、最初の引数として
``queryOptions`` を渡す必要があります。

.. code-block:: javascript

  const queryOptions = {
    privateKey: 'f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70', // Private key in hex format
    creatorAccountId: '', // Account id, ex. admin@test
    queryService: null
  }

2 番目の引数として、クエリが必要とするプロパティを
含むオブジェクトを渡す必要があります。

.. code-block:: javascript
  
  // Example usage of getAccountDetail
  
  const queryService = new QueryService_v1Client(
    '127.0.0.1:50051',
    grpc.credentials.createInsecure()
  )

  const adminPriv = 'f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70'

  queries.getAccountDetail({
    privateKey: adminPriv,
    creatorAccountId: 'admin@test',
    queryService
  }, {
    accountId: 'admin@test'
  })

サンプルコード
^^^^^^^^^^^^^^

.. code-block:: javascript

  import grpc from 'grpc'
  import {
    QueryService_v1Client,
    CommandService_v1Client
  } from '../iroha-helpers/lib/proto/endpoint_grpc_pb'
  import { commands, queries } from 'iroha-helpers'

  const IROHA_ADDRESS = 'localhost:50051'
  const adminPriv =
    'f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70'

  const commandService = new CommandService_v1Client(
    IROHA_ADDRESS,
    grpc.credentials.createInsecure()
  )

  const queryService = new QueryService_v1Client(
    IROHA_ADDRESS,
    grpc.credentials.createInsecure()
  )

  Promise.all([
    commands.setAccountDetail({
      privateKeys: [adminPriv],
      creatorAccountId: 'admin@test',
      quorum: 1,
      commandService
    }, {
      accountId: 'admin@test',
      key: 'jason',
      value: 'statham'
    }),
    queries.getAccountDetail({
      privateKey: adminPriv,
      creatorAccountId: 'admin@test',
      queryService
    }, {
      accountId: 'admin@test'
    })
  ])
    .then(a => console.log(a))
    .catch(e => console.error(e))
