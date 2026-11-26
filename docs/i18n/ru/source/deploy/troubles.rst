.. _deploy_troubles:

=====================
Решение проблем
=====================

—«Пожалуйста, помогите, потому что я…»

Нет бинарника демона Iroha
---------------------------

Вы можете собрать бинарник демона Iroha из исходников. Бинарники
можно получить `здесь
<https://github.com/baziorek/iroha1-cpp/releases>`__

Нет файла конфигурации
----------------------

Посмотрите, как создать файл конфигурации, перейдя по этой
`ссылке <../configure/index.html>`__

Нет genesis block
-----------------

Создайте genesis block, сгенерировав его через `iroha-cli` или
вручную, используя `пример
<https://github.com/baziorek/iroha1-cpp/blob/iroha1-main/example/genesis.block>`__
и проверив `разрешения
<../develop/api/permissions.html>`__

Нет пары ключей для пира
------------------------

Чтобы создать пару ключей для аккаунта или пира, используйте
бинарник iroha-cli, передав имя пира с опцией `--new_account`.
Например:

.. code-block:: shell

    ./iroha-cli --account_name newuser@test --new_account
