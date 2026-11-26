.. _architecture:

Что внутри Iroha?
=================

Сеть HL Iroha состоит из нескольких ключевых компонентов, которые
обеспечивают связь между узлами. Вы можете прочитать о них ниже.

.. image:: ../../image_assets/pipeline-diagram.png
	:width: 80%
	:align: center
	:alt: Iroha Architecture Diagram

Torii
-----

⛩

*Torii* () — традиционные японские ворота, отмечающие вход в
храм — подходящее название для единственной точки входа Iroha.

Точка входа для `клиентов <glossary.html#client>`__.
Использует gRPC в качестве транспорта.
Чтобы взаимодействовать с Irohой, любой может использовать
gRPC-эндпоинты, описанные в разделах `Commands
<../develop/api/commands.html>`__ и `Queries
<../develop/api/queries.html>`__, или использовать `клиентские
библиотеки <../develop/libraries.html>`__.

MST Processor
-------------

*Процессор мультиподписных транзакций*

Это внутренний gRPC-сервис, который отправляет и получает
сообщения от других пиров через `протокол Gossip
<https://en.wikipedia.org/wiki/Gossip_protocol>`_. Его задача —
рассылать `мультиподписные транзакции
<glossary.html#multisignature-transactions>`_, которые ещё не
набрали достаточно подписей для достижения `кворума
<glossary.html#quorum>`_, пока он не будет достигнут.

.. note::
    Распространение мультиподписных (MST) транзакций между пирами
    работает только когда ``mst_enable`` установлено в ``true`` в
    конфигурации узла (см. :ref:`configuration`). При выключенном
    флаге MST-транзакции по-прежнему работают локально на каждом
    пире, но пиры не обмениваются ожидающими MST-транзакциями.

Peer Communication Service
--------------------------

Внутренний компонент Iroha — посредник, который передаёт
`транзакцию <glossary.html#transaction>`__ из `Torii <#torii>`__
через `MstProcessor <#MstProcessor>`_ в `Ordering Gate
<#ordering-gate>`__. Главная цель PCS — скрыть сложность
взаимодействия с реализацией консенсуса.

Ordering Gate
-------------

Это внутренний компонент Iroha (gRPC-клиент), который передаёт
`транзакции <glossary.html#transaction>`__ из `Peer Communication
Service <#peer-communication-service>`__ в `Ordering Service
<#ordering-service>`__. Ordering Gate получает `предложения
<glossary.html#proposal>`_ (потенциальные блоки в цепочке) от
Ordering Service и отправляет их в `Симулятор <#simulator>`__ для
`состоятельной валидации
<glossary.html#stateful-validation>`__. Он также запрашивает
предложение у Ordering Service на основе раунда консенсуса.

Ordering Service
----------------

Внутренний компонент Iroha (gRPC-сервер), который получает
сообщения от других `пиров <glossary.html#peer>`__ и объединяет
несколько `транзакций
<glossary.html#transaction>`__, прошедших `безсостоятельную
валидацию <glossary.html#stateless-validation>`__, в
`предложение <glossary.html#proposal>`__. У каждого узла свой
ordering service. Создание предложения может быть вызвано одним
из следующих событий:

1. Истёк лимит времени, отведённый на сбор транзакций.

2. Ordering service получил максимальное допустимое количество
   транзакций для одного предложения.

Оба параметра (таймаут и максимальный размер предложения)
настраиваются (см. страницу `параметров, зависящих от окружения
<../configure/index.html#environment-specific-parameters>`_).

Общим предусловием для обоих триггеров является то, что хотя бы
одна транзакция должна дойти до ordering service. Иначе
предложение не будет сформировано.

Ordering service также выполняет предварительную валидацию
предложений (например, удаление безсостоятельно отклонённых
транзакций из предложения).

Verified Proposal Creator
-------------------------

Внутренний компонент Iroha, который выполняет `состоятельную
валидацию <glossary.html#stateful-validation>`__ `транзакций
<glossary.html#transaction>`__, содержащихся в полученном
`предложении <glossary.html#proposal>`__ от `Ordering Service
<#ordering-service>`__. На основе транзакций, прошедших
состоятельную валидацию, будет создано **верифицированное
предложение** и передано в `Block Creator <#block-creator>`__.
Все транзакции, не прошедшие состоятельную валидацию, будут
отброшены и не включены в верифицированное предложение.

Block Creator
-------------

Компонент системы, который формирует блок из набора транзакций,
прошедших `безсостоятельную
<glossary.html#stateless-validation>`__ и `состоятельную
<glossary.html#stateful-validation>`__ валидацию, для дальнейшего
распространения в `консенсусе <#consensus>`__.

Block creator вместе с `Verified Proposal Creator
<#verified-proposal-creator>`__ образует компонент под названием
`Симулятор <https://github.com/baziorek/iroha1-cpp/tree/iroha1-main/irohad/simulator>`_.

Block Consensus (YAC)
---------------------

*Консенсус как компонент*

Консенсус — сердце блокчейна: он поддерживает согласованное
состояние среди `пиров <glossary.html#peer>`__ в пир-сети. Iroha
использует собственный алгоритм консенсуса под названием Yet
Another Consensus (он же YAC). Подробное описание протокола —
раунды, голосование, модели супербольшинства и обработка сбоев —
см. :ref:`yac`.

Вы можете посмотреть видео с подробным объяснением принципов
консенсуса, а именно YAC, `здесь <https://youtu.be/mzuAbalxOKo>`__.

Отличительными чертами алгоритма YAC являются его
масштабируемость, производительность и устойчивость к сбоям
(Crash fault tolerance).

Чтобы обеспечить согласованность в сети, отсутствующие блоки
будут загружены с другого пира через `Синхронизатор
<#synchronizer>`__. Подтверждённые блоки хранятся в хранилище
блоков `Ametsuchi <#ametsuchi>`__.

Общее определение консенсуса смотрите `по этой ссылке
<glossary.html#consensus>`_.


Synchronizer
------------

Является частью `консенсуса <#consensus>`__. Добавляет
отсутствующие блоки в `цепочки пиров
<glossary.html#peer>`__ (загружает их с других пиров для
поддержания согласованности).

Ametsuchi Blockstore
--------------------

*Ametsuchi* () — хранилище небесных сокровищ в японской
мифологии; здесь — компонент хранения Iroha, который хранит
блоки и состояние, генерируемое из блоков, называемое `World
State View <#world-state-view>`__. У `клиента
<glossary.html#client>`__ нет способа напрямую взаимодействовать с
Ametsuchi.


World State View
----------------

WSV отражает текущее состояние системы и может рассматриваться
как снимок. Например, WSV хранит информацию о количестве
`активов <glossary.html#asset>`__, которые есть у `аккаунта
<glossary.html#account>`__ в данный момент, но не содержит
никакой информации об истории потока `транзакций
<glossary.html#transaction>`__.

Отправка транзакций с помощью библиотеки Python
===============================================

Откройте новый терминал (обратите внимание, что контейнер Iroha и
``irohad`` должны быть запущены) и подключитесь к docker-контейнеру
``iroha``:

.. code-block:: shell

  docker exec -it iroha /bin/bash

Теперь вы в интерактивной оболочке контейнера Iroha.

Предварительные требования
---------------------------

.. note:: Библиотека работает только в среде Python 3 (Python 2 не
  поддерживается).

Чтобы использовать библиотеку Python Iroha, её нужно получить из
`репозитория <https://github.com/baziorek/iroha1-python>`_
(community-форк `iroha-python
<https://github.com/hyperledger/iroha-python>`_) или через pip3:

.. code-block:: shell

	pip3 install iroha

Создание собственных пар ключей с помощью библиотеки Python
-----------------------------------------------------------

Для тестовых целей можно использовать примерные ключи. Но можно
создать и свои. Чтобы создать **нативные ключи Iroha ed25519
SHA-3** (различие между алгоритмами можно найти `здесь
<../develop/keys.html>`__), используйте следующий код:

.. code-block:: python

	from iroha import IrohaCrypto

	# этих двух первых строк достаточно для создания ключей
	private_key = IrohaCrypto.private_key()
	public_key = IrohaCrypto.derive_public_key(private_key)

	# остальной код записывает их в файл
	with open('keypair.priv', 'wb') as f:
	    f.write(private_key)

	with open('keypair.pub', 'wb') as f:
	    f.write(public_key)

А для ключей HL Ursa ed25519 SHA-2 в формате Multihash
используйте:

.. code-block:: python

	from iroha import IrohaCrypto, ed25519_sha2
	from nacl.encoding import HexEncoder

	private_key = ed25519_sha2.SigningKey.generate()
	public_key = IrohaCrypto.derive_public_key(private_key).encode('ascii')

	with open('keypair.priv', 'wb') as f:
	    f.write(private_key.encode(encoder=HexEncoder))
	    f.write(public_key[6:])

	with open('keypair.pub', 'wb') as f:
	    f.write(public_key)

Теперь, когда у нас есть библиотека и ключи, мы можем начать
отправлять реальные транзакции.

Запуск примерных транзакций
---------------------------

Если вы просто хотите посмотреть, как выглядят транзакции Iroha,
можно просто перейти к примерам из репозитория `здесь
<https://github.com/baziorek/iroha1-python/tree/iroha1-main/examples>`_.
Вот файл `tx-example.py` с комментариями, поясняющими каждый шаг:

.. remoteliteralinclude:: https://raw.githubusercontent.com/baziorek/iroha1-python/iroha1-main/examples/tx-example.py
   :language: python

Теперь, если у вас запущен ``irohad``, вы можете запустить пример
или собственный файл, просто открыв файл .py в другой вкладке.

.. note:: В `каталоге примеров iroha-python
  <https://github.com/baziorek/iroha1-python/tree/iroha1-main/examples>`_
  есть ещё образцы.
