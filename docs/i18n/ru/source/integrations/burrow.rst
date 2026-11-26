Интеграция HL Burrow
====================

Как мейнтейнеры Iroha, мы получили множество вопросов и запросов от пользователей на поддержу пользовательских смарт-контрактов.
И чтобы предоставить им большую свободу в удовлетворении их бизнес-потребностей, мы интегрировали в Iroha HL Burrow EVM – ещё один замечательный проект «теплицы» Hyperledger.

.. note:: В контексте Iroha HL Burrow предоставляет виртуальную машину Ethereum (EVM), которая может запускать смарт-контракты на Solidity.
	Мы постарались обеспечить максимально удобный опыт использования – и чтобы работать с ним в Iroha, достаточно добавить `флаг CMake при сборке Iroha <../build/index.html#cmake-parameters>`_, и всё заработает сразу же.
	
	Существует официальный docker-образ, собранный с `интеграцией Hyperledger Burrow <https://hyperledger.github.io/burrow/#/>`_. Чтобы использовать его, замените `hyperledger/iroha:latest` на `hyperledger/iroha-burrow:latest`.

О языке смарт-контрактов Solidity можно прочитать `здесь <https://solidity.readthedocs.io/>`_, если вы ещё не знакомы с этим языком.

Как это работает
----------------

Для этой интеграции мы создали в Iroha специальную команду `Call Engine <../develop/api/commands.html#call-engine>`_, а также специальный запрос `Engine Receipts <../develop/api/queries.html#engine-receipts>`_ для получения результатов выполнения команды.

Команда
^^^^^^^

В команде можно:

**Создать новый контрактный аккаунт в EVM**

Если *callee* в `CallEngine <../develop/api/commands.html#call-engine>`_ не указан, а параметр *input* содержит какой-либо байткод, то создаётся новый контрактный аккаунт.

**Вызвать метод ранее развёрнутого контракта**

Если *callee* указан, то *input* трактуется как ABI-кодированный селектор метода контракта-адресата (*callee*), за которым следуют аргументы.

.. hint:: Это очень похоже на развёртывание контракта или вызов функции контракта в Ethereum, в зависимости от содержимого поля `data` вызова сообщения `eth_sendTransaction <https://github.com/ethereum/wiki/wiki/JSON-RPC#eth_sendtransaction>`_.
	Подробности см. в `ABI-спецификации <https://solidity.readthedocs.io/en/v0.6.5/abi-spec.html>`_.

Запрос
^^^^^^

Чтобы получить результат команды `CallEngine <../develop/api/commands.html#call-engine>`_, следует использовать запрос `Engine Receipts <../develop/api/queries.html#engine-receipts>`_.
Результат любых вычислений внутри EVM будет недоступен вызывающей стороне, пока он не будет записан в леджер (то есть пока блок, содержащий соответствующую транзакцию Iroha, не будет зафиксирован).
Среди прочих `данных <../develop/api/queries.html#response-structure>`_ запрос *EngineReceipts* вернёт массив записей журнала (*log entries*), сгенерированных в EVM во время выполнения *CallEngine*.

.. hint:: Распространённый способ для разработчиков dApp дать заинтересованным сторонам возможность увидеть результат выполнения контракта – эмитировать событие с некоторыми данными перед выходом из функции контракта, чтобы эти данные были записаны в *Event Log*.
	`Ethereum Yellow Paper <https://ethereum.github.io/yellowpaper/paper.pdf>`_ определяет запись журнала как тройку, содержащую адрес отправителя, массив тем длиной 32 байта и байтовый массив некоторых данных.

Передача упорядоченного списка значений в качестве аргументов выполняется путём отправки JSON-строки с параметрами.
Больше примеров см. в примерах.

Запуск нативных команд Iroha в EVM
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

С интеграцией HL Burrow вы также можете использовать нативные команды для изменения состояния Iroha.

Механизм интеграции Burrow EVM наделяет разработчиков приложений Iroha инструментом, способным напрямую воздействовать на состояние Iroha из кода смарт-контрактов, что обеспечивает основу для программируемых расширений встроенной системы команд Iroha.
Условные переводы активов, комиссии за транзакции, невзаимозаменяемые активы и т.д. – это лишь несколько примеров таких расширений.
Сложная часть здесь в том, что модель данных Iroha существенно отличается от модели данных Ethereum.
Например, в Ethereum существует только один вид встроенного актива (`Eth`), поэтому получение баланса аккаунта в контексте EVM просто означает возврат свойства *balance* аккаунта.
В Iroha же аккаунт может иметь несколько активов или не иметь активов вообще, поэтому любая функция, возвращающая баланс аккаунта, должна принимать как минимум один дополнительный аргумент – идентификатор актива.
Та же логика применима к переводу/отправке активов с одного аккаунта на другой.

В качестве решения этой проблемы несоответствия моделей данных мы вводим так называемые Service Contracts (сервисные контракты) в Burrow, которые «осведомлены» о модели данных Iroha и предоставляют API для взаимодействия с состоянием Iroha (запрос балансов, перевод активов и т.д.).

.. note:: Более подробную информацию о Natives и внешних диспетчерах можно найти в `документации Burrow <https://wiki.hyperledger.org/display/burrow/Burrow+-+The+Boring+Blockchain>`_.

Схематично взаимодействие между различными частями системы выглядит следующим образом:

.. image:: ../../image_assets/burrow/natives.svg

.. attention::
	С точки зрения Burrow EVM такой Service Contract размещён в нативной внешней VM и может вызываться через те же интерфейсы, как если бы он был развёрнут по какому-то специальному адресу в самой EVM.
	Эти методы используются специально для интеграции с Iroha, поэтому адрес Service Contract можно найти только при работе с ним через Iroha.

Текущий релиз обёртки Iroha EVM содержит единственный сервисный контракт, развёрнутый по адресу `A6ABC17819738299B3B2C1CE46D55C74F04E290C` (последние 20 байт *keccak256*-хэша строки *ServiceContract*), который предоставляет следующие команды и запросы Iroha:

.. list-table:: Интегрированные команды/запросы
   :widths: 3 35 35
   :header-rows: 1

   * - №
     - Сигнатуры предоставляемых методов API
     - Аргументы
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


.. hint:: С точки зрения разработчика вызов функции нативного контракта ничем не отличается от вызова метода любого другого смарт-контракта, при условии что известен адрес последнего:

	bytes memory payload = abi.encodeWithSignature("getAssetBalance(string,string)", "myacc@test", "coin#test");

	(bool success, bytes memory ret) = address(0xA6ABC17819738299B3B2C1CE46D55C74F04E290C).delegatecall(payload);

Здесь используется особый вид вызовов сообщений EVM – **delegatecall**, который позволяет контракту динамически загружать и выполнять код по другому адресу во время выполнения в собственном контексте исполнения.

.. seealso:: А теперь перейдём к примерам `использования <burrow_example.html>`_
