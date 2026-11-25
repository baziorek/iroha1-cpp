.. _getting-started:

=================
Быстрый старт
=================

Чтобы пользоваться Hyperledger Iroha, вам нужны две вещи:

#. **Как минимум один узел Iroha** — работающий процесс ``irohad``,
   который создаёт (или присоединяется к) сеть.
#. **Способ отправки транзакций** — клиент, общающийся с узлом через
   gRPC: инструмент ``iroha-cli``, одна из клиентских библиотек или
   собственный код.

Это руководство проведёт вас через оба элемента. Сначала выберите, как
получить узел Iroha, а затем — как с ним общаться.

.. note:: Ledger — синоним блокчейна, а Hyperledger Iroha также известна
  как фреймворк Distributed Ledger Technology — что по сути то же
  самое, что «блокчейн-фреймворк». Остальную терминологию вы найдёте в
  разделе :ref:`core-concepts`.

.. toctree::
   :maxdepth: 2
   :caption: Получить узел Iroha

   node-docker.rst
   node-appimage.rst
   node-deb.rst
   node-build.rst

.. toctree::
   :maxdepth: 2
   :caption: Инструмент общения — ``iroha-cli``

   cli.rst

.. toctree::
   :maxdepth: 2
   :caption: Клиентские библиотеки

   cli-guide.rst
   python-guide.rst

.. toctree::
   :maxdepth: 2
   :caption: Конфигурация узла

   configuration.rst
