=======
Метрики
=======

Чтобы удобно и тщательно отслеживать производительность сети, вы
можете использовать метрики. Это числовые данные, собираемые с
течением времени о вашей сети Iroha. Затем вы можете анализировать
данные, чтобы сделать ваш проект ещё более эффективным!

Как использовать метрики?
=========================

Чтобы использовать метрики, нужно добавить их в вашу
`конфигурацию Iroha
<../configure/index.html#deployment-specific-parameters>`_ и
использовать версию Iroha новее 1.2.1.

.. note:: Если вы запускаете Irohу в Docker, чтобы получить
   доступ к метрикам извне контейнера, нужно: 1) В
   `конфигурации <../configure/index.html>`_ — установить
   ``"metrics":0.0.0.0:PORT``; 2) Открыть соответствующий порт в
   Docker при выполнении ``run ... -pPORT:PORT ...``

Затем можно просто использовать IP-адрес для доступа к данным от
работающего экземпляра Iroha.

Вот пример:

.. code-block:: shell

  > curl http://127.0.0.1:8080/metrics

вернёт результаты вида:

.. code-block:: shell

  # HELP blocks_height Total number of blocks in chain
  # TYPE blocks_height gauge
  blocks_height 135543
  # HELP peers_number Total number peers to send transactions and request proposals
  # TYPE peers_number gauge
  peers_number 7
  # HELP number_of_domains Total number of domains in WSV
  # TYPE number_of_domains gauge
  number_of_domains 14
  # HELP total_number_of_transactions Total number of transactions in blockchain
  # TYPE total_number_of_transactions gauge
  total_number_of_transactions 216499
  # HELP number_of_signatures_in_last_block Number of signatures in last block
  # TYPE number_of_signatures_in_last_block gauge
  number_of_signatures_in_last_block 5

Метрики транзакций с мультиподписью (MST)
==========================================

Когда используются транзакции с мультиподписью, эндпоинт
дополнительно предоставляет счётчики для MST, ожидающих
подписей:

.. code-block:: shell

  # HELP number_of_pending_mst_batches Number of pending MST batches
  # TYPE number_of_pending_mst_batches gauge
  number_of_pending_mst_batches 0
  # HELP number_of_pending_mst_transactions Number of pending MST transactions
  # TYPE number_of_pending_mst_transactions gauge
  number_of_pending_mst_transactions 0

Эти метрики отражают локальный кэш ожидающих MST — они наиболее
полезны на пирах с ``mst_enable``, установленным в ``true`` в
конфигурации (см. :ref:`configuration`), где состояние MST
разделяется между пирами.
