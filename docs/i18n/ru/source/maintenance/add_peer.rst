================
Добавление пиров
================

В HL Iroha вы можете добавлять новых пиров в сеть, когда она уже
работает. Это делается с помощью специальной команды `AddPeer
<../develop/api/commands.html#add-peer>`_.

Требования
==========

**Должен существовать пир, который:**

— работает с Genesis Block (начальным блоком блокчейна),
идентичным тому, что на пирах, уже находящихся в сети;

— имеет разрешимый адрес;

— имеет пару ключей пира (Ed25519 с SHA-2/SHA-3)

.. important:: Аккаунт, отправляющий транзакцию добавления пира,
   должен иметь разрешение `Can Add Peer
   <../develop/api/permissions.html#can-add-peer>`_, а для удаления
   пира — разрешение `Can Remove Peer
   <../develop/api/permissions.html#can-remove-peer>`_ в своей роли —
   это должно быть установлено в genesis block.

Использование
=============

Как описано в `справочнике API
<../develop/api/commands.html#add-peer>`_, для использования
команды вам понадобится только:

— публичный ключ пира, который вы хотите добавить в сеть;

— разрешимый IP-адрес пира

Шаги:

1. Создайте сеть с разрешениями `Can Add Peer
   <../develop/api/permissions.html#can-add-peer>`_ и `Can
   Remove Peer
   <../develop/api/permissions.html#can-remove-peer>`_,
   установленными в genesis block, назначенном пользователю;
2. Создайте ещё одного пира, запускающего HL Irohу с тем же
   genesis block и похожей конфигурацией;
3. Отправьте транзакцию с аккаунта с необходимыми разрешениями,
   содержащую команду ``add peer`` (см. пример ниже)
4. Проверьте логи пиров, чтобы убедиться, что всё работает
   корректно.

Вы также можете убедиться, что всё в порядке, отправив
транзакцию и проверив, совпадает ли количество блоков на узлах.

.. note:: Если работает только 1 существующий пир, вам нужно будет
   настроить добавляемых пиров так, чтобы они имели всех пиров (как
   существующих, так и новых) в параметре "initial_peers" в
   `конфигурации
   <../configure/index.html#environment-specific-parameters>`_.
   Другой случай, когда это нужно — когда сеть работает уже некоторое
   время и пиры, указанные в genesis block, больше не существуют
   (потому что они были удалены командой Remove Peer, когда
   добавлялись новые пиры).

Пример
======

Вот как команда может выглядеть на Python.
В этом примере мы использовали `разрешение Root
<../develop/api/permissions.html#root>`_, которое имеет все
разрешения, включая `Can Add Peer
<../develop/api/permissions.html#can-add-peer>`_ и `Can Remove
Peer <../develop/api/permissions.html#can-remove-peer>`_:

.. code-block:: python

    def add_peer():
        peer1 = primitive_pb2.Peer()
        peer1.address = '192.168.1.1:50541'
        peer1.peer_key = '716fe505f69f18511a1b083915aa9ff73ef36e6688199f3959750db38b8f4bfc'
        tx = iroha.transaction([
            iroha.command('AddPeer', peer=peer1)
        ], creator_account=ADMIN_ACCOUNT_ID, quorum=1)

        IrohaCrypto.sign_transaction(tx, ADMIN_PRIVATE_KEY)

    add_peer()

Удаление пира
=============

Чтобы удалить пира, нужно использовать команду `Remove Peer
<../develop/api/commands.html#remove-peer>`_ с аккаунта, у
которого есть разрешение ``CanRemovePeer``.
