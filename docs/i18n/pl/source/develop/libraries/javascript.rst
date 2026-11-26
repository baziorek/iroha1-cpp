Biblioteka JavaScript (nieutrzymywana)
--------------------------------------

.. note::
   Ta strona opisuje upstreamowy pakiet npm ``iroha-helpers``, który
   jest **nieutrzymywany od lutego 2024** (ostatni commit na gałęzi
   ``iroha1-main``) — nadal działa z węzłami Iroha 1, ale nie
   otrzymuje aktualizacji. Do nowych projektów preferuj aktywnie
   utrzymywane biblioteki :doc:`cpp` lub :doc:`python` tego forka.

.. image:: https://img.shields.io/npm/v/iroha-helpers.svg

Ta biblioteka dostarcza funkcje, które pomogą Ci wchodzić w
interakcję z Hyperledger Iroha z Twojego programu JS.

Instalacja
^^^^^^^^^^
Przez npm

.. code:: sh

    $ npm i iroha-helpers

Przez yarn

.. code:: sh

    $ yarn add iroha-helpers

Kod źródłowy znajduje się w `repozytorium iroha-javascript
<https://github.com/hyperledger-iroha/iroha-javascript/tree/iroha1-main>`__
(kod Iroha 1 na gałęzi ``iroha1-main``; ``main`` to Iroha 2).

Polecenia
^^^^^^^^^
Do użycia jakiegokolwiek polecenia musisz podać
``commandOptions`` jako pierwszy argument.

.. code-block:: javascript

  const commandOptions = {
    privateKeys: ['f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70'], // Array of private keys in hex format
    creatorAccountId: '', // Account id, ex. admin@test
    quorum: 1,
    commandService: null
  }

Jako drugi argument musisz podać obiekt zawierający
właściwości wymagane przez polecenie.

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

Zapytania
^^^^^^^^^
Do użycia jakiegokolwiek zapytania musisz podać
``queryOptions`` jako pierwszy argument.

.. code-block:: javascript

  const queryOptions = {
    privateKey: 'f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70', // Private key in hex format
    creatorAccountId: '', // Account id, ex. admin@test
    queryService: null
  }

Jako drugi argument musisz podać obiekt zawierający
właściwości wymagane przez zapytanie.

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

Przykładowy kod
^^^^^^^^^^^^^^^

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
