======================
PostgreSQL vs. RocksDB
======================

Korzystając z Irohy, masz wybór — zbudować i używać jej z
PostgreSQL (baza relacyjna) lub RocksDB (key-value).

Obie opcje są niezawodne i mogą być używane w środowisku
produkcyjnym, ale istnieją pewne kluczowe różnice, o których
chcemy Ci opowiedzieć, aby pomóc w dokonaniu wyboru.

Specyficzne cechy PostgreSQL:
*****************************

- Klasyczna opcja bazodanowa — co oznacza, że istnieje wiele
  narzędzi do pracy z nią;
- Gdy Iroha działa w Dockerze, PostgreSQL działa w osobnym
  kontenerze;
- Przy dużej ilości danych PostgreSQL może stać się wolniejszy

.. tip:: Możesz dowiedzieć się więcej o tej bazie w jej
  dokumentacji: https://www.postgresql.org/docs/

Specyficzne cechy RocksDB:
**************************

- Szybka (zobacz `wyniki testów wydajności
  <https://wiki.hyperledger.org/display/iroha/Release+1.3.0>`_);
- RocksDB jest osadzona — zarówno WSV, jak i blockstore są w tej
  samej bazie, co oznacza większą spójność, ale istnieje
  możliwość ręcznego dodania hasha z dostępem do bazy, co może
  budzić obawy związane z bezpieczeństwem;
- Zajmuje mniej miejsca na dysku i nie ma informacji, że mogłaby
  urosnąć zbyt duża

.. tip:: Możesz dowiedzieć się więcej o tej bazie w jej
  dokumentacji: https://rocksdb.org/docs/getting-started.html
