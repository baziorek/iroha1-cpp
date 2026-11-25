.. _node-deb:

=================================
Узел Iroha из релизных пакетов
=================================

Помимо Docker-образов, AppImage и сборок из исходников, релизный
pipeline Iroha 1 прикрепляет нативные пакеты к `GitHub Releases
<https://github.com/baziorek/iroha1-cpp/releases>`__ форка
сообщества:

- ``iroha_*.deb`` — пакет Debian/Ubuntu, собранный CPack; содержит
  ``irohad`` и инструменты обслуживания (migration tool, WSV checker)
- ``iroha-*.rpm`` — аналог для RPM-дистрибутивов, когда публикуется
- ``irohad-x86_64.AppImage`` / ``iroha-cli-x86_64.AppImage`` — см.
  :doc:`node-appimage`

.. note:: Пакеты появляются на странице Releases начиная с тега
  ``1.6.1``. До этого используйте варианты :ref:`Docker <node-docker>`
  или :ref:`сборки из исходников <node-build>`.

Установка пакета .deb
---------------------

Скачайте файл ``.deb`` со страницы Releases, затем установите его через
``apt`` (который также разрешит зависимости):

.. code-block:: shell

  sudo apt install ./iroha_*.deb

или напрямую через ``dpkg``:

.. code-block:: shell

  sudo dpkg -i iroha_*.deb
  # если apt сообщит об отсутствующих зависимостях:
  sudo apt -f install

Пакет устанавливает бинарники системно (префикс CPack по умолчанию
``/usr``). Проверьте:

.. code-block:: shell

  which irohad
  irohad --help

Установка пакета .rpm
---------------------

На RPM-дистрибутивах (Fedora, RHEL, openSUSE):

.. code-block:: shell

  sudo dnf install ./iroha-*.rpm
  # или: sudo rpm -i iroha-*.rpm && sudo dnf -f install

Запуск узла
-----------

Упакованный ``irohad`` принимает те же аргументы, что и любая другая
сборка — файл конфигурации, genesis block и имя пары ключей:

.. code-block:: shell

  sudo irohad --config /etc/iroha/config.json \
              --genesis_block /etc/iroha/genesis.block \
              --keypair_name /etc/iroha/node0

В комплекте нет конфигурации — скопируйте примеры из каталога
``example/`` репозитория (см. :doc:`configuration`) и адаптируйте под
своё окружение, включая секцию ``database`` (PostgreSQL или RocksDB),
которая должна быть доступна до запуска узла.

.. note:: Упакованному узлу по-прежнему нужны PostgreSQL (или RocksDB)
  и — для продакшен-развёртывания — терминация TLS перед Torii; см.
  :doc:`../configure/torii-tls`.
