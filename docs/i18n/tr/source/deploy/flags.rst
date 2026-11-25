.. _deploy-flags:

irohad Bayrakları
=================

``irohad``'i farklı bayraklarla çalıştırabilirsiniz. Ana
bayrakların bir kısmı `zaten açıklanmıştır
<single.html#run-iroha-daemon-irohad>`__, ancak benzersiz
durumunuzda yararlı olabilecek başka bayraklar da vardır. İşte
onlar:

+--------------------------+-------------------------------------------------------------------+------------+---------------+
| Bayrak                   | Açıklama                                                          | Tür        | Varsayılan    |
+==========================+===================================================================+============+===============+
| ``-config``              | Iroha provisioning yolunu belirtir                                | ``string`` | ""            |
| ``-drop_state``          | Başlangıçta mevcut durum verilerini kaldırır                      | ``bool``   | false         |
| ``-genesis_block``       | Başlangıç bloğunu içeren dosyayı belirtir                         | ``string`` | ""            |
| ``-keypair_name``        | .pub ve .priv dosyalarının adını belirtir                         | ``string`` | ""            |
| ``-metrics_addr``        | Prometheus HTTP sunucusunun dinleme adresi                        | ``string`` | "127.0.0.1"   |
| ``-metrics_port``        | Prometheus HTTP sunucusunun dinleme portu, varsayılan devre dışı  | ``string`` | ""            |
| ``-overwrite_ledger``    | Ledger verilerini, varsa üzerine yazar                            | ``bool``   | false         |
| ``-reuse_state``         | Başlangıçta mevcut durum verilerini yeniden kullanmaya çalışır    | ``bool``   | true          |
|                          | (kullanımdan kaldırıldı — başlangıç varsayılan olarak durumu      |            |               |
|                          | yeniden kullanır; WSV'yi kaldırmak için ``drop_state`` kullanın)  |            |               |
| ``-verbosity``           | Günlük ayrıntı düzeyi                                             | ``string`` | "config_file" |
| ``-wait_for_new_blocks`` | Başlangıç senkronizasyon politikası — blok deposundaki yeni       | ``bool``   | false         |
|                          | blokları bekler, ağı çalıştırmaz                                  |            |               |
| ``-exit_after_init``     | WSV'yi blok deposundan yeniden oluşturur ve çıkar; durumun        | ``bool``   | false         |
|                          | onarımı veya yeniden dizinlenmesi için kullanışlıdır              |            |               |
| ``-syncing_node``        | Düğümü syncing modunda çalıştırır: konsensüse katılmadan eşlerden | ``bool``   | false         |
|                          | zincire takip eder (bkz. :ref:`syncing-node`)                     |            |               |
+--------------------------+-------------------------------------------------------------------+------------+---------------+
