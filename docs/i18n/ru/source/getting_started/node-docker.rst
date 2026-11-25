.. _node-docker:

==============================
Запуск узла Iroha через Docker
==============================

Этот гайд проведёт через развёртывание ``irohad`` — самого узла — в
Docker на локальной машине. После выполнения этих шагов у вас будет
запущенный узел, к которому можно отправлять транзакции через
``iroha-cli`` (см. :doc:`cli`) или одну из клиентских библиотек (см.
:doc:`../develop/libraries`).

.. note:: Этот гайд относится к форку сообщества
  `baziorek/iroha1-cpp <https://github.com/baziorek/iroha1-cpp>`_.

Перед началом
-------------

Убедитесь, что у вас установлены и запущены:

- `Docker <https://docs.docker.com/get-docker/>`_ (Docker Desktop на
  macOS/Windows или Docker Engine на Linux)
- `docker compose <https://docs.docker.com/compose/>`_ (обычно
  включён в Docker Desktop; на Linux может понадобиться отдельная
  установка)
- `git <https://git-scm.com/downloads>`_, чтобы склонировать
  репозиторий и получить пример конфигурации
- (Опционально) `Python
  <https://www.python.org/downloads/>`_ — если решите настроить
  удобные алиасы команд

1. Клонируйте репозиторий
-------------------------

Склонируйте репозиторий форка сообщества и перейдите в каталог
``example/``:

.. code-block:: shell

  git clone -b iroha1-main https://github.com/baziorek/iroha1-cpp
  cd iroha1-cpp/example

Здесь лежат все файлы, которые нужны для первого запуска узла:
``config.docker`` (конфигурация для Docker-образа),
``genesis.block``, пара ключей ``node0`` и так далее.

2. Запустите PostgreSQL и ноду Iroha
------------------------------------

В ``example/`` находится готовый ``docker-compose.yml``, который
поднимает три сервиса: PostgreSQL (хранилище Iroha), один узел Iroha и
порт-форвардер Nginx для Torii:

.. code-block:: shell

  docker compose up

В первый раз Docker скачает и соберёт образ ``irohad`` и будет это
делать несколько минут. Дождитесь, пока контейнеры отчитаются
``healthy``, затем проверьте, что node отвечает:

.. code-block:: shell

  docker compose ps

Ожидаемый вывод — три сервиса со статусом ``running`` или ``healthy``.

3. Отправьте первую транзакцию
------------------------------

В одном из запущенных контейнеров уже есть ``iroha-cli``. Откройте
интерактивную сессию:

.. code-block:: shell

  docker exec -it iroha iroha-cli -account_name admin@test

По умолчанию CLI предлагает доступные команды. Чтобы увидеть все
опции, запустите ``iroha-cli -h`` внутри контейнера или внутри
запущенного образа.

Подробное руководство по ``iroha-cli`` — отправка транзакций и
запросов — в разделе :doc:`cli-guide`.

4. Следующие шаги
-----------------

- :doc:`configuration` — подробный разбор параметров конфигурации
- :doc:`../deploy/single` — запуск на реальном (не Docker) сервере
- :doc:`../develop/libraries/cpp` — написание собственного приложения
  на C++ с ``iroha-lib``

Остановка узла
--------------

Когда закончите, остановите контейнеры:

.. code-block:: shell

  docker compose down

Чтобы удалить также и данные PostgreSQL, используйте:

.. code-block:: shell

  docker compose down -v
