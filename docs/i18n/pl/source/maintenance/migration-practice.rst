========================
Dobre praktyki migracji
========================

Opiekunowie Irohy często otrzymują pytania o najlepsze sposoby
migrowania projektów na nowe wersje Irohy, więc postanowiliśmy
podzielić się praktykami, które najlepiej sprawdziły się u nas
i w naszych projektach.

**Na przykładzie sieci 4 peerów przejdziemy przez procedurę
migracji (przy okazji przechodząc z bazy PostgreSQL na RocksDB),
która okazała się najbardziej stabilna i niezawodna w naszych
projektach.**

Oto kroki:
***********

1. Masz 4 węzły uruchamiające Irohę w starej wersji na Postgresie
2. Postępuj zgodnie z instrukcjami `Migracja bazy danych Iroha
   <migration-rocksdb.html>`_. Skopiuj folder RocksDB. *Pomiń ten
   krok, jeśli nie musisz przełączać się między Postgres a RocksDB*
3. Dodaj nowego peera uruchamiającego nową wersję Irohy za pomocą
   komendy `Add Peer <add_peer.html>`_ i z folderem RocksDB
4. Dodaj podobne węzły jeszcze 2 razy
5. Masz teraz 7 węzłów — 4 uruchamiające starą wersję i 3
   uruchamiające nową
6. Wyłącz pierwszy węzeł ze starą wersją i zaktualizuj go tą samą
   parą kluczy do nowej wersji (z RocksDB, jeśli to Twój cel)
7. Powtórz to dla każdego węzła ze starą wersją Irohy
8. Teraz możesz usunąć "nowe" węzły i kontynuować pracę nad
   swoim projektem!
