.. _node-appimage:

============================
AppImage ile Iroha düğümü
============================

AppImage'ler bağımsız çalıştırılabilir dosyalardır — kurulum veya
dependencies yapılandırması gerekmez. Iroha 1'in sürüm hattı ikisini
de üretir:

- ``irohad-x86_64.AppImage`` — düğümün kendisi
- ``iroha-cli-x86_64.AppImage`` — komut satırı istemcisi
  (bkz. :doc:`cli`)

.. note:: AppImage'ler, topluluk çatallanmasının `GitHub Releases
  <https://github.com/baziorek/iroha1-cpp/releases>`__ sayfasına
  ``1.6.1`` etiketinden itibaren eklenir. Bunları içeren bir sürüm
  yayınlanana kadar :ref:`Docker <node-docker>` veya
  :ref:`kaynak koddan derleme <node-build>` seçeneklerini
  kullanın.

İndirme
-------

Platformunuz için AppImage'i `Releases
<https://github.com/baziorek/iroha1-cpp/releases>`__ sayfasından
download edin.

Düğümü çalıştırma
-----------------

AppImage'ler varsayılan olarak çalıştırılabilir değildir — önce
dosyayı çalıştırılabilir yapın:

.. code-block:: shell

  chmod +x irohad-x86_64.AppImage

Düğüm, diğer ``irohad`` derlemeleriyle aynı girdilere ihtiyaç duyar:
JSON yapılandırma dosyası, genesis block ve düğüm anahtar çifti.
Depoda ``example/`` altında hazır örnekler var (``config.sample``,
``genesis.block``, ``node0.priv``/``node0.pub``) — çatallanmayı
klonlayın veya bu dizini kopyalayın:

.. code-block:: shell

  git clone -b iroha1-main https://github.com/baziorek/iroha1-cpp --depth=1

İlk çalıştırmada, genesis block'un boş depolamaya uygulanması için
``--overwrite_ledger`` kullanın:

.. code-block:: shell

  ./irohad-x86_64.AppImage \
    --config=iroha1-cpp/example/config.sample \
    --genesis_block=iroha1-cpp/example/genesis.block \
    --keypair_name=iroha1-cpp/example/node0 \
    --overwrite_ledger

.. note:: Önce ``config.sample``'ı ortamınıza göre uyarlayın —
  özellikle ``database`` bölümünü (PostgreSQL host/kimlik bilgileri
  veya RocksDB) ve ``torii_port``'u. Tam parametre referansı
  :doc:`configuration`'dadır.

Sonraki çalıştırmalarda ``--genesis_block`` ve
``--overwrite_ledger`` atlanabilir — düğüm durumu blok deposundan
geri yükler.

.. note:: Depodaki ``config.sample`` PostgreSQL kullanır. Yerleşik
  RocksDB depolamasını kullanmak için yazılabilir bir dizini
  ``block_store_path`` olarak belirtin ve ``database`` bölümünü
  kaldırın veya düzeltin — ayrıntılar :ref:`configuration`'da.
