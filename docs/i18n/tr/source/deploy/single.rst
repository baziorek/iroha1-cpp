=======================
Tek örnek çalıştırma
=======================

Genellikle insanlar API'yi denemek ve yetenekleri keşfetmek için
Iroha'yı yerel olarak çalıştırmak ister. Bu yerel veya konteyner
ortamında (Docker) yapılabilir. Her iki olası durumu da inceleyeceğiz
ancak eş bileşenlerinin dağıtımını basitleştirmek için *makinenizde
Docker kurulu olması önerilir*.

Yerel ortam
-----------

Yerel ortam ile konteynersiz olarak dağıtılmış daemon süreci ve
Postgres kastedilir. Docker ile uğraşmanın tercih edilmediği
durumlarda — genellikle özelliklerin hızlı keşfi — yardımcı
olabilir.

postgres sunucusunu çalıştırma
""""""""""""""""""""""""""""""

postgres sunucusunu yerel olarak çalıştırmak için postgres'in
`website
<https://www.postgresql.org/docs/current/static/server-start.html>`__
sayfasını kontrol etmeli ve açıklamalarını takip etmelisiniz.
Genellikle postgres sunucusu sistem başlatıldığında otomatik
olarak çalışır, ancak bu sistem yapılandırmasında kontrol
edilmelidir.

Postgres veritabanı sunucusu, olağan sistem entegrasyonu olmadan
manuel olarak başlatılıp çalıştırılabilir:

.. code-block::shell

   initdb ~/iroha/nodeX_db/
   ## Start server in background, logs will appear in current console
   postgres -D ~/iroha/nodaX_db/ -p5433 &
   createuser -s iroha_user -p5433

Seçilen ``5433`` portu (varsayılan 5432) ve ``iroha_user``
veritabanı kullanıcısı, irohad'ın veritabanına bağlanmak için
kullandığı değerlerdir. (Daha fazla bilgi için `Yapılandırma
parametreleri <../configure/index.html>`_ sayfasına bakın.)
``postgres`` bakım veritabanı varsayılan olarak oluşturulur, ancak
herhangi bir nedenle başka bir ad gerekiyorsa oluşturun:

.. code-block::shell

   createdb iroha_mainteance -p5433


iroha daemon'unu (irohad) çalıştırma
""""""""""""""""""""""""""""""""""""""""""

Devam etmeden önce karşılamanız gereken bir ön koşul listesi vardır:

* Postgres sunucusu çalışıyor
* `irohad` Iroha daemon binary'si derlenmiş ve sistemde erişilebilir
* genesis block ve yapılandırma dosyaları oluşturulmuş
* Yapılandırma dosyası geçerli postgres bağlantı ayarları kullanıyor
* Eş için anahtar çifti üretilmiş
* Bu peer'da Iroha'yı ilk kez çalıştırıyorsunuz ve yeni bir zincir
  oluşturmak istiyorsunuz

.. Hint:: Varsayımlar listesiyle eşleşmeyen bir şeyiniz mi var?
  Lütfen :ref:`deploy_troubles` bölümüne bakın.

Varsayımlar geçerliyse, geriye yalnızca daemon sürecini şu
parametrelerle başlatmak kalır:

+---------------+---------------------------------------------------------------+
| Parametre     | Anlam                                                         |
+===============+===============================================================+
| config        | postgres bağlantısı ve sistemi ayarlamak için değerler içeren |
|               | yapılandırma dosyası                                          |
| genesis_block | defterdeki başlangıç bloğu                                    |
| keypair_name  | blokları imzalamak için peer tarafından kullanılan uzantısız  |
|               | özel ve açık anahtar dosya adları                             |
+---------------+---------------------------------------------------------------+

.. Attention:: Ledger'da zaten bloklar varken `--genesis_block` ile
  yeni bir genesis block belirtmek `--overwrite_ledger` bayrağının
  ayarlanmasını gerektirir. Aksi takdirde daemon başarısız olur.

Iroha daemon'unu çalıştıran bir shell komutu örneği:

.. code-block:: shell

    irohad --config example/config.sample --genesis_block example/genesis.block --keypair_name example/node0

.. Note:: `HL Ursa desteği
  <../integrations/index.html#hyperledger-ursa>`__ ile derlenmiş
  Iroha çalıştırıyorsanız, örnek anahtarları ve genesis block'u
  `example/ursa-keys/` içinden alın

.. Attention:: Daemon'u durdurduysanız ve mevcut zinciri
  kullanmak istiyorsanız — genesis block parametresini geçirmemelis
  iniz.


Docker
------

Iroha peer'ini Docker'da tek örnek olarak çalıştırmak için önce
Iroha imajını pull etmelisiniz:

.. code-block:: shell

    docker pull hyperledger/iroha:1.6.0

.. note:: ``hyperledger/iroha:1.6.0`` son resmi upstream Iroha 1
  imajıdır — ``:latest``'i (2022'den kalma eski bir derleme) veya
  ``:develop``/``:main`` etiketlerini (artık Iroha 2 derlemelerine
  işaret ediyorlar) **kullanmayın**.

Ardından, imajın sorunsuz çalışması için bir ortam oluşturmanız
gerekir:

docker ağı oluşturma
""""""""""""""""""""

Postgres ve Iroha konteynerleri birbirlerine erişebilmek için aynı
sanal ağda çalışmalıdır. Aşağıdaki komutu yazarak bir ağ oluşturun
(ağ için herhangi bir ad kullanabilirsiniz, ancak örnekte
*iroha-network* adını kullanıyoruz):

.. code-block:: shell

    docker network create iroha-network

Konteynerde Postgresql çalıştırma
""""""""""""""""""""""""""""""""""""""

Benzer şekilde, postgres sunucusunu daha önce oluşturduğunuz ağa
bağlayarak ve iletişim için portları açarak çalıştırın:

.. code-block:: shell

    docker run --name some-postgres \
    -e POSTGRES_USER=postgres \
    -e POSTGRES_PASSWORD=mysecretpassword \
    -p 5432:5432 \
    --network=iroha-network \
    -d postgres:9.5

Blok depolama için volume oluşturma
"""""""""""""""""""""""""""""""""""""""""""""

iroha daemon'unu konteynerde çalıştırmadan önce, zincirin
bloklarını depolayan dosyalar için kalıcı bir volume
oluşturmalıyız. Bu aşağıdaki komutla yapılır:

.. code-block:: shell

    docker volume create blockstore

docker konteynerinde iroha daemon'unu çalıştırma
""""""""""""""""""""""""""""""""""""""""""""""""""""

Devam etmeden önce gözden geçirmeniz gereken bir varsayım listesi
vardır:

* Postgres sunucusu aynı docker ağında çalışıyor
* Tek düğüm için yapılandırma dosyası ve anahtar çifti içeren bir
  klasör var
* Bu peer'da Iroha'yı ilk kez çalıştırıyorsunuz ve yeni bir zincir
  oluşturmak istiyorsunuz

Bunlar sağlanıyorsa, aşağıdaki komutla devam edebilirsiniz:

.. code-block:: shell

    docker run --name iroha \
    # External port
    -p 50051:50051 \
    # Folder with configuration files
    -v ~/Developer/iroha/example:/opt/iroha_data \
    # Blockstore volume
    -v blockstore:/tmp/block_store \
    # Postgres settings
    -e POSTGRES_HOST='some-postgres' \
    -e POSTGRES_PORT='5432' \
    -e POSTGRES_PASSWORD='mysecretpassword' \
    -e POSTGRES_USER='postgres' \
    # Node keypair name
    -e KEY='node0' \
    # Docker network name
    --network=iroha-network \
    hyperledger/iroha:1.6.0

.. note:: ``hyperledger/iroha:1.6.0`` son resmi upstream Iroha 1
  imajıdır — ``:latest``'i (2022'den kalma eski bir derleme)
  kullanmayın.

.. note:: **İlk başlatmada** Iroha, WSV'yi blockstore'dan yeniden
  oluşturur; bu bellek açısından yoğundur — büyük defterlerde
  konteyner OOM ile öldürülebilir (bkz.
  `issue #1833
  <https://github.com/hyperledger-iroha/iroha/issues/1833>`__).
  docker konteynerlerinin varsayılan olarak bellek limiti yoktur,
  ancak **Docker Desktop**'ta (macOS/Windows) tüm VM'nin bir bellek
  sınırı vardır — *Settings → Resources* altında artırın veya Linux
  host'larda ``docker run``'a ``--memory`` geçirin.
