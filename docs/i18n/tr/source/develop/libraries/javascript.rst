JavaScript Kütüphanesi (bakımı yapılmıyor)
------------------------------------------

.. note::
   Bu sayfa, upstream ``iroha-helpers`` npm paketini açıklar; bu
   paket **Şubat 2024'ten beri bakımı yapılmamaktadır**
   (``iroha1-main`` dalındaki son commit) — Iroha 1 düğümleriyle
   çalışmaya devam eder ancak güncelleme almaz. Yeni projeler
   için bu forkun aktif olarak bakımı yapılan :doc:`cpp` veya
   :doc:`python` kütüphanelerini tercih edin.

.. image:: https://img.shields.io/npm/v/iroha-helpers.svg

Bu kütüphane, JS programınızdan Hyperledger Iroha ile
etkileşim kurmanıza yardımcı olacak işlevler sağlar.

Kurulum
^^^^^^^
npm ile

.. code:: sh

    $ npm i iroha-helpers

yarn ile

.. code:: sh

    $ yarn add iroha-helpers

Kaynak kod `iroha-javascript deposunda
<https://github.com/hyperledger-iroha/iroha-javascript/tree/iroha1-main>`__
bulunur (Iroha 1 kodu ``iroha1-main`` dalında; ``main``
Iroha 2'dir).

Komutlar
^^^^^^^^
Herhangi bir komutu kullanmak için ilk argüman olarak
``commandOptions`` sağlamanız gerekir.

.. code-block:: javascript

  const commandOptions = {
    privateKeys: ['f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70'], // Array of private keys in hex format
    creatorAccountId: '', // Account id, ex. admin@test
    quorum: 1,
    commandService: null
  }

İkinci argüman olarak, komutun gerektirdiği özellikleri
içeren bir nesne sağlamanız gerekir.

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

Sorgular
^^^^^^^^
Herhangi bir sorguyu kullanmak için ilk argüman olarak
``queryOptions`` sağlamanız gerekir.

.. code-block:: javascript

  const queryOptions = {
    privateKey: 'f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70', // Private key in hex format
    creatorAccountId: '', // Account id, ex. admin@test
    queryService: null
  }

İkinci argüman olarak, sorgunun gerektirdiği özellikleri
içeren bir nesne sağlamanız gerekir.

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

Örnek kod
^^^^^^^^^

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
