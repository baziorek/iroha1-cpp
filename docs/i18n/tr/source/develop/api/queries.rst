Sorgular
========

Sorgu, World State View'un — yani blockchain'in güncel durumunun — belirli bir bölümüne ilişkin bir istektir.
Sorgu, zincirin içeriğini değiştiremez ve yanıt, isteği alan eş, sorguyu işledikten hemen sonra herhangi bir istemciye döndürülür.

Doğrulama
^^^^^^^^^

Tüm sorgular için doğrulama şunları içerir:

- zaman damgası — geçmişten olmamalı (Iroha yapılandırmasında `Iroha configuration <../../configure/index.html#environment-specific-parameters>`_ ile yapılandırılabilir) veya gelecekten olmamalı (eşi zamanına eklenen 5 dakikalık aralık)
- sorgu oluşturucusunun imzası — sorgu oluşturucusunun kimliğini kontrol etmek için kullanılır
- sorgu sayacı — sorgu oluşturucusunun her ardışık sorgusunda artırıldığı kontrol edilir
- roller — sorgu oluşturucusunun rolüne bağlı olarak: sorgulanabilecek durum aralığı aynı hesaba, domaindeki hesaba, tüm zincire ilişkin olabilir veya hiç izin verilmeyebilir

Sonuç Sayfalama
^^^^^^^^^^^^^^^

Bazı sorgular, geliştirmede kullanılabilecek şekilde sorgu sonucunu farklı şekillerde özelleştirmeye ve sıralamaya olanak tanıyan `TxPaginationMeta` destekler.
Sayfalama, SQL dilindeki `ORDER BY <https://www.postgresql.org/docs/12/sql-select.html#SQL-ORDERBY>`_ ile benzer sıralama parametreleriyle birlikte çalışır – "sonuç satırları belirtilen ifadeye göre sıralanır (Iroha'da – Alan). İki satır en soldaki ifadeye göre eşitse, bir sonraki ifadeye göre karşılaştırılır ve böyle devam eder."

"İfade"nin nasıl belirtildiği aşağıdaki gibidir:

.. code-block:: proto

    enum Field {
        kCreatedTime = 0;
        kPosition = 1;
    }

Sıralama için 2 temel vardır – oluşturulma zamanına göre ve blok numarasına bağlı olarak.

Her Alan için artan ve azalan yönler vardır:

.. code-block:: proto

    enum Direction {
        kAscending = 0;
        kDescending = 1;
    }

Şimdi sıralamanın kendisi:

.. code-block:: proto

    message Ordering {
        message FieldOrdering {
          Field field = 1;
          Direction direction = 2;
        }
        repeated FieldOrdering sequence = 1;
    }

Sıralama belirtildikten sonra sayfalama çalıştırılabilir:

.. literalinclude:: ../../../../shared_model/schema/queries.proto
    :language: proto
    :start-at: message TxPaginationMeta {
    :end-before: message AssetPaginationMeta {


Sayfalama durumunda isteğe ne eklenir
-------------------------------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Sayfa boyutu", "sorgu tarafından döndürülecek sayfanın boyutu; yanıt sayfa boyutundan daha az işlem içeriyorsa, yanıttaki sonraki işlem özeti boş olur", "page_size > 0", "5"
    "İlk işlem özeti", "sayfadaki ilk işlemin özeti. Bu alan ayarlanmamışsa — ilk işlemler döndürülür", "onaltılık formatta özet", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
    "ordering", "sonuçların nasıl sıralanması gerektiği (sayfalama uygulanmadan önce)", "aşağıdaki alanlara bakın", "aşağıdaki alanlara bakın"
    "ordering.sequence", "SQL ORDER BY'daki gibi sıralama tanımı", "alanlar ve yönler dizisi", "[{kCreatedTime, kAscending}, {kPosition, kDescending}]"
    "İlk işlem zamanı", "sorgu sonucundaki ilk işlemin zamanı. Bu alan ayarlanmamışsa — ilk işlemden başlayarak işlemler döndürülür", "Google Protocol Buffer Timestamp tipi", "0001-01-01T00:00:00Z <= first tx time <= 9999-12-31T23:59:59.999999999Z"
    "Son işlem zamanı", "sorgu sonucundaki son işlemin zamanı. Bu alan ayarlanmamışsa — son işleme kadar olan işlemler döndürülür", "Google Protocol Buffer Timestamp tipi", "0001-01-01T00:00:00Z <= last tx time <= 9999-12-31T23:59:59.999999999Z"
    "İlk işlem yüksekliği", "sorgu sonucundaki ilk işlemin blok yüksekliği. Bu alan ayarlanmamışsa — 1. yükseklikten başlayarak işlemler döndürülür", "first tx height > 0", "4"
    "Son işlem yüksekliği", "sorgu sonucundaki son işlemin blok yüksekliği. Bu alan ayarlanmamışsa — son işleme kadar olan işlemler döndürülür", "last tx height > 0", "6"
    
Motor Makbuzları
^^^^^^^^^^^^^^^^

Amaç
----

Bir CallEngine komutunun makbuzunu alır.
Ethereum JSON RPC API'sinin eth.GetTransactionReceipt API çağrısına benzer.
EVM içinde hesaplamalar sırasında oluşturulan olay günlüğüne erişmeye olanak tanır.

İstek Şeması
------------

.. code-block:: proto

   message GetEngineReceipts {
       string tx_hash = 1;     // hex string
   }

İstek Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "İşlem Özeti", "CallEngine komutunu içeren işlemin özeti", "onaltılık formatta özet", "5241f70cf3adbc180199c1d2d02db82334137aede5f5ed35d649bbbc75ab2634"

Yanıt Şeması
------------

.. code-block:: proto

    message EngineReceiptsResponse {
        repeated EngineReceipt engine_receipt = 1;
    }
    message EngineReceipt {
        int32 command_index = 1;
        string caller = 2;
        oneof opt_to_contract_address {
            CallResult call_result = 3;
            string contract_address = 4;
        }
        repeated EngineLog logs = 5;
    }
    message CallResult {
        string callee = 1;
        string result_data = 2;
    }
    message EngineLog {
        string address = 1;         // hex string
        string data = 2;            // hex string
        repeated string topics = 3; // hex string
    }

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "command_index", "işlemdeki CallEngine komutunun dizini", "negatif olmayan tam sayı", "0"
    "caller", "akıllı sözleşmenin çağıran hesabı", "<account_name>@<domain_id>", "admin@test"
    "call_result.callee", "çağrılan sözleşmenin adresi", "onaltılık gösterimde 20 baytlık dize", "0000000000000000000000000000000000000000"
    "call_result.result_data", "sözleşme tarafından döndürülen değer", "onaltılık gösterimde dize", "00"
    "contract_address", "yeni dağıtılan sözleşmenin EVM adresi", "onaltılık gösterimde 20 baytlık dize", "7C370993FD90AF204FD582004E2E54E6A94F2651"
    "logs", "akıllı sözleşme yürütmesi sırasında oluşturulan EVM olay günlüklerinin dizisi.", "aşağıya bakın", "aşağıya bakın"
    "logs.[].address", "sözleşme çağıranın EVM adresi", "onaltılık gösterimde 20 baytlık dize", "577266A3CE7DD267A4C14039416B725786605FF4"
    "logs.[].data", "günlüğe kaydedilen veri", "onaltılık dize", "0000000000000000000000007203DF5D7B4F198848477D7F9EE080B207E544DD000000000000000000000000000000000000000000000000000000000000006D"
    "logs.[].topics", "Ethereum'daki gibi günlük konusu", "32 baytlık diziler", "[3990DB2D31862302A685E8086B5755072A6E2B5B780AF1EE81ECE35EE3CD3345, 000000000000000000000000969453762B0C739DD285B31635EFA00E24C25628]"


Olası Durumlu Doğrulama Hataları
---------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "2", "Böyle bir izin yok", "Sorgunun oluşturucusunun motor makbuzu alma izinlerinden herhangi birine sahip değil", "Gerekli izni verin"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamadan geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"

Hesap Al
^^^^^^^^

Amaç
----

Hesap al sorgusunun amacı bir hesabın durumunu almaktır.

İstek Şeması
------------

.. code-block:: proto

    message GetAccount {
        string account_id = 1;
    }

İstek Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap Kimliği", "durumunu istemek için hesap kimliği", "<account_name>@<domain_id>", "alex@morgan"

Yanıt Şeması
------------

.. code-block:: proto

    message AccountResponse {
        Account account = 1;
        repeated string account_roles = 2;
    }

    message Account {
        string account_id = 1;
        string domain_id = 2;
        uint32 quorum = 3;
        string json_data = 4;
    }


Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap Kimliği", "hesap kimliği", "<account_name>@<domain_id>", "alex@morgan"
    "Domain Kimliği", "hesabın oluşturulduğu domain", "RFC1035 [#f1]_, RFC1123 [#f2]_ ", "morgan"
    "Yeter Sayı", "işlemi geçerli kılmak için imzalamak üzere işlem imzalamak gereken imzacı sayısı", "0 < quorum ≤ 128", "5"
    "JSON verisi", "anahtar-değer hesap bilgisi", "JSON", "{ genesis: {name: alex} }"

Olası Durumlu Doğrulama Hataları
---------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Hesap alınamadı", "İç hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "Böyle bir izin yok", "Sorgunun oluşturucusunun hesap alma izinlerinden herhangi birine sahip değil", "Gerekli izni verin: bireysel, genel veya domain izni"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamadan geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"

Blok Al
^^^^^^^

Amaç
----

Blok al sorgusunun amacı, tanımlayıcı olarak yüksekliğini kullanarak belirli bir bloğu almaktır

İstek Şeması
------------

.. code-block:: proto

    message GetBlock {
        uint64 height = 1;
    }


İstek Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Yükseklik", "alınacak bloğun yüksekliği", "0 < height < 2^64", "42"

Yanıt Şeması
------------

.. code-block:: proto

    message BlockResponse {
        Block block = 1;
    }

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Block", "alınan blok", "blok yapısı", "block"

Olası Durumlu Doğrulama Hataları
---------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Blok alınamadı", "İç hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "Böyle bir izin yok", "Sorgunun oluşturucusunun blok alma izni yok", "`can_get_block <permissions.html#can-get-blocks>`__ iznini verin"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamadan geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"
    "3", "Geçersiz yükseklik", "Verilen yükseklik uint_64 değil veya defterin yüksekliğinden büyük", "Yüksekliği kontrol edin ve tekrar deneyin"

.. note::
    Bu sorgu için 3 hata kodu belirsizdir.
    Ya geçersiz imzacılara ya da geçersiz yüksekliğe işaret eder.
    Geçersiz imzacıları kontrol etmek için bu yöntemi `height = 1` ile kullanın (ilk blok her zaman mevcuttur).

İmzacıları Al
^^^^^^^^^^^^^

Amaç
----

İmzacıları al sorgusunun amacı, hesabın kimliği olarak görev yapan imzacıları almaktır.

İstek Şeması
------------

.. code-block:: proto

    message GetSignatories {
        string account_id = 1;
    }

İstek Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap Kimliği", "imzacılarını istemek için hesap kimliği", "<account_name>@<domain_id>", "alex@morgan"

Yanıt Şeması
------------

.. code-block:: proto

    message SignatoriesResponse {
        repeated bytes keys = 1;
    }

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Keys", "genel anahtarların dizisi", "`ed25519 <https://ed25519.cr.yp.to>`_", "292a8714694095edce6be799398ed5d6244cd7be37eb813106b217d850d261f2"

Olası Durumlu Doğrulama Hataları
---------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "İmzacılar alınamadı", "İç hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "Böyle bir izin yok", "Sorgunun oluşturucusunun imzacıları alma izinlerinden herhangi birine sahip değil", "Gerekli izni verin: bireysel, genel veya domain izni"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamadan geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"

İşlemleri Al
^^^^^^^^^^^^

Amaç
----

GetTransactions, işlemler hakkındaki bilgileri özütlerine göre almak için kullanılır.

.. note:: Bu sorgu ancak ve ancak istenen tüm özütler doğruysa geçerlidir: ilgili işlemler mevcutsa ve kullanıcının bunları alma izni varsa

İstek Şeması
------------

.. code-block:: proto

    message GetTransactions {
        repeated bytes tx_hashes = 1;
    }

İstek Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "İşlem özetleri", "özetlerin dizisi", "32 baytlık özütlerden oluşan dizi", "{hash1, hash2…}"

Yanıt Şeması
------------

.. code-block:: proto

    message TransactionsResponse {
        repeated Transaction transactions = 1;
    }

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Transactions", "işlemlerin dizisi", "İşlenmiş işlemler", "{tx1, tx2…}"

Olası Durumlu Doğrulama Hataları
---------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "İşlemler alınamadı", "İç hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "Böyle bir izin yok", "Sorgunun oluşturucusunun işlemleri alma izinlerinden herhangi birine sahip değil", "Gerekli izni verin: bireysel, genel veya domain izni"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamadan geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"
    "4", "Geçersiz özet", "Verilen özetlerden en az biri kullanıcının işlem listesinde yok veya sorgu oluşturucusunun görme izni yok", "Verilen özetleri kontrol edin ve tekrar deneyin"

Bekleyen İşlemleri Al
^^^^^^^^^^^^^^^^^^^^^

Amaç
----

GetPendingTransactions, sorgu oluşturucusunun hesabı tarafından oluşturulan bekleyen (tamamen imzalanmamış) `çoklu imza işlemlerini <../../concepts_architecture/glossary.html#multisignature-transactions>`_
veya `işlem gruplarını <../../concepts_architecture/glossary.html#batch-of-transactions>`__ almak için kullanılır.

.. note:: Bu sorgu, daha hızlı ve daha uygun sorgu yanıtları için `sayfalama <#result-pagination>`_ kullanır. Lütfen bunu okuyun ve sorgu isteğini göndermeden önce sayfalama belirleyin, ayrıca `istek yapısını <#what-is-added-to-the-request-structure-in-case-of-pagination>`_ belirtin.

İstek Şeması
------------

.. code-block:: proto

    message GetPendingTransactions {
        TxPaginationMeta pagination_meta = 1;
    }

Kullanıcının yarı imzalı tüm çoklu imza (bekleyen) işlemleri sorgulanabilir.
Yanıtta bulunabilecek işlem sayısı en fazla **page_size** alanı ile sınırlandırılabilir.
Tüm bekleyen işlemler, yeterli imzaları toplayana veya süresi dolana kadar saklanır.
Bekleyen işlemlerin veya işlem gruplarının birbirine göre sırası kullanıcı için korunur.
Bu, kullanıcının tüm işlemleri sırayla — sayfa sayfa — sorgulamasına olanak tanır.
Her yanıt, sorgulanabilecek sonraki grup veya işleme referans içerebilir.
Sayfa boyutu, aşağıdaki grubun (işlemlerde) boyutundan büyük olabilir.
Bu durumda, birkaç grup veya işlem döndürülür.
Sayfalar arasında gezinirken, sorgulanmadan önce aşağıdaki grup eksik imzaları toplayabilir.
Bu, grubun özeti eksik olduğu için durumlu başarısız sorgu yanıtıyla sonuçlanır.

Örnek
-----

Her birinde üç işlem olan iki bekleyen grup varsa ve kullanıcı bekleyen işlemleri 5 sayfa boyutuyla sorgularsa,
ilk grubun işlemleri yanıtta yer alır ve ikinci gruba referans
(ikinci işlemin ilk işlem özeti ve grup boyutu, gerçekte tek işlem olsa bile) da belirtilir.
İkinci grubun işlemleri ilk yanıta dahil edilmez çünkü grup birkaç parçaya bölünemez
ve yalnızca tam gruplar yanıt içinde yer alabilir.

Yanıt Şeması
------------

.. code-block:: proto

    message PendingTransactionsPageResponse {
        message BatchInfo {
            string first_tx_hash = 1;
            uint32 batch_size = 2;
        }
        repeated Transaction transactions = 1;
        uint32 all_transactions_size = 2;
        BatchInfo next_batch_info = 3;
    }

Yanıt Yapısı
------------

Yanıt, bir liste `bekleyen işlemi <../../concepts_architecture/glossary.html#pending-transactions>`_,
kullanıcı için saklanan tüm bekleyen işlemlerin sayısını
ve sonraki sayfayı sorgulamak için gereken bilgiyi (varsa) içerir.

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

        "Transactions", "bekleyen işlemlerin dizisi", "Bekleyen işlemler", "{tx1, tx2…}"
        "All transactions size", "saklanan işlemlerin sayısı", "all_transactions_size >= 0", "0"
        "Next batch info", "sonraki sayfaya referans — yanıt içinde bu ileti ayarlanmamış olabilir", "", ""
        "İlk işlem özeti", "sonraki gruptaki ilk işlemin özeti",  "onaltılık formatta özet", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
        "Batch size", "sonraki grubu getirmek için gereken minimum sayfa boyutu", "batch_size > 0", "3"

Bekleyen İşlemleri Al (kullanımdan kaldırılmış)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. warning::
  Parametresiz sorgu artık kullanımdan kaldırılmıştır ve bir sonraki büyük Iroha sürümünde (2.0) kaldırılacaktır.
  Lütfen bunun yerine yeni sorgu sürümünü kullanın: `Bekleyen İşlemleri Al <#get-pending-transactions>`__.

Amaç
----

GetPendingTransactions, sorgu oluşturucusunun hesabı tarafından oluşturulan bekleyen (tamamen imzalanmamış) `çoklu imza işlemlerini <../../concepts_architecture/glossary.html#multisignature-transactions>`_
veya `işlem gruplarını <../../concepts_architecture/glossary.html#batch-of-transactions>`__ almak için kullanılır.

İstek Şeması
------------

.. code-block:: proto

    message GetPendingTransactions {
    }

Yanıt Şeması
------------

.. code-block:: proto

    message TransactionsResponse {
        repeated Transaction transactions = 1;
    }

Yanıt Yapısı
------------

Yanıt, bir liste `bekleyen işlemi <../../concepts_architecture/glossary.html#pending-transactions>`_ içerir.

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

        "Transactions", "bekleyen işlemlerin dizisi", "Bekleyen işlemler", "{tx1, tx2…}"

Olası Durumlu Doğrulama Hataları
---------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Bekleyen işlemler alınamadı", "İç hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "Böyle bir izin yok", "Sorgunun oluşturucusunun bekleyen işlemleri alma izinlerinden herhangi birine sahip değil", "Gerekli izni verin: bireysel, genel veya domain izni"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamadan geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"

Hesap İşlemlerini Al
^^^^^^^^^^^^^^^^^^^^

Amaç
----

Hesap başına işlem listesine ihtiyaç duyulduğunda `GetAccountTransactions` sorgusu oluşturulabilir.

.. note:: Bu sorgu, daha hızlı ve daha uygun sorgu yanıtları için `sayfalama <#result-pagination>`_ kullanır. Lütfen bunu okuyun ve sorgu isteğini göndermeden önce sayfalama belirleyin, ayrıca `istek yapısını <#what-is-added-to-the-request-structure-in-case-of-pagination>`_ belirtin.

İstek Şeması
------------

.. code-block:: proto

    message GetAccountTransactions {
        string account_id = 1;
        TxPaginationMeta pagination_meta = 2;
    }

İstek Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap Kimliği", "işlemlerini istemek için hesap kimliği", "<account_name>@<domain_id>", "makoto@soramitsu"

Yanıt Şeması
------------

.. code-block:: proto

    message TransactionsPageResponse {
        repeated Transaction transactions = 1;
        uint32 all_transactions_size = 2;
        oneof next_page_tag {
            string next_tx_hash = 3;
        }
    }

Olası Durumlu Doğrulama Hataları
---------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Hesap işlemleri alınamadı", "İç hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "Böyle bir izin yok", "Sorgunun oluşturucusunun hesap işlemlerini alma izinlerinden herhangi birine sahip değil", "Gerekli izni verin: bireysel, genel veya domain izni"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamadan geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"
    "4", "Geçersiz sayfalama özeti", "Verilen özet, kullanıcının işlemlerinden hiçbirinde görünmüyor", "Özetin doğru olduğundan emin olun ve tekrar deneyin"
    "5", "Geçersiz hesap kimliği", "Böyle bir hesap kimliğine sahip kullanıcı yok", "Hesap kimliğinin doğru olduğundan emin olun"

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Transactions", "verilen hesap için işlemlerin dizisi", "İşlenmiş işlemler", "{tx1, tx2…}"
    "All transactions size", "verilen hesap tarafından oluşturulan toplam işlem sayısı", "", "100"
    "Next transaction hash", "sayfadaki son işlemden sonraki işleme işaret eden özet. Sayfa, verilen hesap için son işlemi içeriyorsa boştur", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"

Hesap Varlık İşlemlerini Al
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Amaç
----

`GetAccountAssetTransactions` sorgusu, verilen hesap ve varlıkla ilişkili tüm işlemleri döndürür.

.. note:: Bu sorgu, daha hızlı ve daha uygun sorgu yanıtları için `sayfalama <#result-pagination>`_ kullanır. Lütfen bunu okuyun ve sorgu isteğini göndermeden önce sayfalama belirleyin, ayrıca `istek yapısını <#what-is-added-to-the-request-structure-in-case-of-pagination>`_ belirtin.

İstek Şeması
------------

.. code-block:: proto

    message GetAccountAssetTransactions {
        string account_id = 1;
        string asset_id = 2;
        TxPaginationMeta pagination_meta = 3;
    }

İstek Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap Kimliği", "işlemlerini istemek için hesap kimliği", "<account_name>@<domain_id>", "makoto@soramitsu"
    "Varlık Kimliği", "bu varlığı içeren işlemleri filtrelemek için varlık kimliği", "<asset_name>#<domain_id>", "jpy#japan"

Yanıt Şeması
------------

.. code-block:: proto

    message TransactionsPageResponse {
        repeated Transaction transactions = 1;
        uint32 all_transactions_size = 2;
        oneof next_page_tag {
            string next_tx_hash = 3;
        }
    }

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Transactions", "verilen hesap ve varlık için işlemlerin dizisi", "İşlenmiş işlemler", "{tx1, tx2…}"
    "All transactions size", "verilen hesap ve varlık için toplam işlem sayısı", "", "100"
    "Next transaction hash", "sayfadaki son işlemden sonraki işleme işaret eden özet. Sayfa, verilen hesap ve varlık için son işlemi içeriyorsa boştur", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"

Olası Durumlu Doğrulama Hataları
---------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Hesap varlık işlemleri alınamadı", "İç hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "Böyle bir izin yok", "Sorgunun oluşturucusunun hesap varlık işlemlerini alma izinlerinden herhangi birine sahip değil", "Gerekli izni verin: bireysel, genel veya domain izni"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamadan geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"
    "4", "Geçersiz sayfalama özeti", "Verilen özet, kullanıcının işlemlerinden hiçbirinde görünmüyor", "Özetin doğru olduğundan emin olun ve tekrar deneyin"
    "5", "Geçersiz hesap kimliği", "Böyle bir hesap kimliğine sahip kullanıcı yok", "Hesap kimliğinin doğru olduğundan emin olun"
    "6", "Geçersiz varlık kimliği", "Böyle bir varlık kimliğine sahip varlık yok", "Varlık kimliğinin doğru olduğundan emin olun"

Hesap Varlıklarını Al
^^^^^^^^^^^^^^^^^^^^^

Amaç
----

Bir hesaptaki (bakiye) tüm varlıkların durumunu almak için `GetAccountAssets` sorgusu kullanılabilir.

İstek Şeması
------------

.. code-block:: proto

    message AssetPaginationMeta {
        uint32 page_size = 1;
        oneof opt_first_asset_id {
            string first_asset_id = 2;
        }
    }

    message GetAccountAssets {
        string account_id = 1;
        AssetPaginationMeta pagination_meta = 2;
    }

İstek Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Hesap Kimliği", "bakiyesini istemek için hesap kimliği", "<account_name>@<domain_id>", "makoto@soramitsu"
    AssetPaginationMeta.page_size, "İstenen sayfa boyutu. Yanıttaki varlık sayısı bu değeri aşmaz. Yanıt kesildiyse, döndürülenlerin hemen ardından gelen varlık kimliği next_asset_id içinde sağlanır.", 0 < page_size < 32 bit unsigned int max (4294967296), 100
    AssetPaginationMeta.first_asset_id, "İstenen sayfa başlangıcı. Alan ayarlanmamışsa ilk sayfa döndürülür.", name#domain, my_asset#my_domain

Yanıt Şeması
------------
.. code-block:: proto

    message AccountAssetResponse {
        repeated AccountAsset account_assets = 1;
        uint32 total_number = 2;
        oneof opt_next_asset_id {
            string next_asset_id = 3;
        }
    }

    message AccountAsset {
        string asset_id = 1;
        string account_id = 2;
        string balance = 3;
    }

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Varlık Kimliği", "bakiyeyi kontrol etmek için kullanılan varlık tanımlayıcısı", "<asset_name>#<domain_id>", "jpy#japan"
    "Hesap Kimliği", "bu bakiyeye sahip hesap", "<account_name>@<domain_id>", "makoto@soramitsu"
    "Bakiye", "varlığın bakiyesi", "0'dan küçük değil", "200.20"
    total_number, sayfa sınırları olmadan sorguyla eşleşen varlık sayısı, 0 < total_number < 32 bit unsigned int max (4294967296), 100500
    next_asset_id, mevcut sayfayı hemen takip eden varlığın kimliği, name#domain, my_asset#my_domain

.. note::
   Sayfa boyutu, diğer istenen kriterlerle eşleşen varlık sayısına eşit veya daha büyükse, yanıttaki sonraki varlık kimliği ayarlanmaz.
   Aksi takdirde, istemcilerin bir sonraki sayfayı getirmek istemesi durumunda ilk varlık kimliği için kullanması gereken değeri içerir.


Olası Durumlu Doğrulama Hataları
---------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Hesap varlıkları alınamadı", "İç hata oluştu", "Tekrar deneyin veya geliştiricilerle iletişime geçin"
    "2", "Böyle bir izin yok", "Sorgunun oluşturucusunun hesap varlıklarını alma izinlerinden herhangi birine sahip değil", "Gerekli izni verin: bireysel, genel veya domain izni"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamadan geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"
    "4", "Geçersiz sayfalama meta verisi", "Yanlış sayfa boyutu veya var olmayan ilk varlık", "Geçerli bir sayfa boyutu ayarlayın ve varlık kimliğinin geçerli olduğundan emin olun veya ilk varlık kimliğini belirtmeyin"

Hesap Detayını Getir
^^^^^^^^^^^^^^^^^^^^

Amaç
----

Hesap detaylarını almak için `GetAccountDetail` sorgusu kullanılabilir. Hesap detayları anahtar-değer çiftleridir ve yazıcı kategorilerine ayrılır. Yazıcılar, ilgili hesap detayını ekleyen hesaplardır. Böyle bir yapının örneği aşağıdadır:

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18,
            "hobbies": "crypto"
        },
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

Burada dört hesap detayı görülebilir — "age", "hobbies" ve "sports" — bunları iki yazıcı eklemiştir — "account@a_domain" ve "account@b_domain". Tüm bu detaylar, açıkça, aynı hesapla ilgilidir.

İstek Şeması
------------

.. code-block:: proto

    message AccountDetailRecordId {
        string writer = 1;
        string key = 2;
    }

    message AccountDetailPaginationMeta {
        uint32 page_size = 1;
        AccountDetailRecordId first_record_id = 2;
    }

    message GetAccountDetail {
        oneof opt_account_id {
          string account_id = 1;
        }
        oneof opt_key {
          string key = 2;
        }
        oneof opt_writer {
          string writer = 3;
        }
        AccountDetailPaginationMeta pagination_meta = 4;
    }

.. note::
    Sayfalama meta verisi hariç tüm alanların isteğe bağlı olduğunu unutmayın.
    Bunun nedenleri aşağıda açıklanmaktadır.

.. warning::
    Uyumluluk nedenleriyle istekte sayfalama meta verisi eksik olabilir, ancak bu davranış kullanımdan kaldırılmıştır ve kaçınılmalıdır.

İstek Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

        "Hesap ID", "detaylarının alınacağı hesap id", "<account_name>@<domain_id>", "account@domain"
        "Anahtar", "altında detay alınacak anahtar", "string", "age"
        "Yazıcı", "yazıcının hesap id'si", "<account_name>@<domain_id>", "account@domain"
        AccountDetailPaginationMeta.page_size, "İstenen sayfa boyutu. Yanıttaki kayıt sayısı bu değeri aşmayacaktır. Yanıt kesildiyse, döndürülenlerin hemen ardından gelen kayıt id'si next_record_id içinde sağlanacaktır.", 0 < page_size < 32 bit unsigned int max (4294967296), 100
        AccountDetailPaginationMeta.first_record_id.writer, yazıcıya göre istenen sayfa başlangıcı, name#domain, my_asset#my_domain
        AccountDetailPaginationMeta.first_record_id.key, anahtara göre istenen sayfa başlangıcı, string, age

.. note::
    İlk kayıt id'si belirtilirken, ana sorguda ayarlanmamış olan öznitelikleri (writer, key) sağlamak yeterlidir.

Yanıt Şeması
------------

.. code-block:: proto

    message AccountDetailResponse {
        string detail = 1;
        uint64 total_number = 2;
        AccountDetailRecordId next_record_id = 3;
    }

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

        "Detay", "hesap detayları içeren anahtar-değer çiftleri", "JSON", "aşağıya bakınız"
        total_number, sayfa sınırları olmadan sorguyla eşleşen kayıt sayısı, 0 < total_number < 32 bit unsigned int max (4294967296), 100
        next_record_id.writer, geçerli sayfanın hemen ardından gelen kaydın yazıcı hesabı, <account_name>@<domain_id>, pushkin@lyceum.tsar
        next_record_id.key, geçerli sayfanın hemen ardından gelen kaydın anahtarı, string, "cold and sun"

Olası Durumlu Doğrulama Hataları
--------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Hesap detayı alınamadı", "Bir iç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "Yetki yok", "Sorgunun yaratıcısı hesap detayını almak için gerekli izinlerden hiçbirine sahip değil", "Gerekli izni verin: bireysel, genel veya domain izni"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamayı geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"
    "4", "Geçersiz sayfalama meta verisi", "Yanlış sayfa boyutu veya var olmayan ilk kayıt", "Geçerli bir sayfa boyutu ayarlayın ve ilk kayıt id'sinin geçerli olduğundan emin olun ya da ilk kayıt id'sini belirtmeyin"

Kullanım Örnekleri
------------------

Yine, başlangıçtaki detaylar örneğini ele alalım ve farklı `GetAccountDetail` sorgu varyantlarının sonuç yanıtını nasıl değiştireceğine bakalım.

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18,
            "hobbies": "crypto"
        },
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

**account_id ayarlanmamış**

account_id ayarlanmamışsa — diğer alanlar boş olabilir ya da olmayabilir — otomatik olarak sorgu yaratıcısının hesabıyla değiştirilecek ve bu da aşağıdaki durumlardan birine yol açacaktır.

**yalnızca account_id ayarlanmış**

Bu durumda, o hesapla ilgili tüm detaylar döndürülecektir ve şu yanıt elde edilecektir:

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18,
            "hobbies": "crypto"
        },
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

**account_id ve key ayarlanmış**

Burada, tüm yazıcılar tarafından anahtar altında eklenen detaylar döndürülecektir. Örneğin, "age" anahtarını istediğimizde şu yanıtı alırız:

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18
        },
        "account@b_domain": {
            "age": 20
        }
    }

**account_id ve writer ayarlanmış**

Şimdi yanıt, bu hesapla ilgili ve tek bir belirli yazıcı tarafından eklenen tüm detayları içerecektir. Örneğin, "account@b_domain" yazıcısını istediğimizde şunu elde ederiz:

.. code-block:: json

    {
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

**account_id, key ve writer ayarlanmış**

Son olarak, üç alan da ayarlanırsa, sonuç belirli yazıcı tarafından ve belirli anahtar altında eklenen detayları içerecektir; örneğin, "age" anahtarını ve "account@a_domain" yazıcısını istersek şunu elde ederiz:

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18
        }
    }

Varlık Bilgisi Getir
^^^^^^^^^^^^^^^^^^^^

Amaç
----

Verilen varlık hakkındaki bilgileri (şimdilik hassasiyetini) almak için kullanıcı `GetAssetInfo` sorgusu gönderebilir.

İstek Şeması
------------

.. code-block:: proto

    message GetAssetInfo {
        string asset_id = 1;
    }

İstek Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Varlık ID", "ilgili bilgi alınacak varlık id", "<asset_name>#<domain_id>", "jpy#japan"


Yanıt Şeması
------------

.. code-block:: proto

    message Asset {
        string asset_id = 1;
        string domain_id = 2;
        uint32 precision = 3;
    }

.. note::
    Bilinen bir sorun nedeniyle, geçersiz hassasiyet değeri geçerseniz hiçbir istisna almayacağınızı lütfen unutmayın.
    Geçerli aralık: 0 <= precision <= 255

Olası Durumlu Doğrulama Hataları
--------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Varlık bilgisi alınamadı", "Bir iç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "Yetki yok", "Sorgunun yaratıcısı varlık bilgisi almak için gerekli izinlerden hiçbirine sahip değil", "Gerekli izni verin: bireysel, genel veya domain izni"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamayı geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Varlık ID", "bakiye kontrolü için kullanılan varlık tanımlayıcısı", "<asset_name>#<domain_id>", "jpy#japan"
    "Domain ID", "bu varlıkla ilişkili domain", "RFC1035 [#f1]_, RFC1123 [#f2]_", "japan"
    "Hassasiyet", "ondalık virgülden sonraki basamak sayısı", "0 <= precision <= 255", "2"

Roller Getir
^^^^^^^^^^^^

Amaç
----

Sistemde mevcut rolleri almak için kullanıcı Iroha ağına `GetRoles` sorgusu gönderebilir.

İstek Şeması
------------

.. code-block:: proto

    message GetRoles {
    }

Yanıt Şeması
------------

.. code-block:: proto

    message RolesResponse {
        repeated string roles = 1;
    }

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Roller", "ağda oluşturulan rollerin dizisi", "sistemdeki roller kümesi", "{MoneyCreator, User, Admin, …}"

Olası Durumlu Doğrulama Hataları
--------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Roller alınamadı", "Bir iç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "Yetki yok", "Sorgunun yaratıcısı rolleri almak için gerekli izinlerden hiçbirine sahip değil", "Gerekli izni verin: bireysel, genel veya domain izni"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamayı geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"

Rol İzinleri Getir
^^^^^^^^^^^^^^^^^^

Amaç
----

Sistemdeki role göre kullanılabilir izinleri almak için kullanıcı Iroha ağına `GetRolePermissions` sorgusu gönderebilir.

İstek Şeması
------------

.. code-block:: proto

    message GetRolePermissions {
        string role_id = 1;
    }

İstek Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Rol ID", "izinleri alınacak rol", "sistemde mevcut rol", "MoneyCreator"

Yanıt Şeması
------------

.. code-block:: proto

    message RolePermissionsResponse {
        repeated string permissions = 1;
    }

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "İzinler", "role ait izinlerin dizisi", "role ait izinlerin dizisi", "{can_add_asset_qty, …}"

Olası Durumlu Doğrulama Hataları
--------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Rol izinleri alınamadı", "Bir iç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "Yetki yok", "Sorgunun yaratıcısı rol izinlerini almak için gerekli izinlerden hiçbirine sahip değil", "Gerekli izni verin: bireysel, genel veya domain izni"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamayı geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"

.. [#f1] https://www.ietf.org/rfc/rfc1035.txt
.. [#f2] https://www.ietf.org/rfc/rfc1123.txt


Eşler Getir
^^^^^^^^^^^

Amaç
----

Iroha ağındaki eşlerin listesini döndüren bir sorgu.

İstek Şeması
------------

.. code-block:: proto

    message GetPeers {
    }

Yanıt Şeması
------------

.. code-block:: proto

    message Peer {
        string address = 1;
        string peer_key = 2; // hex string
    }

    message PeersResponse {
        repeated Peer peers = 1;
    }

Yanıt Yapısı
------------

Eşlerin adresleri ve genel anahtarları ile birlikte bir listesi döndürülür.

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Eşler", "ağdaki eşlerin dizisi", "boş olmayan eş listesi", "{Peer{""peer.domain.com"", ""292a8714694095edce6be799398ed5d6244cd7be37eb813106b217d850d261f2""}, …}"

Olası Durumlu Doğrulama Hataları
--------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Eşler alınamadı", "Bir iç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "Yetki yok", "Sorgu yaratıcısının eşleri almak için yeterli izni yok", "can_get_blocks veya can_get_peers iznine sahip bir rol ekleyin"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamayı geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"

.. warning::

    Şu anda Get Peers sorgusu uyumluluk nedenleriyle "can_get_blocks" iznini kullanmaktadır.
    Bu, bir sonraki büyük Iroha sürümüyle "can_get_peers" olarak değiştirilecektir.

Commit'leri Getir
^^^^^^^^^^^^^^^^^

Amaç
----

Bloklar commit edildiği anda yeni blokları almak için kullanıcı Iroha ağına `FetchCommits` RPC çağrısını çağırabilir.

İstek Şeması
------------

İstek argümanı gerekmez


Yanıt Şeması
------------

.. code-block:: proto

    message BlockQueryResponse {
        oneof response {
            BlockResponse block_response = 1;
            BlockErrorResponse block_error_response = 2;
        }
    }

    message BlockResponse {
        Block block = 1;
    }

    message BlockErrorResponse {
        string message = 1;
    }

`BlockQueryResponse` akışını döndürdüğünü lütfen unutmayın.

Yanıt Yapısı
------------

.. csv-table::
    :header: "Alan", "Açıklama", "Kısıt", "Örnek"
    :widths: 15, 30, 20, 15

    "Blok", "Iroha bloğu", "yalnızca commit edilmiş bloklar", "{ 'block_v1': ....}"

Olası Durumlu Doğrulama Hataları
--------------------------------

.. csv-table::
    :header: "Kod", "Hata Adı", "Açıklama", "Nasıl çözülür"

    "1", "Blok akışı alınamadı", "Bir iç hata oluştu", "Tekrar deneyin veya geliştiricilere başvurun"
    "2", "Yetki yok", "Sorgunun yaratıcısı blokları almak için gerekli izinlerden hiçbirine sahip değil", "`can_get_block <permissions.html#can-get-blocks>`__ iznini verin"
    "3", "Geçersiz imzalar", "Bu sorgunun imzaları doğrulamayı geçemedi", "Daha fazla imza ekleyin ve sorgunun imzalarının hesabın imzacılarının bir alt kümesi olduğundan emin olun"

.. note::
    `BlockErrorResponse` yalnızca `message` alanı içerir.
    Durumlu doğrulama hatası durumunda değeri "stateful invalid" olacaktır.
    `GetBlock <#get-block>`__ aynı `can_get_block <permissions.html#can-get-blocks>`__ iznini gerektirir.
    Bu nedenle, geçersiz imzacıları veya yetersiz izinleri kontrol etmek için `height = 1` (ilk blok her zaman mevcuttur) ile kullanılabilir.

Örnek
-----
Bu sorguyu nasıl kullanacağınıza dair bir örneği şurada kontrol edebilirsiniz:
https://github.com/x3medima17/twitter
