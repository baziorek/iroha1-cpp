.. _node-deb:

=================================
Sürüm paketlerinden Iroha düğümü
=================================

Docker imajlarına, AppImage'lere ve kaynak kod derlemelerine ek
olarak, Iroha 1'in sürüm hattı topluluk çatallanmasının `GitHub
Releases <https://github.com/baziorek/iroha1-cpp/releases>`__
sayfasına yerel paketler ekler:

- ``iroha_*.deb`` — CPack ile derlenen Debian/Ubuntu paketi;
  ``irohad`` ve bakım araçlarını (migration tool, WSV checker)
  içerir
- ``iroha-*.rpm`` — RPM dağıtımları için eşdeğeri (yayınlandığında)
- ``irohad-x86_64.AppImage`` / ``iroha-cli-x86_64.AppImage`` —
  bkz. :doc:`node-appimage`

.. note:: Paketler ``1.6.1`` etiketinden itibaren Releases
  sayfasında görünür. O zamana kadar :ref:`Docker <node-docker>`
  veya :ref:`kaynak koddan derleme <node-build>` seçeneklerini
  kullanın.

.deb paketi kurulumu
--------------------

Releases sayfasından ``.deb`` dosyasını indirin, ardından
dependencies'leri de çözen ``apt`` ile kurun:

.. code-block:: shell

  sudo apt install ./iroha_*.deb

veya doğrudan ``dpkg`` ile:

.. code-block:: shell

  sudo dpkg -i iroha_*.deb
  # apt eksik bağımlılıklar bildirirse:
  sudo apt -f install

Paket ikilileri sisteme kurar (CPack varsayılan öneki ``/usr``).
Doğrulayın:

.. code-block:: shell

  which irohad
  irohad --help

.rpm paketi kurulumu
--------------------

RPM dağıtımlarında (Fedora, RHEL, openSUSE):

.. code-block:: shell

  sudo dnf install ./iroha-*.rpm
  # veya: sudo rpm -i iroha-*.rpm && sudo dnf -f install

Düğümü çalıştırma
-----------------

Paketlenmiş ``irohad``, diğer derlemelerle aynı argümanları kabul
eder — yapılandırma dosyası, genesis block ve anahtar çifti adı:

.. code-block:: shell

  sudo irohad --config /etc/iroha/config.json \
              --genesis_block /etc/iroha/genesis.block \
              --keypair_name /etc/iroha/node0

Paket yapılandırma içermez — depo ``example/`` dizininden örnekleri
kopyalayın ve ortamınıza uyarlayın (:doc:`configuration`'a bakın);
düğüm başlamadan önce erişilebilir olması gereken ``database`
bölümünü (PostgreSQL veya RocksDB) içerir.

.. note:: Paketlenmiş düğüm yine de PostgreSQL (veya RocksDB)
gerektirir ve üretim dağıtımlarında Torii önünde TLS sonlandırması
gerekir — bkz. :doc:`../configure/torii-tls`.
