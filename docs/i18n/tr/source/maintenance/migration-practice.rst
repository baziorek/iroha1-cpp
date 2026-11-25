========================
İyi migrasyon pratikleri
========================

Iroha bakımcıları, projeleri Iroha'nın yeni sürümlerine
taşımanın en iyi yolları hakkında sık sık sorular alıyor, bu
yüzden biz ve projelerimiz için en iyi çalışan pratikleri
paylaşmaya karar verdik.

**4 eşli bir ağ örneğinde, migrasyon prosedürünü (PostgreSQL
veritabanından RocksDB'ye geçişle birlikte) geçeceğiz. Bu
prosedür projelerimizde en kararlı ve güvenilir olanıydı.**

Adımlar şöyle:
**************

1. Eski sürüm Iroha'yı Postgres üzerinde çalıştıran 4 düğümünüz
   var
2. `Iroha Veritabanı Migrasyonu <migration-rocksdb.html>`_
   talimatlarını izleyin. RocksDB klasörünü kopyalayın.
   *Postgres ile RocksDB arasında geçiş yapmanız gerekmiyorsa bu
   adımı atlayın*
3. `Add Peer <add_peer.html>`_ komutunu kullanarak RocksDB
   klasörünü içeren yeni sürüm Iroha'yı çalıştıran yeni bir eş
   ekleyin
4. Benzer düğümleri 2 kez daha ekleyin
5. Artık 7 düğümünüz var — eski sürümle çalışan 4 ve yeni sürümle
   çalışan 3
6. Eski sürümle çalışan ilk düğümü kapatın ve aynı anahtar çiftiyle
   yeni sürüme (hedefiniz RocksDB ise onunla) güncelleyin
7. Eski sürüm Iroha'yı çalıştıran her düğüm için bunu tekrarlayın
8. Artık "yeni" düğümleri kaldırabilir ve projeniz üzerinde
   çalışmaya devam edebilirsiniz!
