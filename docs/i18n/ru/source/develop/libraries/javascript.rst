Библиотека JavaScript (неподдерживаемая)
----------------------------------------

.. note::
   Эта страница описывает апстрим npm-пакет ``iroha-helpers``, который
   **не поддерживается с февраля 2024** (последний коммит в ветке
   ``iroha1-main``) — он по-прежнему работает с узлами Iroha 1,
   но не получает обновлений. Для новых проектов предпочитайте
   активно поддерживаемые библиотеки :doc:`cpp` или :doc:`python`
   этого форка.

.. image:: https://img.shields.io/npm/v/iroha-helpers.svg

Эта библиотека предоставляет функции, которые помогут вам
взаимодействовать с Hyperledger Iroha из вашей JS-программы.

Установка
^^^^^^^^^^
Через npm

.. code:: sh

    $ npm i iroha-helpers

Через yarn

.. code:: sh

    $ yarn add iroha-helpers

Исходный код находится в `репозитории iroha-javascript
<https://github.com/hyperledger-iroha/iroha-javascript/tree/iroha1-main>`__
(код Iroha 1 в ветке ``iroha1-main``; ``main`` — это Iroha 2).

Команды
^^^^^^^
Для использования любой команды нужно передать
``commandOptions`` первым аргументом.

.. code-block:: javascript

  const commandOptions = {
    privateKeys: ['f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70'], // Array of private keys in hex format
    creatorAccountId: '', // Account id, ex. admin@test
    quorum: 1,
    commandService: null
  }

Вторым аргументом нужно передать объект, содержащий
свойства, требуемые командой.

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

Запросы
^^^^^^^
Для использования любого запроса нужно передать
``queryOptions`` первым аргументом.

.. code-block:: javascript

  const queryOptions = {
    privateKey: 'f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70', // Private key in hex format
    creatorAccountId: '', // Account id, ex. admin@test
    queryService: null
  }

Вторым аргументом нужно передать объект, содержащий
свойства, требуемые запросом.

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

Пример кода
^^^^^^^^^^^

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
