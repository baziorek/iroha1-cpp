.. _node-appimage:

============================
Węzeł Iroha przez AppImage
============================

AppImage to samowystarczalne pliki wykonywalne — bez instalacji i
konfiguracji zależności. Pipeline release'owy Irohy 1 produkuje dwa:

- ``irohad-x86_64.AppImage`` — sam węzeł
- ``iroha-cli-x86_64.AppImage`` — klient wiersza poleceń
  (zobacz :doc:`cli`)

.. note:: AppImage są dołączane do `GitHub Releases
  <https://github.com/baziorek/iroha1-cpp/releases>`__ forka
  społecznościowego począwszy od tagu ``1.6.1``. Dopóki wydanie z nimi
  nie zostanie opublikowane, użyj opcji :ref:`Docker <node-docker>`
  albo :ref:`budowania ze źródeł <node-build>`.

Pobieranie
-----------

Pobierz AppImage dla swojej platformy ze strony
`Releases <https://github.com/baziorek/iroha1-cpp/releases>`__.

Uruchomienie węzła
------------------

AppImage domyślnie nie mają uprawnień wykonywania — nadaj je najpierw:

.. code-block:: shell

  chmod +x irohad-x86_64.AppImage

Węzeł potrzebuje tych samych danych wejściowych co każdy inny build
``irohad``: pliku konfiguracji JSON, genesis blocka i pary kluczy
węzła. Repozytorium zawiera gotowe przykłady w ``example/``
(``config.sample``, ``genesis.block``, ``node0.priv``/``node0.pub``) —
sklonuj forka albo skopiuj ten katalog:

.. code-block:: shell

  git clone -b iroha1-main https://github.com/baziorek/iroha1-cpp --depth=1

Przy pierwszym starcie użyj ``--overwrite_ledger``, żeby genesis block
został zastosowany do pustego storage:

.. code-block:: shell

  ./irohad-x86_64.AppImage \
    --config=iroha1-cpp/example/config.sample \
    --genesis_block=iroha1-cpp/example/genesis.block \
    --keypair_name=iroha1-cpp/example/node0 \
    --overwrite_ledger

.. note:: Najpierw dostosuj ``config.sample`` do swojego środowiska —
  w szczególności sekcję ``database`` (host/dane logowania PostgreSQL
  albo RocksDB) i ``torii_port``. Pełną referencję parametrów znajdziesz
  w :doc:`configuration`.

Przy kolejnych startach możesz pominąć ``--genesis_block`` i
``--overwrite_ledger`` — węzeł przywraca stan z block store.

.. note:: ``config.sample`` w repozytorium używa PostgreSQL. Aby
  uruchomić z wbudowanym storage RocksDB, wskaż ``block_store_path`` na
  zapisywalny katalog i usuń/dostosuj sekcję ``database`` — szczegóły
  w :ref:`configuration`.
