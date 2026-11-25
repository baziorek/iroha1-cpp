.. _node-appimage:

============================
Узел Iroha через AppImage
============================

AppImage — самодостаточные исполняемые файлы: без установки и настройки
зависимостей. Релизный pipeline Iroha 1 производит два из них:

- ``irohad-x86_64.AppImage`` — сам узел
- ``iroha-cli-x86_64.AppImage`` — консольный клиент
  (см. :doc:`cli`)

.. note:: AppImage прикрепляются к `GitHub Releases
  <https://github.com/baziorek/iroha1-cpp/releases>`__ форка
  сообщества начиная с тега ``1.6.1``. Пока релиз с ними не
  опубликован, используйте варианты :ref:`Docker <node-docker>` или
  :ref:`сборки из исходников <node-build>`.

Загрузка
--------

Скачайте AppImage для своей платформы со страницы
`Releases <https://github.com/baziorek/iroha1-cpp/releases>`__.

Запуск узла
-----------

AppImage по умолчанию не исполняемы — сначала сделайте файл
исполняемым:

.. code-block:: shell

  chmod +x irohad-x86_64.AppImage

Узлу нужны те же входные данные, что и любой другой сборке ``irohad``:
JSON-файл конфигурации, genesis block и пара ключей узла. В репозитории
есть готовые примеры в ``example/`` (``config.sample``,
``genesis.block``, ``node0.priv``/``node0.pub``) — склонируйте форк или
скопируйте этот каталог:

.. code-block:: shell

  git clone -b iroha1-main https://github.com/baziorek/iroha1-cpp --depth=1

При первом запуске используйте ``--overwrite_ledger``, чтобы genesis
block применился к пустому хранилищу:

.. code-block:: shell

  ./irohad-x86_64.AppImage \
    --config=iroha1-cpp/example/config.sample \
    --genesis_block=iroha1-cpp/example/genesis.block \
    --keypair_name=iroha1-cpp/example/node0 \
    --overwrite_ledger

.. note:: Сначала адаптируйте ``config.sample`` под своё окружение —
  в частности секцию ``database`` (host/учётные данные PostgreSQL или
  RocksDB) и ``torii_port``. Полный справочник параметров — в
  :doc:`configuration`.

При последующих запусках ``--genesis_block`` и ``--overwrite_ledger``
можно опустить — узел восстановит состояние из block store.

.. note:: ``config.sample`` в репозитории использует PostgreSQL. Чтобы
  работать со встроенным хранилищем RocksDB, укажите ``block_store_path``
  на записываемый каталог и удалите/скорректируйте секцию ``database`` —
  подробности в :ref:`configuration`.
