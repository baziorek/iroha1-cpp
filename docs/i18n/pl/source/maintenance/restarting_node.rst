.. raw:: html

    <style> .red {color:#aa0060; font-weight:bold; font-size:16px} </style>

.. role:: red

Restartowanie węzła Iroha z istniejącym WSV
=============================================

Wcześniej, w przypadkach gdy trzeba było zaktualizować węzeł lub
został on z jakiegoś powodu wyłączony, istniała tylko jedna
opcja — ponowne odczytanie wszystkich bloków, aby odtworzyć
spójny `world state view (zwany też WSV)
<../concepts_architecture/architecture.html#world-state-view>`__.
Aby uruchomić węzeł szybciej, teraz możliwe jest ponowne
użycie istniejącej bazy WSV po szybkiej weryfikacji.
W tym celu ``hash`` górnego bloku i ``height`` blockstorage są
zawarte w WSV.

.. warning::
	To na administratorach węzła spoczywa obowiązek upewnienia
	się, że WSV nie jest edytowane ręcznie – tylko przez Irohę
	lub `skrypt migracyjny <#changing-iroha-version-migration>`__.
	Ręczna edycja lub edycja skryptu migracyjnego niezgodna z
	wiarygodnymi wytycznymi może prowadzić do niespójnej sieci.
	Rób to na własne ryzyko (ostrzegaliśmy).

Choć może to być świetny pomysł w niektórych przypadkach,
proszę wziąć pod uwagę, że istnieją pewne specyfiki ponownego
użycia WSV w porównaniu z jego odtworzeniem z blockstorage:

| :red:`Punkt zaufania`
| **Ponowne użycie WSV:** musimy polegać zarówno na
  blockstorage, jak i WSV.
| **Odtworzenie WSV z block storage:** ufamy tylko blokowi
  genesis.


| :red:`Integralność`
| **Ponowne użycie WSV:** blockstorage i WSV muszą do siebie
  pasować! Iroha nie będzie tego sprawdzać.
| **Odtworzenie WSV z block storage:** Iroha sprawdzi każdy
  blok podczas odtwarzania WSV.
	Każdy błąd w blockstorage zostanie znaleziony (oczywiście
	oprócz bloku genesis).
	WSV jest gwarantowane, że pasuje do blockstorage.

| :red:`Czas`
| **Ponowne użycie WSV:** Iroha jest niemal natychmiast gotowa
  do działania w sieci.
| **Odtworzenie WSV z block storage:** im większy blockstorage,
  tym dłużej trwa jego odtworzenie i rozpoczęcie działania.

.. note:: Jeśli lokalny ledger, który się wyłączył, ma więcej
bloków niż powinien, a poprawny WSV jest wśród nich — to w
porządku, Iroha weźmie WSV poprawnego bloku.
	Jeśli bloków jest mniej niż powinno — opcja ponownego
	użycia WSV ci nie zadziała.
	Proszę odtwórz je z bloków.


Porzucanie WSV
^^^^^^^^^^^^^^

Domyślnie Iroha ponownie używa stanu WSV przy starcie, więc
flaga `--reuse_state` nie jest już potrzebna. Została jednak dla
zgodności wstecznej.
Jeśli chcesz porzucić stan WSV, uruchom Irohę z flagą
``--drop_state``. Przy tej fladze Iroha zresetuje i nadpisze
bazę stanu.

Wersja schematu bazy stanu
^^^^^^^^^^^^^^^^^^^^^^^^^^

Przy ponownym użyciu istniejącego WSV Iroha wykonuje sprawdzenie
zgodności wersji schematu. Nie uruchomi się ani nie zmieni
bazy w żaden sposób, jeśli jej schemat nie jest zgodny z
używaną Irohą.

Jeśli Twój schemat został utworzony przez Irohę w wersji
v1.1.1 lub niższej, najprawdopodobniej nie zawiera informacji o
wersji. W tym przypadku musisz dodać ją ręcznie.
Zachęcamy do użycia naszego skryptu do tego celu — znajduje
się `tutaj
<https://github.com/hyperledger-iroha-state-migration-tool/blob/master/state_migration.py>`__.
Aby wymusić (tj. bez żadnego `procesu migracji
<#changing-iroha-version-migration>`__) ustawienie wersji
schematu, uruchom skrypt z flagą `--force_schema_version` i
przekaż wersję binarki Iroha, która została użyta do utworzenia
Twojego schematu.

.. warning::
  Przed wymuszeniem zapisu numerów wersji schematu, podwójnie
  sprawdź wersję irohad, która utworzyła schemat.
  Żadne sprawdzenia nie są wykonywane przy wymuszaniu numerów
  schematu, dlatego łatwo jest zepsuć bazę stanu w przyszłości
  (podczas następnej migracji).

Zmiana wersji Iroha. Migracja.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
W przypadku chęci zmiany wersji Iroha z zachowaniem WSV,
zachęcamy do przeprowadzenia migracji.
Choć może być to niepotrzebne (Iroha odmówi uruchomienia, jeśli
schemat jest niezgodny), jako ogólna zasada ulepszamy schemat
z każdą wersją, a migracja może być dobrym pomysłem dla lepszej
wydajności.
Zachęcamy do wykonania kopii zapasowej bazy przed migracją
przy użyciu standardowych `wytycznych PostgreSQL
<https://www.postgresql.org/docs/current/backup.html>`__.

Aby przeprowadzić migrację, użyj naszego `skryptu
<https://github.com/hyperledger-iroha-state-migration-tool/blob/master/state_migration.py>`__.

Załaduje on informacje o schemacie z bazy i dopasuje je do
kroków migracji (domyślnie scenariusze migracji są zdefiniowane
w katalogu ``migration_data`` w tym samym folderze co skrypt).
Następnie znajdzie wszystkie ścieżki migracji, które przeprowadzą
Twoją bazę do żądanej wersji, i poprosi Cię o wybranie jednej.

.. seealso::
	`Tutaj
	<https://github.com/hyperledger-iroha-state-migration-tool/blob/master/README.md>`__
	znajdują się szczegóły o różnych przypadkach migracji i
	przykłady, które możesz sprawdzić, aby przeprowadzić
	migrację

Tryb synchronizacji WSV.
^^^^^^^^^^^^^^^^^^^^^^^

Określ opcje ``--wait_for_new_blocks`` dla trybu synchronizacji
WSV. Iroha odtwarza WSV z blockstore i czeka na dodanie nowych
bloków z zewnątrz. W tym trybie Iroha nie będzie wykonywać
operacji sieciowych.
