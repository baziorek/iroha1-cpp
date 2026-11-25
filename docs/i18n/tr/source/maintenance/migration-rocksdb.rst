====================
RocksDB
====================

IrohaPostgresRocks Database'ini kullanmanıza olanak tanır.
Tercih ettiğinizi kullanabilirsiniz ve geçmek isterseniz
sizin için bir seçenek de var.
**Veritabanınızı taşımanız yeterli!**
İşte nasıl

.. hint:: Hem taşıma aracı hem de WSV denetleyicisinin
   istediğiniz zaman mevcut bayrakları kontrol etmek için
   çağırabileceğiniz ``-help`` seçeneği vardır.

Nasıl taşınır
=============

Iroha'yı derledikten sonra (veya sadece
<https://hub.docker.com/r/hyperledger/iroha>`_ adresinden
Docker imajını çekerek) 1.3 (veya daha yeni) sürümünde
taşıma araçlarına zaten ayrı bir çalıştırılabilir dosya
olarak sahipsiniz! Varsayılan olarak derleme sonrası Iroha
ikili dosyalarını ``BUILD_DIR/bin/`` altında saklar.

Şu bayraklarla ``iroha_migrate`` komutunu çalıştırmanız
yeterli:

- ``-help`` - yardım
- ``-block_store_path`` - block store yolunu belirtir.
  Varsayılan: "/tmp/block_store"
- ``-export`` - block store'u belirtilen dizine aktarır;
  varsayılan olarak geçerli çalışma dizini (CWD). Bunu
  RocksDB'ye *geri* taşınırken (dosyaları ondan dışa
  aktararak) kullanın.
- ``-drop_state`` - RocksDB blockstore'unda bloklar zaten
  varsa üzerine yazmak için kullanın. Taşınan veritabanının
  doğruluğunu kontrol eden sonraki adım hatalarla
  sonuçlanırsa ve taşıma işlemini tekrarlamanız gerekirse
  çok yararlı olabilir. Varsayılan: false
- ``-rocksdb_path`` - RocksDB yolunu belirtir. Varsayılan:
  "rocks.db"

.. raw:: html

   <details>

   <summary>Başarılı olursa <b>bu mesajı</b> görürsünüz</summary>

.. code-block:: bash

	Success! WSV in RocksDB was build.
	Next step check consintancy with Postgres WSV using iroha_wsv_diff.

.. raw:: html

   </details>
	<br/><div style="line-height: 0; padding: 0; margin: 1"></div>

Taşıma başarısız olursa sıfır olmayan bir kodla sonlanır. Bu
durumda tüm bayrakları kontrol edip tekrar deneyin.


Ve... veritabanınız taşındı! Ama bu her şey değil.

Taşıma işleminin başarılı olduğundan emin olmak için
daha sonra WSV kontrolünü kullanın.

Taşıma sonrası WSV nasıl kontrol edilir
=======================================

``iroha_wsv_diff`` komutunu şu bayraklarla çalıştırın:

- ``-help`` - yardım
- ``-pg_opt`` - Postgres seçenek satırını belirtir. Yapılandırmanızdaki
  ile aynı olmalıdır. Varsayılan:
  "dbname=iroha_default host=localhost port=5432 user=postgres
  password=postgres"
- ``-rocksdb_path`` - RocksDB yolunu belirtir. Varsayılan:
  "rocks.db"

Veritabanları aynıysa Bay Mr. Spock'u görürsünüz. Ayrıca
(bilim kurgu hayranı değilseniz) çıkış kodu sıfır olur.

.. raw:: html

   <details>

   <summary>Başarılı kontrol <b>şuna benzer bir şey</b> verir</summary>

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

Hayırsa veritabanları arasında farklar olur: soldaki veriler
RocksDB'de belirtilen, sağdaki ise Postgres'teki veriler olur.

.. raw:: html

   <details>

   <summary>Başarısız kontrol <b>şuna benzer bir şey</b> verir</summary>

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

Farklar varsa taşıma aracını ``-drop_state`` bayrağıyla tekrar
kullanmanızı öneririz.

Tutarsızlık durumunda komut sıfır olmayan bir kodla sonlanır.
Farklar konsola bildirilir ve her iki veritabanının tam WSV'si
ilgili dosyalara kaydedilir (``postgres.wsv`` ve
``rocksdb.wsv`` sonucu geçerli çalışma dizininde (CWD)
olacaktır). Gelecekteki incelemeler için WSV'ler arasındaki
kesin farkları görmek üzere herhangi bir diff aracını
kullanabilirsiniz.
