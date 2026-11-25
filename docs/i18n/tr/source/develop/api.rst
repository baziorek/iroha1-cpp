Iroha API reference
===================

API bölümünde Iroha ile etkileşime giren bir uygulamanın yapı
taşlarına bakacağız. Sistemin sahip olduğu komutları ve
sorguları, ayrıca taşıma ve uygulama katmanı mantığını kapsayan
istemci kütüphaneleri setini gözden geçireceğiz.

Iroha API `komut-sorgu ayrımı
<https://en.wikipedia.org/wiki/Command%E2%80%93query_separation>`_
ilkesine uyar.

Bir Iroha eşi ile istemci uygulaması arasındaki iletişim
`gRPC <https://grpc.io/about/>`_ çerçevesi aracılığıyla
sürdürülür. İstemci uygulamaları açıklanan protokole uymalı ve
bunların `tanımına
<../concepts_architecture/glossary.html#transaction>`_ uygun
olarak işlemleri oluşturmalıdır.


.. toctree::
    :maxdepth: 2
    :caption: İçindekiler

    api/commands.rst
    api/queries.rst
    api/permissions.rst

Buradaki komutlar, sistem içinde durumu değiştiren bir dizi
eylem olan `transaction <../concepts_architecture/glossary.html#transaction>`__
'ların parçalarıdır. Bir transaction doğrulama ve konsensüs
aşamalarından geçtiğinde, bir `block
<../concepts_architecture/glossary.html#block>`__ içine
yazılır ve değiştirilemez blok deposunda (blockchain) saklanır.

Transaction'lar, sistemdeki bir `entity
<../concepts_architecture/er_model.html>`__ üzerinde eylem
gerçekleştiren komutlardan oluşur. Entity bir hesap, varlık vb.
olabilir.
