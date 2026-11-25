.. _architecture:

Iroha'nın içinde ne var?
=========================

HL Iroha ağı, düğümler arasındaki iletişimi sağlayan birkaç temel
bileşenden oluşur. Aşağıda bunları inceleyebilirsiniz.

.. image:: ../../image_assets/pipeline-diagram.png
	:width: 80%
	:align: center
	:alt: Iroha Architecture Diagram

Torii
-----

⛩

*Torii* (), tapınağa girişi işaretleyen geleneksel bir Japon
torii kapısıdır — Iroha'nın tek giriş noktası için uygun bir ad.

`İstemciler <glossary.html#client>`__ için giriş noktası.
Taşıma olarak gRPC kullanır.
Iroha ile etkileşim kurmak için herkes `Commands
<../develop/api/commands.html>`__ ve `Queries
<../develop/api/queries.html>`__ bölümlerinde açıklanan gRPC uç
noktalarını veya `istemci kütüphanelerini
<../develop/libraries.html>`__ kullanabilir.

MST Processor
-------------

*Multisignature Transactions Processor*

Diğer peer'larla `Gossip protokolü
<https://en.wikipedia.org/wiki/Gossip_protocol>`_ aracılığıyla
mesaj alıp gönderen dahili bir gRPC hizmetidir. Görevi, `quorum'a
<glossary.html#quorum>` ulaşmak için yeterli imzayı henüz
alamamış `multisignature transaction'ları
<glossary.html#multisignature-transactions>` quorum'a ulaşana
kadar yaymaktır.

.. note::
    Peer'lar arası multisignature (MST) transaction yayılımı yalnızca
    düğüm yapılandırmasında ``mst_enable`` ``true`` olarak
    ayarlandığında çalışır (bkz. :ref:`configuration`). Bayrak
    kapalıyken MST transaction'ları her peer'da yerel olarak
    çalışmaya devam eder, ancak peer'lar bekleyen MST
    transaction'larını birbirleriyle değiştirmez.

Peer Communication Service
--------------------------

Iroha'nın dahili bileşeni — `Torii <#torii>`__ üzerinden
`MstProcessor <#MstProcessor>`_ aracılığıyla `Ordering Gate
<#ordering-gate>`__'e `transaction
<glossary.html#transaction>`__ aktaran bir aracı. PCS'nin ana
amacı, konsensüs uygulamasıyla etkileşimin karmaşıklığını
gizlemektir.

Ordering Gate
-------------

Iroha'nın dahili bileşeni (gRPC istemcisi) — `Peer Communication
Service <#peer-communication-service>`__'ten `Ordering Service
<#ordering-service>`__'e `transaction'ları
<glossary.html#transaction>`__ aktarır. Ordering Gate, Ordering
Service'ten `önerileri <glossary.html#proposal>` (zincirdeki
potansiyel bloklar) alır ve `stateful validation
<glossary.html#stateful-validation>`__ için `Simulator
<#simulator>`__'e gönderir. Ayrıca konsensüs turuna göre Ordering
Service'ten öneri ister.

Ordering Service
----------------

Iroha'nın dahili bileşeni (gRPC sunucusu) — diğer `peer'lerden
<glossary.html#peer>` mesajları alır ve `stateless validation
<glossary.html#stateless-validation>`__'dan geçmiş birkaç
`transaction'ı <glossary.html#transaction>`__ tek bir `öneride
<glossary.html#proposal>`__ birleştirir. Her düğümün kendi ordering
service'i vardır. Öneri oluşturma şu olaylardan biriyle
tetiklenebilir:

1. İşlemlerin toplanması için ayrılan süre doldu.

2. Ordering service tek bir öneri için izin verilen maksimum
   işlem sayısına ulaştı.

Her iki parametre de (zaman aşımı ve öneri maksimum boyutu)
yapılandırılabilir (`ortama özgü parametreler
<../configure/index.html#environment-specific-parameters>`_ sayfasına
bakın).

Her iki tetikleyici için ortak bir ön koşul, en az bir işlemin
ordering service'e ulaşmasıdır. Aksi takdirde öneri oluşturulmaz.

Ordering service ayrıca önerilerin ön doğrulamasını da yapar (örn.
öneriden stateless olarak reddedilen işlemleri temizler).

Verified Proposal Creator
-------------------------

`Ordering Service <#ordering-service>`__'ten alınan `öneride
<glossary.html#proposal>`__ bulunan `transaction'ların
<glossary.html#transaction>`__ `stateful validation
<glossary.html#stateful-validation>`__'ını gerçekleştiren Iroha
dahili bileşenidir. Stateful validation'dan geçen işlemlere
dayanarak **doğrulanmış öneri** oluşturulur ve `Block Creator
<#block-creator>`__'a aktarılır. Stateful validation'dan geçmeyen
tüm işlemler atılır ve doğrulanmış öneriye dahil edilmez.

Block Creator
-------------

`Stateless <glossary.html#stateless-validation>`__ ve `stateful
<glossary.html#stateful-validation>`__ validation'dan geçmiş
işlem kümesinden blok oluşturan ve daha sonra `consensus
<#consensus>`__'a yaymak için sistem bileşenidir.

Block creator, `Verified Proposal Creator
<#verified-proposal-creator>`__ ile birlikte `Simulator
<https://github.com/baziorek/iroha1-cpp/tree/iroha1-main/irohad/simulator>`__
adında bir bileşen oluşturur.

Block Consensus (YAC)
---------------------

*Bileşen olarak konsensüs*

Konsensüs, blokzincirin kalbidir — peer ağındaki `peer'lar
<glossary.html#peer>`__ arasında tutarlı bir durum korur. Iroha,
Yet Another Consensus (YAC) adlı kendi konsensüs algoritmasını
kullanır. Protokolün ayrıntılı açıklaması — turlar, oylama,
süper çoğunluk modelleri ve hata işleme — için :ref:`yac`'a
bakın.

Konsensüs ilkelerini, özellikle YAC'ı ayrıntılı olarak açıklayan
bir videoyu `burada <https://youtu.be/mzuAbalxOKo>`__
izleyebilirsiniz.

YAC algoritmasının ayırt edici özellikleri ölçeklenebilirliği,
performansı ve Crash fault tolerance'ıdır.

Ağda tutarlılığı sağlamak için eksik bloklar varsa, başka bir
peer'dan `Synchronizer <#synchronizer>`__ aracılığıyla
download edilir. Commit edilmiş bloklar `Ametsuchi
<#ametsuchi>`__ blok deposunda saklanır.

Konsensüsün genel tanımı için `bu bağlantıya
<glossary.html#consensus>`__ bakın.


Synchronizer
------------

`Consensus <#consensus>`__'in bir parçasıdır. `Peer'lerin
<glossary.html#peer>`__ zincirlerine eksik blokları ekler (tutarlılığı
korumak için diğer peer'lerden indirir).

Ametsuchi Blockstore
--------------------

*Ametsuchi* (), Japon mitolojisinde göksel hazinelerin ambarıdır —
burada, blokları ve bloklardan üretilen durumu (`World State View
<#world-state-view>`__ olarak adlandırılır) saklayan Iroha'nın
depolama bileşenidir. `İstemcinin
<glossary.html#client>`__ Ametsuchi ile doğrudan etkileşme yolu
yoktur.


World State View
----------------

WSV sistemin mevcut durumunu yansıtır ve bir anlık görüntü olarak
görülebilir. Örneğin WSV, bir `hesabın <glossary.html#account>`__
o anda sahip olduğu `varlık <glossary.html#asset>`__ miktarı
hakkında bilgi tutar, ancak `transaction akışının
<glossary.html#transaction>`__ geçmiş bilgisi içermez.
