.. _getting-started:

=================
Hızlı başlangıç
=================

Hyperledger Iroha'yı kullanmak için iki şeye ihtiyacınız var:

#. **En az bir Iroha düğümü** — ağ oluşturan veya ona katılan
   çalışan bir ``irohad`` süreci
#. **İşlem gönderme yolu** — düğümle gRPC üzerinden iletişim
   kuran bir istemci: ``iroha-cli`` aracı, istemci
   kütüphanelerinden biri veya kendi kodunuz

Bu kılavuz her ikisini de adım adım açıklar. Önce Iroha düğümünü
nasıl edineceğinizi seçin, ardından onunla nasıl iletişim
kuracağınızı seçin.

.. note:: Ledger, blockchain'in eş anlamlısıdır ve Hyperledger
  Iroha ayrıca Distributed Ledger Technology çerçevesi olarak da
  bilinir — bu özünde "blockchain framework" ile aynıdır. Diğer
  terminoloji için :ref:`core-concepts` bölümüne bakın.

.. toctree::
   :maxdepth: 2
   :caption: Iroha düğümü edinin

   node-docker.rst
   node-appimage.rst
   node-deb.rst
   node-build.rst

.. toctree::
   :maxdepth: 2
   :caption: İletişim aracı — ``iroha-cli``

   cli.rst

.. toctree::
   :maxdepth: 2
   :caption: İstemci kütüphaneleri

   cli-guide.rst
   python-guide.rst

.. toctree::
   :maxdepth: 2
   :caption: Düğüm yapılandırması

   configuration.rst
