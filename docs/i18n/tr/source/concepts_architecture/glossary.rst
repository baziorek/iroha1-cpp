Hesap
=====

Belirli bir dizi eylemi gerçekleştirebilen bir Iroha
varlığı. Her hesap, mevcut `alan adlarından
<Alan Adı_>`__ birine aittir.

Bir hesabın belirli sayıda `rolü <Rol_>`__ vardır
(null olabilir) — bunlar izinlerin bir koleksiyonudur.
Yalnızca `devredilebilir izinler <Devredilebilir İzin_>`__
doğrudan bir hesaba atanır.


Varlık
======

Sayımla belirlenebilen herhangi bir mal veya değer. Her
varlık, mevcut `alan adlarından <Alan Adı_>`__ birine
bağlıdır. Örneğin bir varlık; para birimi birimi, altın
külçesi, gayrimenkul birimi gibi her türlü birimi
temsil edebilir.

Blok
====

İşlem verileri kalıcı olarak blok adı verilen dosyalara
kaydedilir. Bloklar zaman içinde doğrusal bir dizi
halinde düzenlenir (blok zinciri olarak da bilinir)
[#f1]_.

Bloklar, bu blok için `konsensüs <Konsensüs_>`__
sırasında oy kullanan Iroha `eşlerinin <Eş_>`__
kriptografik imzalarıyla imzalanır. İmzalanabilir içerik
yük (payload) olarak adlandırılır, bu nedenle blok
yapısı şöyle görünür:

.. image:: ../../image_assets/block.png
    :width: 80%
    :align: center
    :alt: Iroha Blok Yapısı

*Yük dışında*

    - imzalar — konsensüs turu sırasında blok için oy kullanan eşlerin imzaları

*Yük içinde*

    - yükseklik — zincirde bu bloğa kadar olan blok sayısı
    - zaman damgası — eş tarafından blok oluşturma Unix
      zamanı (milisaniye cinsinden)
    - doğrulama ve konsensüs adımını başarıyla geçen
      işlemlerin dizisi
    - zincirdeki önceki bloğun özeti
    - reddedilen işlem özetleri — durumlu doğrulamayı
      geçemeyen işlemlerin özetlerinin dizisi; bu alan
      isteğe bağlıdır


İstemci
=======

Iroha kullanan herhangi bir uygulama istemci olarak
değerlendirilir.

Iroha'nın ayırt edici bir özelliği, tüm istemcilerin bir
eş ağında etkileşim kurarken basit istemci-sunucu
soyutlamaları kullanmasıdır: blockchain ile ilgili
sistemlere özgü hiçbir soyutlama kullanmazlar. Örneğin
Bitcoin'de istemcilerin blokları doğrulaması gerekirken,
HL Fabric'de bir işlemin bir bloğa yazıldığından emin
olmak için birkaç eşi sorgulamaları gerekir; HL Iroha'da
ise bir istemci herhangi bir eşle tek sunucuya benzer
şekilde etkileşir.

Komut
=====

Komut, ağın `durumunu <architecture.html#world-state-view>`__
değiştirme niyetidir. Örneğin Iroha'da yeni bir `rol
<Rol_>`__ oluşturmak için `Create role
<../develop/api/commands.html#create-role>`__ komutunu
vermeniz gerekir.

Konsensüs
=========

Konsensüs algoritması, dağıtık süreçler veya sistemler
arasında tek bir veri değeri üzerinde anlaşmaya
varmak için bilgisayar biliminde kullanılan bir süreçtir.
Konsensüs algoritmaları, birden fazla güvenilmez düğüm
içeren bir ağda güvenilirliği sağlamak için tasarlanmıştır.
Konsensüs problemi olarak bilinen bu sorunu çözmek,
dağıtık bilişim ve çok ajanlı sistemlerde önemlidir.

*Konsensüs, bir algoritma olarak*

Ağdaki eşler arasında bir blok üzerinde anlaşmaya varan
bir algoritma. Sistemde bulunması güvenilirliği artırır.

Konsensüsü Iroha'nın bir bileşeni olarak görmek için
`bu bağlantıyı <architecture.html#block-consensus-yac>`__
kontrol edin; YAC protokolünün ayrıntılı açıklaması için
`YAC belgelerine <yac.html>`_ bakın.

Alan Adı
========

`Hesapları <Hesap_>`__ ve `varlıkları <Varlık_>`__
gruplamak için adlandırılmış bir soyutlama. Örneğin
Iroha ile çalışan kuruluşlar grubundaki bir kuruluşu
temsil edebilir.

Eş
==

Iroha ağının bir parçası olan düğüm. `Konsensüs
<Konsensüs_>`_ sürecine katılır.

Bir eş ayrıca **senkronize eş** olarak da kaydedilebilir
— yalnızca defteri izleyen (örn. yedekleme veya denetim
için) ve konsensüs oylamasına katılmayan bir düğüm.
Senkronize eşler, senkronize olarak işaretlenmiş bir eş
kaydıyla olağan `AddPeer
<../develop/api/commands.html#add-peer>`__ komutu
kullanılarak eklenir; aynı ``can_add_peer`` izni geçerlidir.
Konsensüs durumunu doğrudan aldıkları için yetişebilirler
ancak blok önermezler veya oy vermezler.

İzin
====

Bir komutu gerçekleştirme ayrıcalığı veren adlandırılmış
bir kural. İzin doğrudan bir `hesaba <Hesap_>`__
**verilemez**; bunun yerine hesap, izin koleksiyonları
olan rollere sahiptir. Yine de bir istisna vardır — bkz.
`Devredilebilir İzin <Devredilebilir İzin_>`__.

`Iroha izinlerinin listesi
<../develop/api/permissions.html>`_.

Devredilebilir İzin
------------------

Yalnızca devredilebilir izin doğrudan bir `hesaba
<Hesap_>`__ verilir. Devredilebilir izne sahip bir
hesap, başka bir hesap adına belirli bir eylemi
gerçekleştirebilir. Örneğin a@domain1 hesabı b@domain2
hesabına varlık transfer etme izni verirse — o zaman
b@domain2, a@domain1'in varlıklarını herkese
transfer edebilir.

Teklif
======

Yalnızca `durumsuz doğrulamayı
<Durumsuz Doğrulama_>`__ geçen `işlemlerin
<İşlem_>`__ kümesi.

Doğrulanmış Teklif
------------------

Hem `durumsuz <Durumsuz Doğrulama_>`__ hem de
`durumlu <Durumlu Doğrulama_>`__ doğrulamayı geçen,
ancak henüz taahhüt edilmemiş işlemlerin kümesi.

Sorgu
====

Ağın `durumunu <architecture.html#world-state-view>`__
**değiştirmeyen** bir Iroha isteği. Bir sorgu
gerçekleştirerek istemci, durumdan veri isteyebilir; örneğin
hesabının bakiyesi, işlem geçmişi vb.

Çoğunluk
=======

İşlem imzalama bağlamında çoğunluk sayısı, bir işlemin
imzalı sayılması için gereken minimum imza sayısıdır.
Varsayılan değer 1'dir. `Çoklu imza işlemleri
<Çoklu İmzalı İşlemler_>`__ için bu sayıyı
artırmanız gerekecektir.

Her hesap ek genel anahtarlar bağlayabilir ve kendi
çoğunluk sayısını artırabilir.

Rol
===

Bir `izinler <İzin_>`__ kümesi tutan adlandırılmış
bir soyutlama.

İmzalayan
=========

Bir `hesap <Hesap_>`__ için çoklu imza işlemlerini
onaylayabilen bir varlığı temsil eder. `AddSignatory
<../develop/api/commands.html#add-signatory>`__ ile
hesaba eklenebilir ve `RemoveSignatory
<../develop/api/commands.html#remove-signatory>`__ ile
kaldırılabilir.

İşlem
====

Deftere atomik olarak uygulanan `komutların
<Komut_>`__ sıralı kümesi. Doğrulama süreci sırasında
bir işlemin içindeki geçersiz herhangi bir komut, tüm
işlemin reddedilmesine yol açar.

İşlem Yapısı
------------

**Yük (Payload)**, imzalar dışında tüm işlem alanlarını
saklar:

    - Oluşturma zamanı (unix zamanı, milisaniye cinsinden)
    - İşlem oluşturucusunun hesap kimliği (kullanıcı@alan_adı)
    - Çoğunluk alanı (gerekli imza sayısını gösterir)
      `komutlar bölümünde <../develop/api/commands.html>`__
      ayrıntılı açıklanan tekrarlanan komutlar
    - Toplu meta bilgisi (isteğe bağlı kısım). Ayrıntılar
      için `İşlem Toplamı`_ bölümüne bakın


**İmzalar**, bir veya daha fazla imza içerir (ed25519
genel anahtarı + imza)

Azaltılmış İşlem Özeti
^^^^^^^^^^^^^^^^^^^^^^

Azaltılmış özet, toplu meta bilgisi hariç işlem yükü
üzerinden hesaplanır. `İşlem Toplamı`_'nda kullanılır.


İşlem Durumları
---------------

Hyperledger Iroha, istemciyle hem push hem de pull etkileşim
modunu destekler. Pull modunu kullanan bir istemci, işlem
özetlerini göndererek ve yanıt bekleyerek Iroha eşinden
işlem durumu güncellemelerini ister. Buna karşılık push
etkileşimi, her işlem için bir olay akışını dinleyerek
gerçekleştirilir. Bu modların her ikisinde de işlem
durumları kümesi aynıdır:

 .. image:: ./../../image_assets/tx_status.png

.. note::
    Iroha'daki Durum akış özelliği hakkında Medium'da
    harika bir `makale yazdık <https://medium.com/iroha-contributors/status-streaming-in-hl-iroha-5503487ffcfd>`_.
    Okuyun ve yorumlarda ne düşündüğünüzü bize bildirin!

İşlem Durumu Kümesi
^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Durum
     - Anlamı
   * - ``NOT_RECEIVED``
     - İstenen eş bu işleme sahip değil.
   * - ``ENOUGH_SIGNATURES_COLLECTED``
     - Çoklu imza işlemi yeterli imzaya sahip ve
       doğrulanacak.
   * - ``MST_PENDING``
     - Çoklu imza işlemi hâlâ çoğunluğa ulaşmak için
       daha fazla imzaya ihtiyaç duyuyor.
   * - ``MST_EXPIRED``
     - Çoklu imza işlemi artık geçerli değil ve
       silinecek.
   * - ``STATELESS_VALIDATION_FAILED``
     - İşlem hatalı biçimlendirilmiş veya imzalar geçersiz;
       neden döndürülür.
   * - ``STATELESS_VALIDATION_SUCCESS``
     - İşlem durumsuz doğrulamayı geçti; gönderildikten
       hemen sonra döndürülür.
   * - ``STATEFUL_VALIDATION_FAILED``
     - Bir komut duruma bağlı kuralları ihlal ediyor
       (bakiye, izinler vb.); neden döndürülür.
   * - ``STATEFUL_VALIDATION_SUCCESS``
     - İşlem durumlu doğrulamayı geçti.
   * - ``COMMITTED``
     - İşlem, yeterli oy almış ve block store'da bulunan
       bir bloğun parçası.
   * - ``REJECTED``
     - İşlem, önceki bir konsensüs turunda durumlu
       doğrulama sırasında reddedildi; tekrarlama
       saldırılarını önlemek için özeti `blok
       <Blok_>`__ deposunda saklanır.

Bekleyen İşlemler
^^^^^^^^^^^^^^^^^

O an işlem oluşturucu hesabın `çoğunluğundan <Çoğunluk_>`__
daha az imzaya sahip olan herhangi bir işlem bekleyen
olarak değerlendirilir. Çoklu imza
`<Çoklu İmzalı İşlemler_>`__ mekanizması çoğunluk
için gerekli imza sayısını topladığı anda bekleyen işlem
`durumlu doğrulamaya <Durumlu Doğrulama_>`__
gönderilir.

Zaten çoğunluk imzasına sahip bir işlem de, işlem bir
`İşlem Toplamı`_'nın parçası olduğunda ve tamamen imzalanmamış
bir işlem olduğunda bekleyen olarak değerlendirilebilir.

İşlem Toplamı
=============

İşlem toplamı özelliği, sıralarını koruyarak birden fazla
işlemi aynı anda Iroha'ya göndermenizi sağlar.

Toplu içindeki her işlem, toplu meta bilgisi içerir.
Toplu meta, toplu türü tanımlayıcısını (atomik veya sıralı)
ve topludaki tüm işlemlerin `azaltılmış özetlerinin
<Azaltılmış İşlem Özeti_>`_ listesini içerir. Özetlerin
sırası işlem dizisini tanımlar.

Toplu, farklı hesaplarca oluşturulan işlemler içerebilir.
Toplu içindeki herhangi bir işlem tek veya
`çoklu <Çoklu İmzalı İşlemler_>`__ imza
gerektirebilir (işlem oluşturucu hesabı için ayarlanan
çoğunluğa bağlıdır). Toplunun `durumsuz
doğrulamayı <Durumsuz Doğrulama_>`__ geçebilmesi için
toplu içinde en az bir işlemin en az bir imzaya sahip
olması gerekir.

Toplular hakkında bir makaleyi Contributors sayfamızda
`Medium <https://medium.com/iroha-contributors/batches-in-iroha-117614cf1e88>`__
üzerinden okuyabilirsiniz.

Atomik Toplu
------------

Atomik topludaki tüm işlemler, toplunun deftere uygulanması
için `durumlu doğrulamayı <Durumlu Doğrulama_>`__
geçmelidir.

Bir atomik toplu durumlu doğrulamayı geçemediğinde,
toplu içindeki her işlem kendi durum güncellemelerini
alır, böylece başarısız işlem durum akışından
tanımlanabilir:

- Başarısız komutu içeren işlem, boş olmayan bir
  ``err_or_cmd_name`` (başarısız komutun adı), komuta
  özgü bir ``error_code`` (`komutlar bölümü
  <../develop/api/commands.html>`__'nde kodların
  anlamını görün) ve ``failed_cmd_index``'in **o işlem
  içindeki** (toplu içindeki değil) başarısız komutun
  konumuna ayarlandığı ``STATEFUL_VALIDATION_FAILED``
  alır.
- Topludaki diğer her işlem, boş bir
  ``err_or_cmd_name``, ``error_code`` ``1`` ve
  ``failed_cmd_index``'in ``18446744073709551615``
  (``SIZE_MAX``, "belirli komut yok" anlamına gelir)
  olduğu ``STATEFUL_VALIDATION_FAILED`` alır, ardından
  toplu bir bloğa taahhüt edildiğinde ``REJECTED`` alır.

Topluyu hangi işlemin bozduğunu öğrenmek için
``err_or_cmd_name``'in boş olmadığı durum güncellemesini
arayın; ``failed_cmd_index`` daha sonra o işlem içindeki
hangi komutun başarısız olduğunu gösterir.

Sıralı Toplu
------------

Sıralı toplu yalnızca deftere uygulanan işlemlerin sırasını
korur. Toplu içinde durumlu doğrulamayı geçebilen tüm
işlemler deftere uygulanır. Bir işlemin doğrulama
başarısızlığı, tüm toplunun doğrudan başarısızlığına
dönüşmez.

Çoklu İmzalı İşlemler
=====================

`Çoğunluğu <Çoğunluk_>`__ birden büyük olan işlem çoklu
imzalı (mst olarak da adlandırılır) olarak
değerlendirilir. `Durumlu geçerliliğe
<Durumlu Doğrulama_>`__ ulaşmak için oluşturucu
hesabın `imzalayıcıları <İmzalayan_>`__ tarafından
onay gerekir. Bu katılımcıların aynı işlemi kendi
imzalarıyla göndermeleri gerekir.

Doğrulama
=========

İki tür doğrulama vardır — durumsuz ve durumlu.

Durumsuz Doğrulama
------------------

`Torii <architecture.html#torii>`__'de gerçekleştirilir.
İmza dahil bir nesnenin iyi biçimlendirilip
biçimlendirilmediğini kontrol eder.

Durumlu Doğrulama
-----------------

`Doğrulanmış Teklif Oluşturucu
<Doğrulanmış Teklif_>`__'da gerçekleştirilir.
`World State View
<architecture.html#world-state-view>`__'e karşı
doğrular.


.. [#f1] https://en.bitcoin.it/wiki/Block
