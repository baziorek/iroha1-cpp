.. _node-configuration:

======================
Düğüm yapılandırması
======================

Her ``irohad`` örneği, ``--config`` bayrağıyla iletilen tek bir JSON
dosyasıyla yapılandırılır (ilk çalıştırmada ``--genesis_block`` ve
``--keypair_name`` ile birlikte — bkz. :ref:`node-appimage`).

Depoda kopyalayıp uyarlayabileceğiniz hazır örnekler var:

- ``example/config.sample`` — genel yapılandırma (localhost
  PostgreSQL)
- ``example/config.docker`` — Docker imajının kullandığı
- ``example/config.postgres.sample`` — açık PostgreSQL varyantı
- ``example/config-win.sample`` — Windows varyantı

.. note:: Bu kılavuz, topluluk tarafından sürdürülen
  `baziorek/iroha1-cpp <https://github.com/baziorek/iroha1-cpp>`_
  çatallanmasıyla ilgilidir.

Temel ayarlar
-------------

- ``database`` — depolama arka ucu: ``postgres`` (host, port,
  kimlik bilgileri, çalışan/bakım veritabanları) veya
  ``block_store_path`` ile RocksDB
- ``torii_port`` — istemciler için gRPC portu (varsayılan
  ``50051``)
- ``internal_port`` — peer'lar arası gRPC portu (varsayılan
  ``10001``)
- ``max_proposal_size`` — blok/öneri başına maksimum işlem sayısı
- ``vote_delay`` / ``proposal_creation_timeout`` — konsensüs
  zamanlamaları
- ``mst_enable`` / ``mst_expiration_time`` — çoklu imza işlemleri
  desteği (bkz. :ref:`architecture`)
- ``metrics`` / ``healthcheck_port`` — gözlemlenebilirlik
  uç noktaları

Tam referans
------------

Her yapılandırma parametresi — türleri, varsayılan değerleri,
birimleri ve yan etkileriyle — :ref:`configuration` bölümünde
ayrıntılı olarak belgelenmiştir. Özellikle ayar ipuçları için
`ortama özgü parametreler
<../configure/index.html#environment-specific-parameters>`_
bölümüne bakın.

.. hint:: Çok düğümlü bir ağda her peer'ın kendi yapılandırması,
  anahtar çifti ve tüm peer'ları içeren bir genesis block'a ihtiyacı
  vardır — bu kurulum :ref:`deploy-guide` bölümünde açıklanmıştır.
