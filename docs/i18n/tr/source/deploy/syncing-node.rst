.. _syncing-node:

=============================
Senkron düğümü (Syncing node)
=============================

**Senkron düğüm** (syncing node), zinciri takip eden ancak konsensüse
katılmayan bir Iroha peer'ıdır: öneri oluşturmaz, oy vermez ve
quorum'a dahil edilmez. Rolü, tam konsensüs mekanizmasının maliyeti
olmadan yerel world state view (WSV) kopyasını hızlı tutmaktır —
örneğin sorguları sunmak, okuma replikası olarak veya yeni bir tam
düğümü doldurmak için.

Senkron peer'lar normal peer'lar gibi ağa kaydedilir: ``Peer``
mesajı ``syncing_peer`` bayrağını taşır ve bu bayrak `Add Peer
<../develop/api/commands.html#add-peer>`_ komutuyla (veya genesis
block ile) ayarlanır. Komutu gönderen hesabın ``can_add_peer``
iznine sahip olması gerekir — normal bir peer için olduğu gibi.

Düğümü çalıştırma
-----------------

``irohad``'ı ``--syncing_node`` bayrağıyla başlatın:

.. code-block:: shell

    irohad --config example/config.sample \
           --genesis_block example/genesis.block \
           --keypair_name example/node0 \
           --syncing_node

Bu bayrak ``config.syncing_mode`` olarak yansır ve ordering ile
konsensüs başlatılırken kullanılır — düğüm YAC oylamasını atlar ve
denetimden geçmiş blokları peer'lerden çeker.

.. note:: Senkron düğüm yine de doğru yapılandırmaya, anahtar çiftine
  ve veritabanına ihtiyaç duyar — diğer peer'lar gibi blokları ve
  WSV'yi saklar. Sadece üretmez ve oy vermez.

İlgili kullanışlı bayraklar
---------------------------

- ``--exit_after_init`` — WSV'yi blok deposundan yeniden oluşturur
  ve çıkar. Bakım sonrası veya depolama arka ucu değişikliğinde
  yeniden indekslemek için kullanışlıdır; bkz. :ref:`deploy-flags`.
- ``--wait_for_new_blocks`` — ağı başlatmadan yerel blok deposunda
  yeni blokları bekleyen başlatma politikası.

Ayrıca bakınız
--------------

- :doc:`../concepts_architecture/glossary` — **Peer** altındaki
  *syncing peer* kaydı
- :ref:`yac` — normal (doğrulayan) peer'ların konsensüse nasıl
  ulaştığı
- :doc:`../configure/index` — ``healthcheck_port`` düğüm durumu
  yanıtında ``is_syncing`` değerini açığa çıkarır
