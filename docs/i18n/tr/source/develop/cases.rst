Kullanım Senaryoları
====================

Hyperledger Iroha'nın bu uygulamalara getirebileceği bir
dizi kullanım senaryosu ve somut avantajları listeliyoruz.
Uygulamaların ve kullanım senaryolarının, geliştiricileri
ve yaratıcıları Hyperledger Iroha ile daha fazla yeniliğe
davet etmesini umuyoruz.


Eğitim ve Sağlıkta Sertifikalar
-------------------------------

Hyperledger Iroha sisteme üniversiteler, okullar ve tıbbi
kurumlar gibi birden fazla sertifikalandırma otoritesini
dahil eder. Hyperledger Iroha'da kullanılan esnek izin
modeli, sertifikalandıran kimlikler ve verilen sertifikalar
oluşturmayı sağlar. Kullanıcı hesaplarındaki açık ve örtük
bilgilerin depolanması, çeşitli itibar ve kimlik sistemleri
kurulmasını mümkün kılar.

Hyperledger Iroha kullanarak her eğitim veya tıbbi sertifika,
belirli sertifikalandırma otoriteleri tarafından verilip
verilmediği doğrulanabilir. Değişmezlik ve net doğrulama
kuralları, sağlık ve eğitimde şeffaflık sağlayarak sahte
sertifika kullanımını önemli ölçüde azaltır.

Örnek
^^^^^

Hyperledger Iroha'da ``hospital`` alanı olarak kayıtlı bir
tıbbi kurum hayal edin. Bu alan, her birinin ``physician``,
``therapist``, ``nurse`` gibi bir rolü olan sertifikalı ve
kayıtlı çalışanlara sahiptir. Hastanenin her hastasının tam
tıbbi geçmişi olan bir hesabı vardır. Kan testi sonuçları
gibi her tıbbi kayıt, hastanın hesabında JSON anahtar/değer
çifti olarak güvenli ve gizli şekilde saklanır.
``hospital`` alanındaki kurallar, yalnızca sertifikalı tıbbi
çalışanların ve kullanıcının kişisel bilgilere erişebileceği
şekilde tanımlanmıştır. Bir sorgunun döndürdüğü tıbbi
veriler, güvenilir bir kaynaktan geldiği doğrulanır.

Hastane belirli bir konuma bağlıdır ve o konumun yasal
kurallarına uyar — vatandaşların kişisel verilerinin yalnızca
belirli bölgelerde saklanması gibi (``privacy rules``_).
Hyperledger Iroha'daki çoklu-alan yaklaşımı, yasal kuralları
ihlal etmeden birden fazla ülke arasında bilgi paylaşımına
izin verir. Örneğin, ``makoto@hospital`` kullanıcısı tıbbi
vaka geçmişini başka bir ülkedeki tıbbi kurumla paylaşmaya
karar verirse, ``can_get_my_acc_detail`` izniyle ``grant``
komutunu kullanabilir.

Tıbbi kuruma benzer şekilde, Hyperledger Iroha'da kayıtlı bir
üniversite, mezun öğrencilere bilgi gönderme iznine sahiptir.
Diploma veya sertifika esasen tanınmış bir üniversitenin
imzasıyla Proof-of-Graduation'dır. Bu yaklaşım işe alım
sürecini kolaylaştırır — işveren, potansiyel çalışanın
edinilmiş beceri ve yetkinliklerini almak için Hyperledger
Iroha'ya sorgu yapar.

.. _`privacy rules`: https://privacypolicies.com/blog/privacy-law-by-country/

Sınır Ötesi Varlık Transferleri
-------------------------------

Hyperledger Iroha, çoklu imza hesapları ve atomik takas
kullanarak hızlı ve net ticaret ve uzlaşma kuralları sağlar.
Varlık yönetimi merkezi sistemlerdeki kadar kolaydır ve
gerekli güvenlik garantilerini sağlar. Varlık oluşturma
ve aktarmak için gereken kuralları ve komutları basitleştirerek
giriş engelini düşürüyor, aynı zamanda yüksek güvenlik
garantilerini koruyoruz.

Örnek
^^^^^

Örneğin [#f1]_, bir kullanıcı bir arabanın sahipliğini
aktarmak isteyebilir. ``haruto`` kullanıcısı ``sora`` markalı
bir arabayla ``{"id": "34322069732074686520616E73776572",
"color": "red", "size": "small"}`` parametreleriyle
sahip-varlık ilişkisini kaydetmiştir. Bu sahiplik, sistemin
temelindeki veritabanına ve her doğrulayan peer'e kopyalarla
sabitlenmiştir. Transfer işlemini gerçekleştirmek için
``haruto`` kullanıcısı bir teklif, yani iki komutlu çoklu
imza işlemi oluşturur: ``haru`` kullanıcısına arabanın
tanımlayıcısını ``transfer`` etmek ve ``haru``'dan
``haruto``'ya belirli miktarda ``usd`` token ``transfer``
etmek. Teklifi aldıktan sonra ``haru``, çoklu imza işlemini
imzalayarak kabul eder — bu durumda işlem atomik olarak
sisteme commit edilir.

Hyperledger Iroha'nın yerleşik bir token'ı yoktur, ancak
farklı yaratıcıların çeşitli varlıklarını destekler. Bu
yaklaşım merkeziyetsiz bir değişim piyasası kurmayı sağlar.
Örneğin sistemde farklı ülkelerin merkez bankaları varlık
yayımlayabilir.

.. [#f1] Şu anda uygulanmamıştır

Finansal Uygulamalar
--------------------

Hyperledger Iroha denetim sürecinde çok yararlı olabilir.
Her bilgi iş kurallarıyla doğrulanır ve farklı ağ
katılımcıları tarafından sürekli olarak sürdürülür. Erişim
kontrol kuralları ve bir miktar şifreleme, istenen gizlilik
seviyesini korur. Erişim kontrol kuralları farklı
seviyelerde tanımlanabilir: kullanıcı, alan veya sistem
seviyesi. Kullanıcı seviyesinde belirli bir birey için
gizlilik kuralları tanımlanır. Erişim kuralları alan veya
sistem seviyesinde belirlenirse, alandaki tüm kullanıcıları
etkiler. Hyperledger Iroha'da, her rolün belirli izinlere
sahip olduğu kullanışlı rol tabanlı erişim kontrol kuralları
sağlıyoruz.

İşlemler yerel bir veritabanıyla izlenebilir. Iroha-API
kullanarak denetçi veriler üzerinde sorgu ve analiz
yapabilir, belirli denetim yazılımı çalıştırabilir.
Hyperledger Iroha analiz yazılımının dağıtımı için farklı
senaryoları destekler: yerel bir bilgisayarda veya belirli
bir ara yazılım üzerinde kod çalıştırma. Bu yaklaşım
Hadoop, Apache ve diğerleriyle Big Data uygulamalarını
analiz etmeye izin verir. Hyperledger Iroha, veri bütünlüğü
ve gizliliğinin garantörü olarak hizmet eder (sorgu izinleri
kısıtlaması sayesinde).

Örnek
^^^^^

Örneğin denetim finansal uygulamalarda yararlı olabilir.
Denetçi hesabının ``auditor`` rolü vardır ve kullanıcıyı
rahatsız etmeden alandaki kullanıcıların bilgilerine erişim
izni vardır. Hesap ele geçirme olasılığını azaltmak ve
denetçinin kötü niyetli sorgular göndermesini önlemek için
denetçi genellikle çoklu imza hesabı olarak tanımlanır —
denetçi yalnızca birden fazla ayrı kimliğin imzası olmadan
sorgu yapamaz. Denetçi yalnızca hesap verilerini ve bakiyeyi
getirmek için değil, aynı zamanda kullanıcının tüm
işlemleri için de sorgu yapabilir — örneğin ``haruto``
kullanıcısının ``konoha`` alanındaki tüm transferleri.
Milyonlarca kullanıcının verilerini verimli bir şekilde
analiz etmek için her Iroha düğümü analiz yazılımıyla
birlikte çalışabilir.

Çoklu imza işlemleri Hyperledger Iroha'nın güçlü bir
aracıdır ve vergi sistemini dönüştürebilir. Belirli bir
alandaki her işlem, bir imzası kullanıcıdan (örneğin varlık
transferi) ve ikinci imzası özel vergilendirme düğümlerinden
gelerek çoklu imza işlemi olabilir. Vergilendirme
düğümlerinin Iroha-API ile yazılmış özel doğrulama
kuralları olacaktır — örneğin sertifikalı mağazalardaki her
alışveriş vergi ödemelidir. Başka bir deyişle, Iroha'da
geçerli bir alışveriş işlemi iki komut içermelidir: mağazaya
para transferi (alışveriş) ve hükümete para transferi (vergi
ödemesi).


Kimlik Yönetimi
---------------

Hyperledger Iroha'nın kimlik yönetimi için doğal desteği
vardır. Sistemdeki her kullanıcının kişisel bilgileri olan
benzersiz şekilde tanımlanmış bir hesabı vardır ve her işlem
imzalanır ve belirli bir kullanıcıyla ilişkilendirilir.
Bu, Hyperledger Iroha'yı KYC (Know Your Customer)
özellikli çeşitli uygulamalar için mükemmel kılar.

Örnek
^^^^^

Örneğin sigorta şirketleri, bilginin doğruluğu konusunda
endişelenmeden kullanıcının işlem bilgileri hakkında sorgu
yaparak faydalanabilir. Kullanıcılar da kimliği
doğrulanmış bilgilerin hasar işleme süresini kısaltacağı için
kişisel bilgilerini bir blockchain'de saklamaktan
faydalanabilir.
Kullanıcının ipotek kredisi almak istediği bir durumu
hayal edin. Şu anda ön yeterlilik, gelir, borç ve bilgi
doğrulama hakkında bilgi toplama sürecidir. Hyperledger
Iroha'daki her kullanıcının sahip olunan varlıklar, iş
pozisyonları ve borçlar gibi doğrulanmış kişisel bilgileri
olan bir hesabı vardır. Kullanıcı geliri ve borçları
``GetAccountTransactions`` sorgusuyla, sahip olunan
varlıklar ``GetAccountAssets`` sorgusuyla ve iş pozisyonları
``GetAccountDetail`` ile izlenebilir. Her sorgu doğrulanmış
bir sonuç döndürdüğü için ipotek kredisi işleme süresi
yalnızca birkaç saniye sürecektir.
Kullanıcıları kişisel bilgi paylaşmaya teşvik etmek için
çeşitli şirketler iş süreçleri geliştirebilir. Örneğin
sigorta şirketleri fitness yapan kullanıcılara bonus
indirimler sunabilir. Fitness uygulamaları sisteme özel
Proof-of-Activity gönderebilir ve kullanıcı daha sonra
``can_get_my_acc_detail`` izniyle ``GrantPermission``
kullanarak sigorta şirketleriyle bilgi paylaşmayı
kararlaştırabilir.


Tedarik Zinciri
---------------

Merkeziyetsiz bir sistemin yönetimi ve yasal kuralların
sistemin kodu olarak temsil edilmesi, her tedarik zinciri
sisteminin temel birleşimidir. Hyperledger Iroha'da
kullanılan sertifikasyon sistemi, fiziksel öğelerin
tokenizasyonuna ve sisteme yerleştirilmesine izin verir.
Her öğe "ne, ne zaman, nerede ve neden" bilgilerini içerir.

İzin sistemleri ve sınırlı güvenli çekirdek komutları kümesi
saldırı vektörünü daraltır ve çaba harcamadan temel bir
gizlilik seviyesi sağlar. Her işlem hash değeri, kimlik
denetimi bilgileri veya yaratıcının sertifikalarıyla sistem
içinde izlenebilir.

Örnek
^^^^^

Gıda tedarik zinciri, çiftçiler, depolar, bakkallar ve
müşteriler gibi birden fazla farklı aktörü olan paylaşılan
bir sistemdir. Amaç, gıdayı çiftçinin tarlasından
müşterinin sofrasına ulaştırmaktır. Ürün birçok aşamadan
geçer ve her aşama paylaşılan alana kaydedilir. Müşteri
ürünün kodunu bir mobil cihazla tarar ve içinde Iroha
sorgusu kodlanmıştır. Iroha sorgusu tüm aşamaları, ürün
bilgisini ve çiftçiyi içeren tam bir geçmiş sağlar.

Örneğin ``gangreen``, ``tomato`` varlığının kayıtlı çiftçisi
ve yaratıcısıdır; fiziksel öğeleri tokenize eden, yani her
domatesi Iroha ``tomato`` öğesiyle ilişkilendiren bir
garantör olarak hizmet eder. Varlık oluşturma ve dağıtım
işlemleri ağ katılımcıları için tamamen şeffaftır. Iroha
``tomato`` sonunda kullanıcı ``chad``'e ulaşmak üzere çok
sayıda satıcıdan geçerek bir yolculuğa çıkar.

Varlık oluşturmayı karmaşık akıllı sözleşmeler gerektirmeden
yalnızca tek bir ``CreateAsset`` komutuna indirgeyerek
sadeleştirdik. Hyperledger Iroha'nın başlıca avantajlarından
biri kolaylığıdır; geliştiricilerin uygulamalarının
sağladığı değere odaklanmasına olanak tanır.

Fon Yönetimi
------------

Çoklu imza işlemlerinin desteğiyle bir fonun birçok yönetici
tarafından sürdürülmesi mümkündür. Bu şemada yatırım,
katılımcıların yeterli çoğunluğunun onayından sonra
yapılabilir.

Örnek
^^^^^

Fon varlıkları tek bir hesapta tutulmalıdır.
İmza sahipleri yatırımlarla ve portföy dağılımlarıyla
ilgilenen fon yöneticileri olmalıdır. Bu ``AddSignatory``
komutuyla eklenebilir. Tüm varlıklar, imza sahiplerinin fon
yöneticilerini temsil ettiği tek bir hesapta tutulmalıdır.
Böylece belirli borsalar çoklu imza işlemiyle gerçekleştirilebilir
ve herkes belirli bir finansal kararı vermelidir. Bir taraf,
orijinal işlemi ve yöneticilerden birinin imzasını göndererek
işi onaylayabilir. Iroha işlem gönderimini sürdürür, böylece
işlem, işlemin yeterli çoğunluk parametresiyle
parametreleştirilen gerekli onay sayısını alana kadar
tamamlanmaz.


İlgili Araştırmalar
-------------------

(Fikir, blokzincir uygulamalarının güncel öncülerini ve
işlerini göstermeydi.)
