.. _node-configuration:

======================
Конфигурация узла
======================

Каждый экземпляр ``irohad`` настраивается одним JSON-файлом, передаваемым
флагом ``--config`` (вместе с ``--genesis_block`` и ``--keypair_name``
при первом запуске — см. :ref:`node-appimage`).

В репозитории есть готовые примеры, которые можно скопировать и
адаптировать:

- ``example/config.sample`` — общая конфигурация (PostgreSQL на localhost)
- ``example/config.docker`` — используется образом Docker
- ``example/config.postgres.sample`` — явный вариант для PostgreSQL
- ``example/config-win.sample`` — вариант для Windows

.. note:: Это руководство относится к форку, поддерживаемому
  сообществом `baziorek/iroha1-cpp <https://github.com/baziorek/iroha1-cpp>`_.

Ключевые настройки
------------------

- ``database`` — backend хранения: ``postgres`` (host, port, учётные
  данные, рабочая/обслуживающая БД) или RocksDB через
  ``block_store_path``
- ``torii_port`` — gRPC-порт для клиентов (по умолчанию ``50051``)
- ``internal_port`` — gRPC-порт между пирами (по умолчанию ``10001``)
- ``max_proposal_size`` — максимальное число транзакций в блоке/предложении
- ``vote_delay`` / ``proposal_creation_timeout`` — тайминги консенсуса
- ``mst_enable`` / ``mst_expiration_time`` — поддержка мультиподписных
  транзакций (см. :ref:`architecture`)
- ``metrics`` / ``healthcheck_port`` — эндпоинты наблюдаемости

Полный справочник параметров
----------------------------

Каждый параметр конфигурации подробно документирован — типы, значения
по умолчанию, единицы и побочные эффекты — в разделе :ref:`configuration`.
В частности см. `параметры, зависящие от окружения
<../configure/index.html#environment-specific-parameters>`_ с советами
по тюнингу.

.. hint:: Для сети из нескольких узлов каждому пиру нужны собственный
  конфиг, пара ключей и genesis block со списком всех пиров — такая
  настройка описана в :ref:`deploy-guide`.
