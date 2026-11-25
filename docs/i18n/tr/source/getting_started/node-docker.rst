.. _node-docker:

==================================
Docker ile Iroha düğümü çalıştırma
==================================

Bu kılavuz, yerel makinenizde Docker'da ``irohad``'ı — düğümün
kendisini — dağıtmanızı adım adım anlatır. Bu adımları
tamamladığınızda, ``iroha-cli`` (bkz. :doc:`cli`) veya istemci
kütüphanelerinden biri (bkz. :doc:`../develop/libraries`) aracılığıyla
işlem gönderebileceğiniz çalışan bir düğümünüz olur.

.. note:: Bu kılavuz topluluk çatallanması
  `baziorek/iroha1-cpp <https://github.com/baziorek/iroha1-cpp>`_
  ile ilgilidir.

Başlamadan önce
---------------

Şunların kurulu ve çalışır durumda olduğundan emin olun:

- `Docker <https://docs.docker.com/get-docker/>`_ (macOS/Windows'ta
  Docker Desktop, Linux'ta Docker Engine)
- `docker compose <https://docs.docker.com/compose/>`_ (genellikle
  Docker Desktop'a dahildir; Linux'ta ayrı kurulum gerekebilir)
- Depoyu klonlamak ve örnek yapılandırmayı almak için
  `git <https://git-scm.com/downloads>`_
- (İsteğe bağlı) kullanışlı komut takma adları ayarlamak
  istiyorsanız `Python <https://www.python.org/downloads/>`_

1. Depoyu klonlayın
-------------------

Topluluk çatallanmasının deposunu klonlayın ve ``example/``
dizinine gidin:

.. code-block:: shell

  git clone -b iroha1-main https://github.com/baziorek/iroha1-cpp
  cd iroha1-cpp/example

İlk düğüm çalıştırması için gereken tüm dosyalar burada:
``config.docker`` (Docker imajı için yapılandırma),
``genesis.block``, ``node0`` anahtar çifti vb.

2. PostgreSQL ve Iroha düğümünü başlatın
----------------------------------------

``example/`` içinde üç servisi başlatan hazır bir
``docker-compose.yml`` var: PostgreSQL (Iroha'nın depolaması), bir
Iroha düğümü ve Torii için Nginx port yönlendiricisi:

.. code-block:: shell

  docker compose up

İlk seferde Docker ``irohad`` imajını indirip derleyecek ve bu
birkaç dakika sürecektir. Konteynerler ``healthy`` bildirene kadar
bekleyin, ardından düğümün yanıt verdiğini doğrulayın:

.. code-block:: shell

  docker compose ps

Beklenen çıktı, üç servisin de ``running`` veya ``healthy``
durumunda olmasıdır.

3. İlk işlemi gönderin
----------------------

Çalışan konteynerlerden birinde zaten ``iroha-cli`` var.
Etkileşimli bir oturum açın:

.. code-block:: shell

  docker exec -it iroha iroha-cli -account_name admin@test

Varsayılan olarak CLI mevcut komutları sunar. Tüm seçenekleri
görmek için konteyner içinde veya çalışan imajın içinde
``iroha-cli -h`` çalıştırın.

``iroha-cli`` ile işlem ve sorgu göndermeye dair ayrıntılı kılavuz
:doc:`cli-guide`'dadır.

4. Sonraki adımlar
------------------

- :doc:`configuration` — yapılandırma parametrelerinin ayrıntılı
  dökümü
- :doc:`../deploy/single` — gerçek (Docker olmayan) bir sunucuda
  çalıştırma
- :doc:`../develop/libraries/cpp` — ``iroha-lib`` ile kendi C++
  uygulamanızı yazma

Düğümü durdurma
---------------

İşiniz bittiğinde konteynerleri durdurun:

.. code-block:: shell

  docker compose down

PostgreSQL verilerini de silmek için:

.. code-block:: shell

  docker compose down -v
