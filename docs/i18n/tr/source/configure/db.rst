======================
PostgreSQL vs. RocksDB
======================

Iroha'yı kullanırken PostgreSQL (ilişkisel veritabanı) veya
RocksDB (key-value) ile derleyip kullanmayı seçebilirsiniz.

Her iki seçenek de güvenilirdir ve üretim ortamında
kullanılabilir, ancak seçim yapmanıza yardımcı olacak bazı
önemli farklar vardır.

PostgreSQL'e özgü özellikler:
*****************************

- Klasik veritabanı seçeneği — yani onunla çalışmak için birçok
  araç mevcut
- Iroha Docker'da çalışırken PostgreSQL ayrı bir konteynerde
  çalışır
- Çok fazla veri olduğunda PostgreSQL daha yavaş olabilir

.. tip:: Bu veritabanı hakkında daha fazlasını belgelerinde
  öğrenebilirsiniz:
  https://www.postgresql.org/docs/

RocksDB'ye özgü özellikler:
***************************

- Hızlıdır (bkz. `performans testi sonuçları
  <https://wiki.hyperledger.org/display/iroha/Release+1.3.0>`_);
- RocksDB gömülüdür — hem WSV hem de blockstore aynı veritabanında
  olduğundan daha fazla tutarlılık sağlar, ancak veritabanına
  erişimle hash'in manuel olarak eklenmesi mümkün olabilir ki bu
  güvenlikle ilgili endişeler uyandırabilir;
- Diskte daha az yer kaplar ve fazla büyüyeceğine dair bilgi yok

.. tip:: Bu veritabanı hakkında daha fazlasını belgelerinde
  öğrenebilirsiniz:
  https://rocksdb.org/docs/getting-started.html
