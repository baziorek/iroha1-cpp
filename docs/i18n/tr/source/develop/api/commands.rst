Komutlar
========

Bir komut, sistemdeki bir varlık (varlık, hesap) üzerinde bir işlem gerçekleştirerek World State View adındaki durumu değiştirir.
Herhangi bir komut, bir işlem gerçekleştirmek için bir transaction'a dahil edilmelidir.

Hata kodları
------------

Her komut, transaction durumunda rapor edilen sayısal bir ``error_code`` ile başarısız olabilir.
``0``, ``1`` ve ``2`` kodları tüm komutlar için aynı anlama sahiptir
(``0`` = başarı, ``1`` = dahili hata, ``2`` = yetki yok);
``>= 3`` kodları komuta özeldir.

Kanonik tanımlar, Postgres ve RocksDB arka uçları ile istemci kütüphaneleri tarafından paylaşılan ``shared_model/interfaces/command_error_codes.hpp`` dosyasında yer alır.
Aşağıdaki tablo, hızlı referans için komut bazlı kodları özetler; tam başlık dosyası tablonun ardından yeniden üretilmiştir.
Bir tablo ile başlık hiç uyuşmazsa, **başlık gerçek kaynaktır** — yürütücülerin gerçekte yaydığı değer budur.

.. list-table:: Komut hata kodları (transaction durumundaki ``error_code``)
   :header-rows: 1
   :widths: 30 10 60

   * - Komut
     - Kod
     - Anlam
   * - *herhangi bir komut*
     - 0
     - başarı
   * - *herhangi bir komut*
     - 1
     - dahili hata
   * - *herhangi bir komut*
     - 2
     - yetki yok
   * - Varlık miktarı ekle
     - 3
     - böyle bir varlık yok (veya hassasiyet çok düşük)
   * - Varlık miktarı ekle
     - 4
     - sonuç miktarı taşması
   * - İmzalayıcı ekle
     - 3
     - böyle bir hesap yok
   * - İmzalayıcı ekle
     - 4
     - imzalayıcı zaten var
   * - Rol ekle
     - 3
     - böyle bir hesap yok
   * - Rol ekle
     - 4
     - böyle bir rol yok
   * - Motor çağır
     - 3
     - kontrat motoru çağrısı başarısız
   * - Hesap detayını karşılaştır ve ayarla
     - 3
     - böyle bir hesap yok
   * - Hesap detayını karşılaştır ve ayarla
     - 4
     - beklenen değer eşleşmiyor
   * - Hesap oluştur
     - 3
     - böyle bir domain yok (varsayılan rol yok)
   * - Hesap oluştur
     - 4
     - hesap zaten var
   * - Varlık oluştur
     - 3
     - böyle bir domain yok
   * - Varlık oluştur
     - 4
     - varlık zaten var
   * - Domain oluştur
     - 3
     - domain zaten var
   * - Domain oluştur
     - 4
     - varsayılan rol yok
   * - Rol oluştur
     - 3
     - rol zaten var
   * - Rolü kaldır
     - 3
     - böyle bir hesap yok
   * - Rolü kaldır
     - 4
     - rol hesaba bağlı değil
   * - Rolü kaldır
     - 5
     - rol mevcut değil
   * - İzin ver
     - 3
     - böyle bir hesap yok
   * - Eş bulun
     - 3
     - böyle bir eş yok
   * - Eş bulun
     - 4
     - kaldırma hiç eş bırakmaz
   * - İmzacı kaldır
     - 3
     - böyle bir hesap yok
   * - İmzacı kaldır
     - 4
     - açık anahtar imzacı değil
   * - İmzacı kaldır
     - 5
     - kaldırma yeter sayıyı ihlal eder
   * - İzni iptal et
     - 3
     - böyle bir hesap yok
   * - Hesap detayını ayarla
     - 3
     - böyle bir hesap yok
   * - Hesap yeter sayısını ayarla
     - 3
     - böyle bir hesap yok
   * - Hesap yeter sayısını ayarla
     - 4
     - hesabın imzacı yok
   * - Hesap yeter sayısını ayarla
     - 5
     - yeter sayısı imzacı sayısını aşıyor
   * - Varlık miktarı çıkar
     - 3
     - hesapta böyle bir varlık yok
   * - Varlık miktarı çıkar
     - 4
     - çıkarma miktarı negatif yapar
   * - Varlık aktar
     - 3
     - böyle bir kaynak hesap yok
   * - Varlık aktar
     - 4
     - böyle bir hedef hesap yok
   * - Varlık aktar
     - 5
     - böyle bir varlık yok
   * - Varlık aktar
     - 6
     - yeterli bakiye yok
   * - Varlık aktar
     - 7
     - miktar taşması
   * - Varlık aktar
     - 8
     - açıklama çok uzun

Yukarıda listelenmeyen komutlar (``AddPeer``, ``SetSettingValue``,
``AddSyncPeer``, ``RemoveSyncPeer``) yalnızca evrensel
``0``–``2`` kodlarını yayarlar. ``AddSyncPeer``/``RemoveSyncPeer`` hata ad alanları
başlık dosyasında senkronizasyon eş özelliği için bulunur ancak henüz
bunları yaymayan istemciye dönük komut yoktur.

``1007`` ve ``1008`` kodları yalnızca RocksDB komut yürütücüsü (test arka ucu) tarafından yayılır:
``1007`` = izin zaten ayarlı (``grant_permission``), ``1008`` = boş açık anahtar
(``remove_peer``). Postgres arka ucunun eşdeğer kodları yoktur.

.. literalinclude:: ../../../../shared_model/interfaces/command_error_codes.hpp
   :language: cpp
   :caption: shared_model/interfaces/command_error_codes.hpp

Varlık miktarı ekle
-------------------

Amaç
^^^^

Varlık miktarı ekleme komutunun amacı, transaction oluşturucunun hesabındaki bir varlığın miktarını artırmaktır.
Kullanım senaryosu, sistemdeki değiştirilebilir bir varlığın sayısını artırmaktır; bu varlık bir emtiaya (ör. para, altın vb.) olan talep niteliğinde olabilir.

Şema
^^^^

.. code-block:: proto

    message AddAssetQuantity {
        string asset_id = 1;
        string amount = 2;
        optional string description = 3;
    }

.. note::
    Bilinen bir sorun nedeniyle geçersiz bir precision değeri geçtiğinizde herhangi bir istisna almayacağınızı lütfen unutmayın.
    Geçerli aralık: 0 <= precision <= 255


Yapı
^^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Asset ID", "varlığın kimliği", "<asset_name>#<domain_id>", "usd#morgan"
    "Amount", "eklenecek varlığın pozitif miktarı", "> 0", "200.02"
    "Description", "transaction'ın açıklaması", "Açıklamanın maksimum uzunluğu (genesis bloğunda ayarlanır, varsayılan olarak 100*1024)", "Mint assets"

Doğrulama
^^^^^^^^^

1. Varlık ve hesap mevcut olmalıdır
2. Eklenen miktarın hassasiyeti varlığın hassasiyetine eşit olmalıdır
3. Transaction oluşturucusu, varlık ihraç etme iznine sahip bir role sahip olmalıdır

Olası Durum Bağlı Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not add asset quantity", "Dahili hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "No such permissions", "Komutun oluşturucusu varlık miktarı ekleme iznine sahip değil", "Gerekli izni verin"
    "3", "No such asset", "Bu ada veya hassasiyete sahip varlık bulunamadı", "Varlık kimliğinin ve hassasiyetin doğru olduğundan emin olun"
    "4", "Summation overflow", "Sonuç varlık miktarı sistemin destekleyebileceğinden büyük", "Sonuç miktarının 2^256 / 10^asset_precision değerinden küçük olduğundan emin olun"

Eş ekle
-------

Amaç
^^^^

Eş ekleme komutunun amacı, eş ağına bir eşin eklendiği gerçeğini deftere yazmaktır.
AddPeer içeren bir transaction commit edildikten sonra, konsensüs ve senkronizasyon bileşenleri onu kullanmaya başlayacaktır.
Add Peer komutu hakkında `daha fazla bilgi edinebilirsiniz <../../maintenance/add_peer.html>`_.

Şema
^^^^

.. literalinclude:: ../../../../shared_model/schema/primitive.proto
    :language: proto
    :start-at: message Peer {
    :end-before: message AccountDetailRecordId {

.. code-block:: proto

    message AddPeer {
        Peer peer = 1;
    }

Yapı
^^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 10, 30

    "Address", "ağda çözümlenebilir adres (IPv4, IPv6, alan adı vb.)", "çözümlenebilir olmalıdır", "192.168.1.1:50541"
    "Peer key", "konsensüs algoritmasında oy, commit, ret mesajlarını imzalamak için kullanılan eş açık anahtarı", "ed25519 açık anahtarı", "292a8714694095edce6be799398ed5d6244cd7be37eb813106b217d850d261f2"

Doğrulama
^^^^^^^^^

1. Eş anahtarı benzersiz olmalıdır (bu açık anahtara sahip başka bir eş olmamalıdır)
2. Transaction oluşturucusu CanAddPeer iznine sahip bir role sahip olmalıdır
3. Böyle bir ağ adresi daha önce eklenmemiş olmalıdır

Olası Durum Bağlı Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not add peer", "Dahili hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "No such permissions", "Komutun oluşturucusu eş ekleme iznine sahip değil", "Gerekli izni verin"

İmzacı ekle
-----------

Amaç
^^^^

İmzacı ekleme komutunun amacı, hesaba bir tanımlayıcı eklemektir.
Böyle bir tanımlayıcı, başka bir cihazın veya başka bir kullanıcının açık anahtarıdır.

Şema
^^^^

.. code-block:: proto

    message AddSignatory {
        string account_id = 1;
        bytes public_key = 2;
    }

Yapı
^^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Account ID", "imzacı eklenecek hesap", "<account_name>@<domain_id>", "makoto@soramitsu"
    "Public key", "hesaba eklenecek imzacı", "ed25519 açık anahtarı", "359f925e4eeecfdd6aa1abc0b79a6a121a5dd63bb612b603247ea4f8ad160156"

Doğrulama
^^^^^^^^^

İki durum:

    Durum 1. Transaction oluşturucusu, CanAddSignatory iznine sahip olarak kendi hesabına bir imzacı eklemek istiyor

    Durum 2. Transaction oluşturucusuna CanAddSignatory izni verilmiş

Olası Durum Bağlı Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not add signatory", "Dahili hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "No such permissions", "Komutun oluşturucusu imzacı ekleme iznine sahip değil", "Gerekli izni verin"
    "3", "No such account", "İmzacı eklenecek hesap bulunamadı", "Hesap kimliğinin doğru olduğundan emin olun"
    "4", "Signatory already exists", "Hesapta zaten böyle bir imzacı bağlı", "Başka bir imzacı seçin"

Rol ekle
--------

Amaç
^^^^

Rol ekleme komutunun amacı, bir hesabı sistemde oluşturulmuş bir role yükseltmektir; rol, bir hesabın bir işlem (komut veya sorgu) gerçekleştirmek için sahip olması gereken izinler kümesidir.

Şema
^^^^

.. code-block:: proto

    message AppendRole {
       string account_id = 1;
       string role_name = 2;
    }

Yapı
^^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Account ID", "role eklenecek hesabın kimliği", "zaten mevcut", "makoto@soramitsu"
    "Role name", "zaten oluşturulmuş rolün adı", "zaten mevcut", "MoneyCreator"

Doğrulama
^^^^^^^^^

1. Rol sistemde mevcut olmalıdır
2. Transaction oluşturucusunun rol ekleme (CanAppendRole) izni olmalıdır
3. Rol ekleyen hesabın rollerindeki izinler kümesi, eklenen rolün üst kümesi olmalıdır (başka bir deyişle kimse, transaction oluşturucusundan daha güçlü bir rol ekleyemez)

Olası Durum Bağlı Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not append role", "Dahili hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "No such permissions", "Komutun oluşturucusu rol ekleme iznine sahip değil", "Gerekli izni verin"
    "3", "No such account", "Rol eklenecek hesap bulunamadı", "Hesap kimliğinin doğru olduğundan emin olun"
    "4", "No such role", "Bu ada sahip rol bulunamadı", "Rol kimliğinin doğru olduğundan emin olun"

Motor çağır
-----------

Amaç
^^^^

Motor çağırma komutunun amacı, Iroha EVM'e yeni bir kontrat dağıtmak veya zaten var olan bir akıllı kontratın bir metodunu çağırmaktır.
Bir akıllı kontratın yürütülmesi, bu komutu içeren transaction kabul edilip blok commit edildiğinde defterin durumunu değiştirebilir.

Şema
^^^^

.. code-block:: proto

    message CallEngine {
        string caller = 1;
        oneof opt_callee {
            string callee = 2;  // hex string
        }
        string input = 3;   // hex string
    }

Yapı
^^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"

    "Caller", "komutun adına çalıştırıldığı hesabın Iroha hesap kimliği", "<account_name>@<domain_id>", "test@mydomain"
    "Callee", "dağıtılmış akıllı kontratın EVM adresi", "hex gösteriminde 20 baytlık string", "7C370993FD90AF204FD582004E2E54E6A94F2651"
    "Input", "yeni dağıtılan kontratlar için akıllı kontratın bytecode'u veya `metod seçici ve argüman kümesinin ABI kodlaması <https://solidity.readthedocs.io/en/v0.6.5/abi-spec.html>`_", "Hex string", "40c10f19000000000000000000000000969453762b0c739dd285b31635efa00e24c2562800000000000000000000000000000000000000000000000000000000000004d2"

Doğrulama
^^^^^^^^^

1. Caller geçerli bir Iroha hesap kimliğidir
2. Transaction oluşturucusunun can_call_engine veya can_call_engine_on_my_behalf izninden birine sahip bir rolü vardır

Olası Durum Bağlı Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Engine is not configured", "Bu hata, Iroha'nın Burrow EVM olmadan derlendiği anlamına gelir", "Iroha'yı doğru şekilde derlemek için dokümantasyonun `Derleme <../../build/index.html#main-parameters>`_ bölümüne bakın"
    "2", "No such permissions", "Komutun oluşturucusu EVM motorunu çağırma iznine sahip değil", "Gerekli izni verin"
    "3", "CallEngine error", "EVM'de kod yürütme başarısız oldu; neden hem kontrat kodunun kendisinde hem de iç içe Iroha komutu çağrılarında olabilir", "Sorunu teşhis etmek için hatanın kök nedeninin araştırılması gerekir"

Hesap oluştur
-------------

Amaç
^^^^

Hesap oluşturma komutunun amacı, transaction veya sorgu gönderebilen, imzacıları, kişisel verileri ve tanımlayıcıları saklayabilen bir varlığı sistemde oluşturmaktır.

Şema
^^^^

.. code-block:: proto

    message CreateAccount {
        string account_name = 1;
        string domain_id = 2;
        bytes public_key = 3;
    }

Yapı
^^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Account name", "hesap için domain içinde benzersiz ad", "`[a-z_0-9]{1,32}`", "morgan_stanley"
    "Domain ID", "ilişki kurulacak hedef domain", "hesaptan önce oluşturulmuş olmalıdır", "america"
    "Public key", "hesaba eklenecek ilk açık anahtar", "ed25519 açık anahtarı", "407e57f50ca48969b08ba948171bb2435e035d82cec417e18e4a38f5fb113f83"

Doğrulama
^^^^^^^^^

1. Transaction oluşturucusunun hesap oluşturma izni vardır
2. domain_id olarak verilen domain sistemde daha önce oluşturulmuştur
3. Bu açık anahtar daha önce hesabın ilk açık anahtarı olarak eklenmemiş veya çoklu imza hesabına eklenmemiştir

Olası Durum Bağlı Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not create account", "Dahili hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "No such permissions", "Komutun oluşturucusu ya hesap oluşturma iznine sahip değil ya da oluşturucunun bulunduğundan daha ayrıcalıklı bir domain'de hesap oluşturmaya çalışıyor", "Gerekli izni verin veya başka bir domain seçin"
    "3", "No such domain", "Bu ada sahip domain bulunamadı", "Domain kimliğinin doğru olduğundan emin olun"
    "4", "Account already exists", "Bu domain'de bu ada sahip hesap zaten var", "Başka bir ad seçin"

Varlık oluştur
--------------

Amaç
^^^^

Varlık oluşturma komutunun amacı, bir domain içinde benzersiz olan yeni bir varlık türü oluşturmaktır.
Varlık, bir emtianın sayılabilir bir temsilidir.

Şema
^^^^

.. code-block:: proto

    message CreateAsset {
        string asset_name = 1;
        string domain_id = 2;
        uint32 precision = 3;
    }

.. note::
    Bilinen bir sorun nedeniyle geçersiz bir precision değeri geçtiğinizde herhangi bir istisna almayacağınızı lütfen unutmayın.
    Geçerli aralık: 0 <= precision <= 255

Yapı
^^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Asset name", "varlık için domain içinde benzersiz ad", "`[a-z_0-9]{1,32}`", "soracoin"
    "Domain ID", "ilişki kurulacak hedef domain", "RFC1035 [#f1]_, RFC1123 [#f2]_", "japan"
    "Precision", "virgül/nokta sonrası basamak sayısı", "0 <= precision <= 255", "2"

Doğrulama
^^^^^^^^^

1. Transaction oluşturucusunun varlık oluşturma izni vardır
2. Varlık adı domain içinde benzersizdir

Olası Durum Bağlı Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not create asset", "Dahili hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "No such permissions", "Komutun oluşturucusu varlık oluşturma iznine sahip değil", "Gerekli izni verin"
    "3", "No such domain", "Bu ada sahip domain bulunamadı", "Domain kimliğinin doğru olduğundan emin olun"
    "4", "Asset already exists", "Bu ada sahip varlık zaten var", "Başka bir ad seçin"

Domain oluştur
--------------

Amaç
^^^^

Domain oluşturma komutunun amacı, Iroha ağında yeni bir domain oluşturmaktır; domain, bir grup hesaptır.

Şema
^^^^

.. code-block:: proto

    message CreateDomain {
        string domain_id = 1;
        string default_role = 2;
    }

Yapı
^^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Domain ID", "oluşturulan domain için kimlik", "benzersiz, RFC1035 [#f1]_, RFC1123 [#f2]_", "japan05"
    "Default role", "domain içindeki yeni oluşturulan kullanıcılar için rol", "mevcut rollerden biri", "User"

Doğrulama
^^^^^^^^^

1. Domain kimliği benzersiz olmalıdır
2. Bu komutu transaction içinde gönderen hesabın, domain oluşturma iznine sahip bir rolü vardır
3. Oluşturulan kullanıcıya varsayılan olarak atanacak rol sistemde mevcuttur

Olası Durum Bağlı Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not create domain", "Dahili hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "No such permissions", "Komutun oluşturucusu domain oluşturma iznine sahip değil", "Gerekli izni verin"
    "3", "Domain already exists", "Bu ada sahip domain zaten var", "Başka bir domain adı seçin"
    "4", "No default role found", "Domain için varsayılan olarak sağlanan rol bulunamadı", "Sağladığınız rolün var olduğundan emin olun veya oluşturun"

Rol oluştur
-----------

Amaç
^^^^

Rol oluşturma komutunun amacı, sistemde izinler kümesinden yeni bir rol oluşturmaktır.
Farklı izinleri rollerle birleştirerek Iroha eş ağının yöneticileri özelleştirilmiş güvenlik modeli oluşturabilir.

Şema
^^^^

.. code-block:: proto

    message CreateRole {
        string role_name = 1;
        repeated RolePermission permissions = 2;
    }

Yapı
^^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Role name", "oluşturulacak rolün adı", "`[a-z_0-9]{1,32}`", "User"
    "RolePermission", "zaten mevcut izinlerin dizisi", "verilen izinler kümesi mevcut izinler kümesine tamamen dahildir", "{can_receive, can_transfer}"

Doğrulama
^^^^^^^^^

1. Verilen izinler kümesi mevcut izinler kümesine tamamen dahildir
2. İzinler kümesi boş değildir

Olası Durum Bağlı Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not create role", "Dahili hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "No such permissions", "Komutun oluşturucusu rol oluşturma iznine sahip değil", "Gerekli izni verin"
    "3", "Role already exists", "Bu ada sahip rol zaten var", "Başka bir rol adı seçin"

Rolü ayır
----------

Amaç
^^^^^

Rolü ayır komutunun amacı, bir hesabın roller kümesinden bir rolü ayırmaktır.
Bu komutu çalıştırarak, kullanıcı için sistemdeki olası eylemlerin sayısı azaltılabilir.

Şema
^^^^

.. code-block:: proto

    message DetachRole {
        string account_id = 1;
        string role_name = 2;
    }

Yapı
^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap ID", "Rolün silineceği hesabın ID'si", "zaten mevcut", "makoto@soramitsu"
    "Rol adı", "ayrılacak rol adı", "mevcut rol", "User"

Doğrulama
^^^^^^^^^

1. Rol sistemde mevcut olmalıdır
2. Hesapta bu rol bulunmalıdır

Olası Stateful Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not detach role", "İç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "No such permissions", "Komut oluşturucusunun rolü ayırma izni yok", "Gerekli izni verin"
    "3", "No such account", "Rolün ayrılacağı hesap bulunamıyor", "Hesap id'sinin doğru olduğundan emin olun"
    "4", "No such role in account's roles", "Bu id'ye sahip hesapta bu ada sahip rol yok", "Hesap-rol çiftinin doğru olduğundan emin olun"
    "5", "No such role", "Bu ada sahip rol mevcut değil", "Rol id'sinin doğru olduğundan emin olun"

İzin ver
----------

Amaç
^^^^^

İzin ver komutunun amacı, işlem gönderenin hesabında eylem gerçekleştirme haklarını başka bir hesaba vermektir (birine hesabımla bir şey yapma hakkı vermek).

Şema
^^^^

.. code-block:: proto

    message GrantPermission {
        string account_id = 1;
        GrantablePermission permission = 2;
    }

Yapı
^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap ID", "hakların verildiği hesabın id'si", "zaten mevcut", "makoto@soramitsu"
    "GrantablePermission adı", "verilebilir iznin adı", "izin tanımlı", "CanTransferAssets"


Doğrulama
^^^^^^^^^

1. Hesap mevcut olmalıdır
2. İşlem oluşturucusunun bu izni vermesine izin verilmiş olmalıdır

Olası Stateful Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not grant permission", "İç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "No such permissions", "Komut oluşturucusunun izin verme izni yok", "Gerekli izni verin"
    "3", "No such account", "İzin verilecek hesap bulunamıyor", "Hesap id'sinin doğru olduğundan emin olun"

Eş düğümü kaldır
----------------

Amaç
^^^^^

Eş düğümü kaldır komutunun amacı, bir eş düğümün ağdan kaldırıldığının kaydını deftere yazmaktır.
RemovePeer içeren bir işlem commit edildikten sonra, konsensüs ve senkronizasyon bileşenleri bunu kullanmaya başlayacaktır.

Şema
^^^^

.. code-block:: proto

    message RemovePeer {
        bytes public_key = 1; // hex string
    }

Yapı
^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 10, 30

    "Public key", "eş düğüm public key, konsensüs algoritmasında oy mesajlarını imzalamak için kullanılır", "ed25519 public key", "292a8714694095edce6be799398ed5d6244cd7be37eb813106b217d850d261f2"

Doğrulama
^^^^^^^^^

1. Ağda birden fazla eş düğüm olmalıdır
2. İşlemin oluşturucusu CanRemovePeer iznine sahip bir role sahip olmalıdır
3. Eş düğüm daha önce ağa eklenmiş olmalıdır

Olası Stateful Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not remove peer", "İç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "No such permissions", "Komut oluşturucusunun eş düğüm kaldırma izni yok", "Gerekli izni verin"
    "3", "No such peer", "Bu public key'e sahip eş düğüm bulunamıyor", "Public key'in doğru olduğundan emin olun"
    "4", "Network size does not allow to remove peer", "Eş düğüm kaldırıldıktan sonra ağ boş kalacak", "Ağın en az iki eş düğüm bulunduğundan emin olun"

İmzacı kaldır
----------------

Amaç
^^^^^

İmzacı kaldır komutunun amacı, bir kimliğe bağlı public key'i bir hesaptan kaldırmaktır.

Şema
^^^^

.. code-block:: proto

    message RemoveSignatory {
        string account_id = 1;
        bytes public_key = 2;
    }

Yapı
^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap ID", "hakların verildiği hesabın id'si", "zaten mevcut", "makoto@soramitsu"
    "Public key", "Silinecek imzacı", "ed25519 public key", "407e57f50ca48969b08ba948171bb2435e035d82cec417e18e4a38f5fb113f83"

Doğrulama
^^^^^^^^^

1. İmzacı silinirken, **size(signatories) >= quorum** invariant'ının sağlandığı kontrol edilmelidir
2. İmzacı daha önce hesaba eklenmiş olmalıdır

İki durum:

    Durum 1. İşlem oluşturucusu imzacıyı kendi hesabından kaldırmak istediğinde ve CanRemoveSignatory iznine sahiptir.

    Durum 2. CanRemoveSignatory işlem oluşturucusuna verilmiş.

Olası Stateful Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not remove signatory", "İç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "No such permissions", "Komut oluşturucusunun kendi hesabından imzacı kaldırma izni yok", "Gerekli izni verin"
    "3", "No such account", "İmzacı kaldırılacak hesap bulunamıyor", "Hesap id'sinin doğru olduğundan emin olun"
    "4", "No such signatory", "Bu public key'e sahip imzacı bulunamıyor", "Public key'in doğru olduğundan emin olun"
    "5", "Quorum does not allow to remove signatory", "İmzacı kaldırıldıktan sonra hesap, quorum'un izin verdiğinden daha az imzacıya sahip kalacak", "Quorum'u azaltın"

İzni geri al
-----------------

Amaç
^^^^^

İzni geri al komutunun amacı, ağdaki başka bir hesaptan verilmiş izni geri almak veya iptal etmektir.

Şema
^^^^

.. code-block:: proto

    message RevokePermission {
        string account_id = 1;
        GrantablePermission permission = 2;
    }

Yapı
^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: auto

        "Hesap ID", "hakların verildiği hesabın id'si", "zaten mevcut", "makoto@soramitsu"
        "GrantablePermission adı", "verilebilir iznin adı", "izin verilmiş", "CanTransferAssets"

Doğrulama
^^^^^^^^^

İşlem oluşturucusu bu izni daha önce hedef hesaba vermiş olmalıdır.

Olası Stateful Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not revoke permission", "İç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "No such permissions", "Komut oluşturucusunun izin geri alma izni yok", "Gerekli izni verin"
    "3", "No such account", "İznin geri alınacağı hesap bulunamıyor", "Hesap id'sinin doğru olduğundan emin olun"

Hesap detayı ayarla
-------------------

Amaç
^^^^^

Hesap detayı ayarla komutunun amacı, verilen hesap için anahtar-değer bilgisi ayarlamaktır.

.. warning:: Verilen anahtar için depoda zaten bir değer varsa, yeni değerle değiştirilecektir.

Şema
^^^^

.. code-block:: proto

    message SetAccountDetail{
        string account_id = 1;
        string key = 2;
        string value = 3;
    }

Yapı
^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap ID", "anahtar-değer bilgisinin ayarlandığı hesabın id'si", "zaten mevcut", "makoto@soramitsu"
    "Anahtar", "ayarlanan bilginin anahtarı", "`[A-Za-z0-9_]{1,64}`", "Name"
    "Değer", "ilgili anahtarın değeri", "≤ 4096", "Makoto"

Doğrulama
^^^^^^^^^

Üç durum:

    Durum 1. İşlem oluşturucusu başka birinin hesabına hesap detayı ayarlamak istediğinde ve `can_set_detail <../api/permissions.html#can-set-detail>`_ iznine sahiptir.

    Durum 2. `can_set_my_account_detail <../api/permissions.html#can-set-my-account-detail>`_ izinleri işlem oluşturucusuna verilmiş ve hedef hesabın hesap detaylarını ayarlamasına izin verir.

    Durum 3. Hesap sahibi kendi hesap detaylarını ayarlamak istediğinde – bu durumda izne gerek yoktur.

Olası Stateful Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not set account detail", "İç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "No such permissions", "Komut oluşturucusunun başka bir hesap için hesap detayı ayarlama izni yok", "Gerekli izni verin"
    "3", "No such account", "Hesap detayı ayarlanacak hesap bulunamıyor", "Hesap id'sinin doğru olduğundan emin olun"

Hesap quorum'unu ayarla
-----------------------

Amaç
^^^^^

Hesap quorum'unu ayarla komutunun amacı, işlemi oluşturan kullanıcının kimliğini doğrulamak için gereken imzacı sayısını ayarlamaktır.
Kullanım senaryosu, tek bir hesabı kullanan farklı kullanıcıların sayısını, işlemi imzalamak üzere ayarlamaktır.

Şema
^^^^

.. code-block:: proto

    message SetAccountQuorum {
        string account_id = 1;
        uint32 quorum = 2;
    }

Yapı
^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap ID", "quorum ayarlanacak hesabın ID'si", "zaten mevcut", "makoto@soramitsu"
    "Quorum", "bu hesaptan bir işleme dahil edilmesi gereken imzacı sayısı", "0 < quorum ≤ hesaba eklenmiş public-key kümesi ≤ 128", "5"

Doğrulama
^^^^^^^^^

Quorum ayarlandığında, **size(signatories) >= quorum** invariant'ının sağlandığı kontrol edilir.

İki durum:

    Durum 1. İşlem oluşturucusu kendi hesabı için quorum ayarlamak istediğinde ve CanRemoveSignatory iznine sahiptir.

    Durum 2. CanRemoveSignatory işlem oluşturucusuna verilmiş.

Olası Stateful Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not set quorum", "İç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "No such permissions", "Komut oluşturucusunun kendi hesabı için quorum ayarlama izni yok", "Gerekli izni verin"
    "3", "No such account", "Quorum ayarlanacak hesap bulunamıyor", "Hesap id'sinin doğru olduğundan emin olun"
    "4", "No signatories on account", "Hesaba bağlı hiçbir imzacı bulunamıyor", "Quorum ayarlamadan önce bazı imzacılar ekleyin"
    "5", "New quorum is incorrect", "Yeni quorum boyutu hesabın imzacı sayısından az", "Başka bir değer seçin veya daha fazla imzacı ekleyin"

Varlık miktarını azalt
-----------------------

Amaç
^^^^^

Varlık miktarını azalt komutunun amacı, AddAssetQuantity komutlarının tersidir — işlem oluşturucunun hesabındaki varlık sayısını azaltmak.

Şema
^^^^

.. code-block:: proto

    message SubtractAssetQuantity {
        string asset_id = 1;
        string amount = 2;
        optional string description = 3;
    }

.. note::
    Bilinen bir sorun nedeniyle, geçersiz precision değeri verirseniz herhangi bir istisna almayacağınızı lütfen unutmayın.
    Geçerli aralık: 0 <= precision <= 255

Yapı
^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Varlık ID", "varlığın id'si", "<asset_name>#<domain_id>", "usd#morgan"
    "Miktar", "azaltılacak varlığın pozitif miktarı", "> 0", "200"
    "Açıklama", "işlemin açıklaması", "Açıklamanın maksimum uzunluğu (genesis bloğunda ayarlanır, varsayılan 100*1024)", "Burn assets"

Doğrulama
^^^^^^^^^

1. Varlık ve hesap mevcut olmalıdır
2. Eklenen miktar precision'ı varlık precision'ına eşit olmalıdır
3. İşlemin oluşturucusu, varlık çıkarmak için izinleri olan bir role sahip olmalıdır

Olası Stateful Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not subtract asset quantity", "İç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "No such permissions", "Komut oluşturucusunun varlık miktarını azaltma izni yok", "Gerekli izni verin"
    "3", "No such asset found", "Hesabın varlıkları arasında bu ada veya precision'a sahip varlık bulunamıyor", "Varlık adının ve precision'ın doğru olduğundan emin olun"
    "4", "Not enough balance", "Hesabın bakiyesi işlemi gerçekleştirmek için çok düşük", "Hesaba varlık ekleyin veya azaltılacak daha düşük değer seçin"

Varlık aktar
--------------

Amaç
^^^^^

Varlık aktar komutunun amacı, peer ağında hesap içindeki varlıkları paylaşmaktır: kaynak hesabın varlıkları hedef hesaba aktarması şeklinde.

Şema
^^^^

.. code-block:: proto

    message TransferAsset {
        string src_account_id = 1;
        string dest_account_id = 2;
        string asset_id = 3;
        string description = 4;
        string amount = 5;
    }

Yapı
^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Kaynak hesap ID", "varlığın çekileceği hesabın ID'si", "zaten mevcut", "makoto@soramitsu"
    "Hedef hesap ID", "varlığın gönderileceği hesabın ID'si", "zaten mevcut", "alex@california"
    "Varlık ID", "aktarılacak varlığın ID'si", "zaten mevcut", "usd#usa"
    "Açıklama", "aktarıma eklenecek mesaj", "Açıklamanın maksimum uzunluğu (genesis bloğunda ayarlanır, varsayılan 100*1024)", "here's my money take it"
    "Miktar", "aktarılacak varlık miktarı", "0 <= precision <= 255", "200.20"

Doğrulama
^^^^^^^^^

1. Kaynak hesabın bu varlığa AccountHasAsset ilişkisi ile sahip olması gerekir [#f1]_
2. Miktar pozitif bir sayıdır ve varlık precision'ı varlık tanımıyla tutarlıdır
3. Kaynak hesabın aktarılacak yeterli varlık miktarına sahip olması gerekir ve sıfır olmamalıdır
4. Kaynak hesabın para aktarabilmesi ve hedef hesabın para alabilmesi gerekir (rollerinde bu izinler olmalıdır)
5. Açıklama uzunluğu 100*1024'ten (yüz kilobayt) ve ayarlanmışsa 'MaxDescriptionSize' ayar değerinden küçük olmalıdır.

Olası Stateful Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not transfer asset", "İç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "No such permissions", "Komut oluşturucusunun kendi hesabından varlık aktarım izni yok", "Gerekli izni verin"
    "3", "No such source account", "Paranın çekileceği bu id'ye sahip hesap bulunamıyor", "Kaynak hesap id'sinin doğru olduğundan emin olun"
    "4", "No such destination account", "Paranın gönderileceği bu id'ye sahip hesap bulunamıyor", "Hedef hesap id'sinin doğru olduğundan emin olun"
    "5", "No such asset found", "Bu varlık bulunamıyor", "Varlık adının ve precision'ın doğru olduğundan emin olun"
    "6", "Not enough balance", "Kaynak hesabın bakiyesi işlemi gerçekleştirmek için çok düşük", "Hesaba varlık ekleyin veya azaltılacak daha düşük değer seçin"
    "7", "Too much asset to transfer", "Hedef hesabın sonuç varlık miktarı izin verilen maksimumu aşacak", "Hedef hesabın nihai değerinin 2^256 / 10^asset_precision'dan küçük olduğundan emin olun"
    "8", "Too long description", "Açıklama çok uzun", "Açıklama uzunluğunun yukarıdaki kriterlerle eşleştiğinden emin olun (veya sadece kısaltın)"

.. [#f1] https://www.ietf.org/rfc/rfc1035.txt
.. [#f2] https://www.ietf.org/rfc/rfc1123.txt

Karşılaştır ve Hesap Detayı Ayarla
----------------------------------

Amaç
^^^^^

Karşılaştır ve hesap detayı ayarla komutunun amacı, verilen hesap için eski değer geçilen değerle eşleşiyorsa anahtar-değer bilgisi ayarlamaktır.

Şema
^^^^

.. code-block:: proto

    message CompareAndSetAccountDetail{
        string account_id = 1;
        string key = 2;
        string value = 3;
        oneof opt_old_value {
            string old_value = 4;
        }
        bool check_empty = 5;
    }

.. note::
    Dikkat edin, old_value alanı isteğe bağlıdır.
    Bunun nedeni, anahtar-değer çiftinin mevcut olmaması ihtimalidir.

Yapı
^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap ID", "anahtar-değer bilgisinin ayarlandığı hesabın id'si. Anahtar-değer çifti mevcut değilse, oluşturulacaktır", "mevcut bir hesap", "artyom@soramitsu"
    "Anahtar", "ayarlanan bilginin anahtarı", "`[A-Za-z0-9_]{1,64}`", "Name"
    "Değer", "ilgili anahtar için yeni değer", "değer uzunluğu ≤ 4096", "Artyom"
    "Eski değer", "ilgili anahtar için mevcut değer", "değer uzunluğu ≤ 4096", "Artem"
    "check_empty", "true ise, komuttaki boş old_value WSV'deki mevcut olmayan değerle eşleşmelidir; false ise, komuttaki herhangi bir old_value WSV'de mevcut olmayanla eşleşir (legacy)", "bool", "true"

Doğrulama
^^^^^^^^^

Üç durum:

    Durum 1. İşlem oluşturucusu kendi hesabına hesap detayı ayarlamak istediğinde ve GetMyAccountDetail / GetAllAccountsDetail / GetDomainAccountDetail iznine sahiptir.

    Durum 2. İşlem oluşturucusu başka bir hesaba hesap detayı ayarlamak istediğinde ve SetAccountDetail ve GetAllAccountsDetail / GetDomainAccountDetail izinlerine sahiptir.

    Durum 3. SetAccountDetail izni işlem oluşturucusuna verilmiş ve GetAllAccountsDetail / GetDomainAccountDetail iznine sahiptir.

Olası Stateful Doğrulama Hataları
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Could not compare and set account detail", "İç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "No such permissions", "Komut oluşturucusunun bu hesap için hesap detayını ayarlama ve okuma izni yok", "Gerekli izni verin"
    "3", "No such account", "Hesap detayı ayarlanacak hesap bulunamıyor", "Hesap id'sinin doğru olduğundan emin olun"
    "4", "No match values", "Eski değerler eşleşmiyor", "Eski değerin doğru olduğundan emin olun"

Ayar değerini ayarla
--------------------

Amaç
^^^^^

Ayar değerini ayarla komutunun amacı, ihtiyaçlarınıza göre özelleştirmeyi sağlamaktır.


Şema
^^^^

.. code-block:: proto

    message SetSettingValue {
        string key = 1;
        string value = 2;
    }

Yapı
^^^^

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Anahtar", "Ayarın anahtarı", "olası ayarların listesi", "MaxDescriptionSize"
    "Değer", "Ayarın değeri", "ayarın tipi", "255"


Doğrulama
^^^^^^^^^

1. Komut yalnızca genesis bloğundan çalıştırılabilir

Olası ayarların listesi
^^^^^^^^^^^^^^^^^^^^^^^^^

.. csv-table::
    :header: "Anahtar", "Değer kısıtı", "Açıklama"

    "MaxDescriptionSize", "İşaretsiz tam sayı, 0 <= MaxDescriptionSize < 2^32", "Maksimum işlem açıklaması uzunluğu"
