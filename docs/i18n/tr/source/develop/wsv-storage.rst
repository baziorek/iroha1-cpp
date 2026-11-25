.. _wsv-storage:

===========================
WSV depolaması (PostgreSQL)
===========================

World State View (WSV), bir Iroha düğümünün veritabanında tuttuğu
durum görünümüdür: hesaplar, varlıklar, işlem ayrıntıları vb.
Fiziksel olarak PostgreSQL'de (eski derlemelerde RocksDB) saklanır
ve Ametsuchi katmanı aracılığıyla erişilebilir hale gelir.

WSV yapısı
----------

WSV, varlık türlerine göre anahtarlanmış birçok tablodan oluşur:

- ``account`` — hesaplar ve bakiyeleri
- ``asset`` — kullanıcı tanımlı dijital varlıklar
- ``tx_status`` — işlem durumları (kesinleşti, reddedildi vb.)
- ``peer`` / ``tx`` / ``command`` / ``query`` — konsensüsü ve
  işleme sırasını destekleyen servis tabloları
- ``mst_*`` — çoklu imza işlemi tabloları

Her kayıt, Iroha komutlarının/sorgularının ikili serileştirmesi
olarak saklandığından, veritabanını doğrudan okuduğunuzda veriler
ikili blob olarak görünür. İnceleme için ``iroha-wsv-checker``
veya Torii üzerinden Iroha sorgularını kullanın.

WSV'yi yeniden indeksleme
-------------------------

Depolama bozulursa veya bir taşıma sonrası düğüm, WSV'yi blok
deposundan yeniden oluşturabilir:

.. code-block:: shell

  irohad --config config.json --genesis_block genesis.block \
         --keypair_name node0 --exit_after_init

``--exit_after_init``, düğümün tüm blokları yeniden uygulamasını
ve çıkmasını sağlar. Ayrıntılar :ref:`deploy-flags`'tedir.

Depolama ile çalışan araçlar
----------------------------

- **iroha-wsv-checker** — sürüm paketleriyle gelir; WSV bütünlüğünü
  kontrol eder, tutarsızlıkları arar
- **wsv_diff** — iki WSV dökümünü karşılaştıran yardımcı program
  (``jq`` ve ``psql`` gerektirir); düğümler arası replikasyonu
  hata ayıklarken kullanışlı

.. hint:: Depo ``example/`` dizininde PostgreSQL tablolarını
tamamlamak için betikler vardır; tam tablo ve alan listesi düğümün
ilk çalıştırmasında Ametsuchi göçleri tarafından üretilir.
