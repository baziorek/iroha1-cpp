.. _configuration:

============
Yapılandırma
============

.. toctree::
      :maxdepth: 1

      torii-tls.rst
      db.rst

Bu bölümde Iroha'yı nasıl yapılandıracağımızı
anlayacağız. Bazı yapılandırma parametreleri tüm
düğümlerde aynı olmalıdır (\* ile işaretlenirler) ve
bazıları farklı olabilir. ``example/config.sample``
dosyasına bakalım.

.. note:: v1.2'den itibaren ``irohad`` yalnızca yapılandırma
   dosyasıyla değil, ortam değişkenleriyle de
   yapılandırılabilir.

Önce yapılandırma dosyasına, ardından Iroha'nın
`ortam değişkenleri <#environment-variables>`_ ile nasıl
yapılandırılacağına bakacağız.

.. literalinclude:: ../../../example/config.sample
   :language: json


Gördüğünüz gibi yapılandırma dosyası geçerli bir
``json`` yapısıdır. Satır satır geçelim ve her
parametrenin yapılandırma dosyası formatında ne
anlama geldiğini anlayalım.

Dağıtıma özgü parametreler
==========================

- ``block_store_path`` (isteğe bağlı) blokların
  saklandığı klasörün yolunu ayarlar. Bu parametre
  belirtilmezse bloklar veritabanında saklanır.
- ``torii_port`` dış iletişim için portu ayarlar.
  Sorgular ve işlemler buraya gönderilir.
- ``internal_port`` iç iletişim için portu ayarlar:
  sıralama hizmeti, konsensüs ve block loader.
- ``database`` (isteğe bağlı) veritabanı
  yapılandırmasını ayarlamak için kullanılır (aşağıya
  bakın)
- ``pg_opt`` (isteğe bağlı) PostgreSQL kimlik
  bilgilerini ayarlamanın **kullanımdan kaldırılmış**
  yoludur: ana makine adı, port, kullanıcı adı, parola
  ve veritabanı adı. Veritabanı adı hariç tüm veriler
  zorunludur. Veritabanı adı verilmezse varsayılan
  olan ``iroha_default`` kullanılır.
- ``log``, günlük çıktısının ayrıntı düzeyini ve
  biçimini kontrol eden isteğe bağlı bir parametredir
  (aşağıya bakın).
- ``utility_service`` (isteğe bağlı) bakım görevleri
  için uç noktadır. Mevcutsa, bağlanılacak ``ip``
  adresini ve ``port``'u içermelidir. Bakım uç
  noktasının örnek kullanımı için `shepherd
  belgelerine <../maintenance/shepherd.html>`_ bakın.
- ``metrics`` (isteğe bağlı) Iroha metriklerini izlemek
  için uç noktadır. Prometheus HTTP sunucusu bu uç
  noktada dinler. Mevcutsa "[addr]:<port>" biçimine
  uymalıdır ve örneğin "127.0.0.1:8080", "9090" veya
  ":1234" olabilir. Yanlış değerler Prometheus metrik
  sunucusunu örtük olarak devre dışı bırakır.
  Bu parametreyi geçersiz kılmak için ```--metrics_port``
  ve ``--metrics_addr`` komut satırı seçenekleri de
  vardır.
- ``healthcheck_port`` (isteğe bağlı) Iroha sağlık
  kontrolü için uç noktadır. Bu uç noktaya
  ``http://<host>:<healthcheck_port>/healthcheck``
  biçiminde bir istek göndermek düğümün durumu hakkında
  bilgi döndürür: geçerli bellek tüketimi
  (``memory_consumption``), geçerli blok sayısı
  (``last_block_round``), geçerli red turu sayısı
  (``last_reject_round``), düğümün o anda uzaktaki bir
  düğümle bilgi senkronize edip etmediği
  (``is_syncing``) ve düğümün şu anda çalışıp
  çalışmadığı (``status``).

Ayrıca, istemci iletişimi için TLS desteğini etkinleştirmek
üzere yapılandırmaya eklenebilecek isteğe bağlı bir
``torii_tls_params`` parametresi vardır.

Orada ``port``, TLS sunucusunun bağlanacağı TCP
portudur ve ``key_pair_path``, ``.crt`` eklenirse PEM
kodlamalı sertifikanın yolunu, ``.key`` eklenirse bu
sertifika için PEM kodlamalı özel anahtarın yolunu
veren anahtar çifti biçiminde bir yoldur (örn.
``key_pair_path`` ``"/path/to/the/keypair"`` ise
iroha sertifikayı ``"/path/to/the/keypair.crt"`` ve
anahtarı ``"/path/to/the/keypair.key"`` konumunda
arar)

.. warning::
   ``pg_opt`` yapılandırma alanı kullanımdan
   kaldırılmıştır, lütfen ``database`` bölümünü
   kullanın!

   Her ikisi de yapılandırmada sağlandığında
   ``database`` bölümü ``pg_opt``'u geçersiz kılar.

   Hem ``pg_opt`` hem de ``database`` alanları
   isteğe bağlıdır ancak en az biri belirtilmelidir.

``database`` bölümü alanları:

- ``host`` — PostgreSQL bağlantısı için kullanılacak
  ana makine
- ``port`` — PostgreSQL bağlantısı için kullanılacak
  port
- ``user`` — PostgreSQL bağlantısı için kullanılacak
  kullanıcı
- ``password`` — PostgreSQL bağlantısı için
  kullanılacak parola
- ``working database`` — world state view'ı ve
  isteğe bağlı olarak blokları saklamak için
  kullanılacak veritabanının adı
- ``maintenance database`` — çalışma veritabanını
  sürdürmek için kullanılacak veritabanının adı.
  Örneğin, iroha çalışma veritabanını oluşturması veya
  silmesi gerektiğinde, PostgreSQL'e bağlanmak için
  başka bir veritabanı kullanmalıdır.

Ortamla ilgili parametreler
===========================

- ``max_proposal_size`` \* tek bir teklifte ve
  sonuç olarak tek bir blokta bulunabilecek maksimum
  işlem sayısıdır. Bu değeri değiştirerek potansiyel
  bloğun boyutunu belirlersiniz. Başlangıç için
  ``10``'da kalabilirsiniz. Ancak saniyede çok işlem
  varsa bu sayıyı artırmanızı öneririz.

    **Bu parametre performansı etkiler.** Ağınızda çok
    sayıda işlem varsa bu parametreyi artırın.
    ``max_proposal_size`` değerini artan bir iş
    hacmi nedeniyle artırırsanız bağımsız olarak
    artırabilirsiniz. Bu parametreyi artırarak
    performansı iyileştirebilirsiniz ancak belirli bir
    noktadan sonra bu değeri artırmanın performansın
    düşmesine yol açabileceğini unutmayın.

- ``vote_delay`` \* bir sonraki eşe oy göndermeden
  önce milisaniye cinsinden bekleme süresidir. Optimal
  değer büyük ölçüde ağdaki Iroha eşlerinin sayısına
  bağlıdır (daha fazla düğüm daha uzun ``vote_delay``
  gerektirir). ** En az bir saniyeye ayarlamanızı
  şiddetle tavsiye ederiz — aksi takdirde bazı eşlere
  kolayca ulaşılamadığında blok zinciri çok yavaş
  büyür veya büyümeyi tamamen durdurur.**

    **Bu parametre yalnızca konsensüs mekanizmasını
    etkiler.** Ağınız hızlıysa her şey yolundadır ve
    bu parametre ağınızı pek etkilemez. Ancak ağınız
    yavaşsa, eşlerin yanıt vermesi için daha fazla
    zaman tanımak amacıyla artırın.

- ``mst_enable``, Iroha'da çoklu imza işlemi ağ
  aktarımını etkinleştirir veya devre dışı bırakır.
  MST motorunun bayrak ``false`` olarak ayarlandığında
  bile her eş için her zaman çalıştığını unutmayın.
  Bayrak yalnızca MST işlemleri hakkında bilgileri
  eşler arasında paylaşmaya izin verir.

- ``mst_expiration_time``, tam imzalanmamış bir
  işlemin (veya toplu) süresi dolmuş sayıldığı süreyi
  (dakika cinsinden) belirten isteğe bağlı bir
  parametredir. Varsayılan değer 1440'tır.

- ``proposal_creation_timeout`` (eski adı
  ``max_rounds_delay``)\* sıralama hizmetinin bir
  teklifi oluştururken — yani ``max_proposal_size``'a
  kadar işlem toplarken — bir teklif isteğine
  yanıt vermeden önce beklediği maksimum süreyi
  (milisaniye cinsinden) belirten isteğe bağlı bir
  parametredir. Varsayılan değer 3000'dir.
  İkinci, türetilmiş bir zaman aşımı — eşler
  arasında ``RequestProposal`` RPC'leri için ağ son
  tarihi — ``2 × proposal_creation_timeout``
  olarak otomatik hesaplanır ve ayrıca
  yapılandırılamaz.

    **Bu parametre kaynak tüketimini etkiler.**
    Iroha'nın daha uzun süre boşta kalmasını
    bekliyorsanız ve bazı kaynakları tasarruf etmek
    istiyorsanız bu değeri artırın — Iroha'nın yeni
    işlemleri daha seyrek kontrol etmesini sağlar.
    NB: boşta kalma süresinden sonraki ilk işlem bu
    nedenle biraz gecikebilir. İkinci ve sonraki
    bloklar daha hızlı işlenecektir.

- ``stale_stream_max_rounds``, hiçbir durum güncellemesi
  bildirilmezken açık durum akışını kaç tur boyunca
  tutacağını belirten isteğe bağlı bir parametredir.
  Varsayılan değer 2'dir. Bu değeri artırmak, bir
  işlem neden herhangi bir nedenle yeni turlarla
  güncellenmezse istemcinin işlemi izlemek için
  yeniden bağlanması gereken sefer sayısını azaltır.
  Ancak büyük değerler her turda bağlı istemcilerin
  ortalama sayısını artırır.

    Düğümün akışlarla aşırı yüklenmemesini sağlamak
    için bu parametreyi sınırlamak önerilir.

- ``initial_peers``, bir düğümün başlangıç bloğundaki
  eşler yerine başlangıçtan sonra kullanacağı eşlerin
  listesini belirten isteğe bağlı bir parametredir.
  Başlangıç eşlerinin çoğunun kötü niyetli olabileceği
  bir ağa yeni bir düğüm eklerken yararlı olabilir.
  Eş listesi bir JSON dizisi olarak sağlanmalıdır:

.. code-block:: javascript

  "initial_peers": [
    {
      "address": "127.0.0.1:10001",
      "public_key": "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
    }
  ]

- ``max_past_created_hours``: işlemin ``created_time``
  değerinin geçerli zamandan (eş üzerinde ölçülen)
  kaç saat geriye ayarlanabileceğini belirten isteğe
  bağlı parametredir. Varsayılan değer ``"24"``
  saattir. Bu değer tüm eşlerde aynı olmalıdır,
  aksi takdirde ağın blok üretmeyi durdurmasına sessizce
  neden olabilir.

İyi Uygulama Örneği
-------------------

Eşit dağılım ile oldukça iyi sonuçlar aldık — 5 dakikada
gönderilen 300k işlem. Commit 2 saniyeden 2 dakikaya
kadar sürdü.
**Lütfen sonuçların her zaman ağınızdaki eş sayısına,
hızına ve eşlerin çalıştığı ana bilgisayarların
parametrelerine bağlı olduğunu unutmayın.**

Kullandığımız yapılandırma:

.. code-block:: javascript

  "max_proposal_size" : 10000,
  "vote_delay" : 1000,
  "mst_enable" : true,
  "mst_expiration_time": 1440,
  "proposal_creation_timeout": 500,
  "stale_stream_max_rounds": 100000


Ortam Değişkenleri
==================

Iroha'yı yapılandırmanın başka bir yolu ortam
değişkenlerini kullanmaktır. Yapılandırma dosyası ve
ortam değişkenleri birleştirilebilir. **Yapılandırma
dosyasında belirtilen parametreler, mevcutsa, ortam
üzerinden ayarlananları geçersiz kılar.**

Parametrelerin nasıl görüneceğine dair bazı örnekler:

Unix
----

.. code-block:: bash

  export IROHA_BLOCK_STORE_PATH=/tmp/block_store/
  export IROHA_TORII_PORT=50051
  export IROHA_INTERNAL_PORT=10001
  export IROHA_PG_OPT="host=172.19.0.2 port=5432 user=iroha password=helloworld"
  export IROHA_MAX_PROPOSAL_SIZE=10
  export IROHA_VOTE_DELAY=5000
  export IROHA_MST_ENABLE=false
  export IROHA_MST_EXPIRATION_TIME=1440
  export IROHA_PROPOSAL_CREATION_TIMEOUT=3000
  export IROHA_CRYPTO_PROVIDERS_0_KEY=p1
  export IROHA_CRYPTO_PROVIDERS_0_CRYPTO_TYPE=ed25519_sha3_256
  export IROHA_CRYPTO_PROVIDERS_0_PRIVATE_KEY=cc5013e43918bd0e5c4d800416c88bed77892ff077929162bb03ead40a745e88
  export IROHA_CRYPTO_PROVIDERS_0_TYPE=default
  export IROHA_CRYPTO_SIGNER=p1

Windows
-------

.. code-block:: bat

  setx IROHA_BLOCK_STORE_PATH C:\block_store
  setx IROHA_TORII_PORT 50051
  setx IROHA_INTERNAL_PORT 10001

PowerShell
----------

.. code-block:: bash

  $Env:IROHA_BLOCK_STORE_PATH="C:\block_store"
  $Env:IROHA_TORII_PORT="50051"
  $Env:IROHA_INTERNAL_PORT="10001"

Parametre Adları
----------------

Gördüğünüz gibi parametre adları yapılandırma
dosyasındakilerle aynı değildir.

Yapılandırma yapısından oluşturulurlar, başa sabit IROHA
etiketi eklenir ve her şey büyük harfe çevrilip _ ile
birleştirilir. Nasıl yapılandıklarına biraz daha yakından
bakalım:

**Basit dize değerleriyle**

Yapılandırma dosyasında:

.. code-block:: javascript

  "block_store_path": "/tmp/block_store/"

Ortam değişkenlerinde:

.. code-block:: bash

  IROHA_BLOCK_STORE_PATH=/tmp/block_store/


**Dizilerle**

Diziler 0'dan başlayarak dizinlenir ve hiçbir sayıyı
atlamadan doğrudan sırada olmalıdır:

Yapılandırma dosyasında:

.. code-block:: javascript

  "initial_peers": [
    {
      "address": "127.0.0.1:10001",
      "public_key": "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
    },
    {
      "address": "127.0.0.1:10002",
      "public_key": "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308920"
    }
  ]

Ortam değişkenlerinde:

.. code-block:: bash

  IROHA_INITIAL_PEERS_0_ADDRESS=127.0.0.1:10001
  IROHA_INITIAL_PEERS_0_PUBLIC_KEY=bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929
  IROHA_INITIAL_PEERS_1_ADDRESS=127.0.0.1:10002
  IROHA_INITIAL_PEERS_1_PUBLIC_KEY=bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308920

**Kullanıcı tanımlı anahtarlı sözlükler**

Kullanıcı tarafından sağlanan sözlük anahtarları biraz
daha karmaşıktır: anahtar ve değer ayrı değişkenlerde
ayarlanır. Kriptografi sağlayıcılarının yapılandırılması
örneğiyle gösterilebilir:

Kriptoğrafi Sağlayıcılar
========================

Şu anda HL Iroha ``default`` adlı tek bir sağlayıcı
türünü destekler — yerleşik kripto sağlayıcıları içerir:
HL Iroha kripto kütüphanesi (``ed25519_sha3_256`` kripto
türü ile) ve HL Ursa kütüphanesi (Iroha'nın şu anda
``ed25519_sha2_256`` kripto türünü desteklediği).

İkisi de yalnızca ``crypto_type`` ve ``private_key``
parametrelerini alır.

.. note::  ``hsm`` — donanım güvenlik modülleri için —
   dahil diğer türleri eklemek üzerinde çalışıyoruz;
   bunlar farklı bir parametre kümesine sahip olacak ve
   sürümden sonra buraya eklenecektir.

Kripto sağlayıcılarını yapılandırma
-----------------------------------

Şu anda mevcut kripto sağlayıcılarını
yapılandırmak için, eş üzerinde kullanılabilecek
sağlayıcıları tanımlamanız (``p1`` ve ``p2``'ye bakın)
ve ardından bu düğümde mesajları imzalamak için
kullanılacak ``signer``'ı seçmeniz gerekir:

Yapılandırma dosyasında:

.. code-block:: javascript

  "crypto": {
    "providers": {
      "p1": {
        "crypto_type": "ed25519_sha3_256",
        "private_key": "cc5013e43918bd0e5c4d800416c88bed77892ff077929162bb03ead40a745e88",
        "type": "default"
      },
      "p2": {
        "crypto_type": "ed25519_sha2_256",
        "private_key": "7bab70e95cb585ea052c3aeb27de0afa9897ba5746276aa1c25310383216ceb860eb82baacbc940e710a40f21f962a3651013b90c23ece31606752f298c38d90",
        "type": "default"
      }
    },
    "signer": "p1"
  }

Ortam değişkenlerinde:

.. code-block:: bash

  IROHA_CRYPTO_PROVIDERS_0_KEY=p1
  IROHA_CRYPTO_PROVIDERS_0_CRYPTO_TYPE=ed25519_sha3_256
  IROHA_CRYPTO_PROVIDERS_0_PRIVATE_KEY=cc5013e43918bd0e5c4d800416c88bed77892ff077929162bb03ead40a745e88
  IROHA_CRYPTO_PROVIDERS_0_TYPE=default
  IROHA_CRYPTO_PROVIDERS_1_KEY=p2
  IROHA_CRYPTO_PROVIDERS_1_CRYPTO_TYPE=ed25519_sha2_256
  IROHA_CRYPTO_PROVIDERS_1_PRIVATE_KEY=7bab70e95cb585ea052c3aeb27de0afa9897ba5746276aa1c25310383216ceb860eb82baacbc940e710a40f21f962a3651013b90c23ece31606752f298c38d90
  IROHA_CRYPTO_PROVIDERS_1_TYPE=default
  IROHA_CRYPTO_SIGNER=p1


Günlük Kaydı
============

Iroha'da günlük kaydı dilediğiniz kadar ayrıntılı
ayarlayabilirsiniz. Her bileşenin, üst öğesinden devralınan
ve yapılandırma dosyasıyla geçersiz kılınabilen
özelliklerle kendi günlük kaydı yapılandırması vardır.
Bu, tüm bileşen günlükçülerinin tek köklü bir ağaçta
düzenlendiği anlamına gelir. Yapılandırma dosyasının
ilgili bölümü geçersiz kılma değerlerini içerir:

Yapılandırma dosyasında:

.. code-block:: javascript

  "log": {
    "level": "trace",
    "patterns": {
      "debug": "don't panic, it's %v.",
      "error": "MAMA MIA! %v!"
    },
    "children": {
      "KeysManager": {
        "level": "trace"
      },
      "Irohad": {
        "children": {
          "Storage": {
            "level": "trace",
            "patterns": {
              "debug": "thread %t: %v."
            }
          }
        }
      }
    }
  }

Ortam değişkenlerinde:

.. code-block:: bash

  IROHA_LOG_LEVEL=trace
  IROHA_LOG_PATTERNS_0_KEY=debug
  IROHA_LOG_PATTERNS_0="don't panic, it's %v."
  IROHA_LOG_PATTERNS_1_KEY=error
  IROHA_LOG_PATTERNS_1="MAMA MIA! %v!"
  IROHA_LOG_CHILDREN_0_KEY=KeysManager
  IROHA_LOG_CHILDREN_0_LEVEL=trace
  IROHA_LOG_CHILDREN_1_KEY=Irohad
  IROHA_LOG_CHILDREN_1_CHILDREN_0_KEY=Storage
  IROHA_LOG_CHILDREN_1_CHILDREN_0_LEVEL=trace
  IROHA_LOG_CHILDREN_1_CHILDREN_0_PATTERNS_0_KEY=debug
  IROHA_LOG_CHILDREN_1_CHILDREN_0_PATTERNS_0="thread %t: %v."

Bu yapılandırma bölümünün her parçası isteğe bağlıdır.

- ``level`` ayrıntı düzeyini ayarlar. Mevcut değerler
  (azalan ayrıntı sırasıyla):

  - ``trace`` - her şeyi yazdır
  - ``debug``
  - ``info``
  - ``warning``
  - ``error``
  - ``critical`` - yalnızca kritik mesajları yazdır

- ``patterns`` farklı ayrıntı düzeyleri için her günlük
  dizgisinin biçimlendirmesini kontrol eder. Her değer
  daha az ayrıntılı düzeyleri de geçersiz kılar. Yani
  yukarıdaki örnekte "don't panic" kalıbı info ve
  warning düzeylerine de uygulanır ve trace düzeyi
  kalıbı yapılandırmada başlatılmayan tek kalıptır
  (varsayılan sabit kodlanmış değere ayarlanacaktır).

.. note::  Tek bir bileşen için birden fazla kalıp
   belirtilse bile bu bileşen yalnızca bir kalıp
   kullanacaktır — seçilen günlük düzeyine karşılık
   gelen kalıp. Ancak kalıplar devralınır ve alt
   günlükçülerde kullanılabilir.

- ``children`` alt düğümlerin geçersiz kılmalarını
  açıklar. Anahtarlar bileşen adlarıdır ve değerler
  kök günlük yapılandırmasıyla aynı sözdizimi ve
  anlambilime sahiptir.
