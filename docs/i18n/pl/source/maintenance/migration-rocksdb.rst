====================
Migracja do RocksDB
====================

Iroha pozwala używać Postgres lub Rocks Database.
Możesz używać opcji, którą preferujesz, a jeśli chcesz
przełączyć się — jest dla Ciebie opcja.
**Po prostu zmigruj swoją bazę danych!**
Oto jak

.. hint:: Zarówno narzędzie migracji, jak i WSV checker mają
   ``-help``, który możesz wywołać, aby w dowolnym momencie
   sprawdzić dostępne flagi.

Jak migrować
============

Po zbudowaniu Irohy (lub po prostu pobraniu `obrazu Docker
<https://hub.docker.com/r/hyperledger/iroha>`_) w wersji 1.3
(lub nowszej) masz już narzędzia migracji jako osobny
wykonywalny plik! Domyślnie, po zbudowaniu, Iroha przechowuje
binarki w ``BUILD_DIR/bin/``.

Po prostu uruchom ``iroha_migrate`` z następującymi flagami:

- ``-help`` - pomoc
- ``-block_store_path`` - określa ścieżkę do block store.
  Domyślnie: "/tmp/block_store"
- ``-export`` - eksportuje block store do określonego katalogu,
  domyślnie — bieżący katalog roboczy (CWD). Użyj go do
  *odwrotnej* migracji do RocksDB (eksportując pliki z niej).
- ``-drop_state`` - użyj go, aby nadpisać bloki w blockstore
  RocksDB, jeśli już istnieje. Może to być bardzo przydatne, jeśli
  następny krok — sprawdzenie poprawności zmigrowanej bazy —
  przejdzie z błędami i będziesz musiał powtórzyć proces migracji.
  Domyślnie: false
- ``-rocksdb_path`` - określa ścieżkę do RocksDB. Domyślnie:
  "rocks.db"

.. raw:: html

   <details>

   <summary>W przypadku sukcesu zobaczysz <b>tę wiadomość</b></summary>

.. code-block:: bash

	Success! WSV in RocksDB was build.
	Next step check consintancy with Postgres WSV using iroha_wsv_diff.

.. raw:: html

   </details>
	<br/><div style="line-height: 0; padding: 0; margin: 1"></div>

Jeśli migracja się nie powiedzie, zakończy się z niezerowym kodem.
W tym przypadku sprawdź wszystkie flagi i spróbuj ponownie.


I... Twoja baza danych jest zmigrowana! Ale to nie wszystko.

Aby upewnić się, że proces migracji powiódł się, użyj
następnie sprawdzenia WSV.

Jak sprawdzić WSV po migracji
=============================

Uruchom ``iroha_wsv_diff`` z następującymi flagami:

- ``-help`` - pomoc
- ``-pg_opt`` - określa linię opcji Postgres. Powinna być taka
  jak w Twojej konfiguracji. Domyślnie:
  "dbname=iroha_default host=localhost port=5432 user=postgres
  password=postgres"
- ``-rocksdb_path`` - określa ścieżkę do RocksDB. Domyślnie:
  "rocks.db"

Jeśli bazy są takie same, zobaczysz Pana Spocka. Również (jeśli
nie jesteś wielkim fanem science fiction) kod wyjścia będzie
zerowy.

.. raw:: html

   <details>

   <summary>Pomyślne sprawdzenie da <b>coś takiego</b></summary>

.. code-block:: bash

   Reading rocksdb... in 7112ms
	Reading postgres... in 5923ms
	See detailed dumps in files rockdb.wsv and postgres.wsv
	== VALIDATING ==
	left is rocksdb, right is postgres
	░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▒▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▒▒▒▓▓██████████▓▓▒░░░░░░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▒▓██████████████████▓▒░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▒███████████▓▓▓███▓▓▓▓█▓░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▓███████████████▓▓▓▓█████▒░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▒████████▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▓█▓░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░░░░░░▒░░░░░░░░░░░░░░░░░░░░░░░░░░░▒█████████▓▓▓▒▒▒▒▒▒▒░▒▒▒▒▓▓░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░░░░░▓▒▒░░░░▓▒▒▒░░░░░░░░░░░░░░░░░▒▓█████████▓████▓▒▒▒▓▓▓▓▓▒▓▒░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░░░░▒▓▒▒░░░▒▒▒▒▒░░░░░░░░░░░░░░░░░▓████████▓█▓▓▒▓██▒▒▒▓▓▓▓▓▒▓▒░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░░▒▒▒▒▒▒░░▒▒▒▒▒▒░░░░░░░░░░░░░░░░▓███████▓▓▒▒▒▒▓██▒▒▒▒▒▒▒▒▒▓░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░░▓▒▒▒▒▒░░▒▒▒▒▒▒░░░░░░░░░░░░░░░░▓███████▓▓▒▒████▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░▒▓▒▒▒▒▒░▓▒▒▒▒▒▒░░░░░░░░░░░░░░░░▒████████▓▓██████▓▓▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░▒▒▒▒▒▒▒▓▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░████████████▓▓▓▓▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░▓▒▒▒▒▒▒▒▒▒▒▓▒▒▒░░░░░░░░░░░░░░░░░▒███████▓▓▓▓▓▓▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░░▓▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░██████▓▓██▓▓▓▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░▓█████▓▓▓▓▓▓▓▓▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░▒▒▒▒░░░░░░░░░░░▓██████▓▓▓▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░▒▓▓▒▒░░░░░░░░░░░▓███████▓▓▓▓▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░░▒████████████▓▓▒▒▒░░░░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░░▒▒███████████▓▓▒▒▒▒▒▒▓░░░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░█▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░▒▒▒▒▓▓█████████▓▓▓▓▓▓▓▓██▓░░░░░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░░█▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░▒▓█████████▓██████████████████▓▓▒▒▒░░░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░▒██▓▓▓▓▓▓▓▓▓▒▒▒░░░░▒▒▒▒▓▓▒▒▒▒▓▓▓▓▓▓▓▓█▓█████████████▓▒▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░
	░░░░░░░░░░░░░▓▓▓▓▓▓▓▓▓▓▓▒░░░░▒▒▓▒▒▒░▒▒▓▓▓▓▓▓▓▓▓▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░
	░░░░░░░░░░▒█▓▓▓▓▓▓▒▒▒▓░░░▒▒▒▒▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░
	░░░░░░░░▒████▓▓▓▒▒▒▓▒░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░
	░░░░░░░▒██████▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░
	░░░░░░▒██████▓▓▓▓▓▓▓▒▒▒▒▒▒▒▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░
	░░░░░░██████▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░
	░░░░░░██████▓▓▓▒▓▓▒▒▓▓▒▒▒▓█▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░
	░░░░░▒█████▓▓▓▓▓▓▓▓█▓▓▒▒▒▒▒▒▓█▓▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░
	~~~ WSV-s are same. Enjoy Iroha with RocksDB ~~~




.. raw:: html

   </details>
	<br/><div style="line-height: 0; padding: 0; margin: 1"></div>

Jeśli nie, będą różnice w bazach danych: dane po lewej to to, co
jest wskazane w RocksDB, a po prawej — co jest w Postgres.

.. raw:: html

   <details>

   <summary>Nieudane sprawdzenie da <b>coś takiego</b></summary>

.. code-block:: bash

	Reading rocksdb... in 6990ms
	Reading postgres... in 5652ms
	See detailed dumps in files rockdb.wsv and postgres.wsv
	== VALIDATING ==
	left is rocksdb, right is postgres
	Role-s 'client' have different permissions: '00000000000001110100100100000100100100011010111010000' and '00000000000001110100100100000100100100011010111010011'
	Wsv-s have different roles.
	AssetQuantity-s 'test#test' have different quantity: '0.0' and '1234567.0'
	Accounts 'superuser@bootstrap' have different assetsquantity
	Domains 'bootstrap' have different accounts.
	Wsv-s have different domains.
	~~~ WSV-s DIFFER!!! ~~~
	For future investigation use difftool on files rocksdb.wsv and postgres.wsv. Just like:
   		diff <(tail -n+2 postgres.wsv) <(tail -n+2 rockdb.wsv)




.. raw:: html

   </details>
	<br/><div style="line-height: 0; padding: 0; margin: 1"></div>

Jeśli są różnice, sugerujemy ponowne użycie narzędzia migracji z
flagą ``-drop_state``.

W przypadku rozbieżności polecenie zakończy się niezerowym
kodem. Różnice zostaną zgłoszone do konsoli, a pełne WSV obu baz
danych zostaną zapisane do odpowiednich plików (wynik w postaci
``postgres.wsv`` i ``rocksdb.wsv`` będzie w bieżącym katalogu
roboczym (CWD)).
Do przyszłych badań możesz użyć dowolnego narzędzia diff, aby
zobaczyć dokładne różnice między WSV.
