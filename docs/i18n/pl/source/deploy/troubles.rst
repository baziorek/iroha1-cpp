.. _deploy_troubles:

=======================
Rozwiązywanie problemów
=======================

—„Proszę, pomóżcie mi, bo ja…"

Nie mam binarki demona Iroha
-----------------------------

Możesz zbudować binarkę demona Iroha ze źródeł. Binarki możesz
pobrać `tutaj <https://github.com/baziorek/iroha1-cpp/releases>`__

Nie mam pliku konfiguracyjnego
------------------------------

Sprawdź, jak utworzyć plik konfiguracyjny, podążając za tym
`linkiem <../configure/index.html>`__

Nie mam genesis block
---------------------

Utwórz genesis block, generując go przez `iroha-cli` lub ręcznie,
używając `przykładu
<https://github.com/baziorek/iroha1-cpp/blob/iroha1-main/example/genesis.block>`__
i sprawdzając `uprawnienia <../develop/api/permissions.html>`__

Nie mam pary kluczy dla peera
-----------------------------

Aby utworzyć parę kluczy dla konta lub peera, użyj binarki
iroha-cli, przekazując nazwę peera z opcją `--new_account`.
Na przykład:

.. code-block:: shell

    ./iroha-cli --account_name newuser@test --new_account
