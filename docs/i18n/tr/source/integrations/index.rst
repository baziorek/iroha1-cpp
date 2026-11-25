.. _integrations:

===================
Entegre projeler
===================

Hyperledger Consortium'un fikirlerinden biri, mümkün olan en iyi
blokzincir deneyimini sağlamak için birlikte çalışabilecek
çözümler yaratmaktır. Iroha'da, diğer harika Hyperledger araçları
ve çözümleriyle entegrasyonun kullanım senaryolarınız için Iroha'yı
daha iyi hale getirmenin bir yolu olduğuna inanıyoruz. Bu yüzden
birkaç projeyle entegrasyonlar üzerinde çalıştık ve Iroha'nın
nelerle çalışabileceği hakkında daha fazla anlatmak istiyoruz.

.. note::
	Aşağıda listelenen tüm entegrasyonlar **arşivlenmiştir**:
	karşı taraf projeler ya End of Life'a ulaştı ya da Iroha 1
desteğini geri çekti. Bilgi, tarihsel referans ve eski dağıtımları
    sürdürenler için saklanmaktadır.

Arşivlenmiş entegrasyonlar
==========================

Aşağıdaki entegrasyonlar Iroha 1 için geliştirilmiştir ancak artık
sürdürülmemektedir. Uygun olduğu durumlarda, tarihler
Hyperledger TSC/TOC'nin resmi End of Life kararlarıdır.

.. _hyperledger-cacti:

Hyperledger Cacti (eski adıyla Cactus) — Iroha konektörü Cacti 2.0'da kaldırıldı (2024)
---------------------------------------------------------------------------------------

`Hyperledger Cacti <https://github.com/hyperledger-cacti/cacti>`_
(eski adıyla Hyperledger Cactus), farklı blokzincirlerin ve
dağıtık defterlerin ortak bir API aracılığıyla birbirleriyle
iletişim kurmasını sağlayan bir birlikte çalışabilirlik
platformudur; böylece bir uygulama özel entegrasyonlar olmadan
birden fazla defterle çalışabilir.

Iroha'nın özel bir Cactus konektörü vardı; `Iroha ve Cactus
entegrasyonu
<https://lf-hyperledger.atlassian.net/wiki/spaces/INTERN/pages/21954723/HL+Iroha+and+HL+Cactus+Integration>`_
ve `Mevcut Iroha–Cactus entegrasyonunu genişletme
<https://lf-hyperledger.atlassian.net/wiki/spaces/INTERN/pages/21954791/Extend+existing+Iroha+-+Cactus+integration>`_
wiki sayfalarında açıklanmıştır.

.. note::
	Cacti'nin kendisi hâlâ aktif bir LF Decentralized Trust
	projesidir, ancak **Iroha 1 konektörü Cacti 2.0'da (2024)
	kaldırılmıştır** ve Iroha 2 konektörü de kapatılmaktadır.
	Aşağıdaki materyal bu nedenle Iroha 1 dağıtımları için tarihi
	ilgi taşır.

.. _hyperledger-ursa:

Hyperledger Ursa — EOL 27 Nisan 2023
-------------------------------------

`Hyperledger Ursa
<https://wiki.hyperledger.org/display/ursa/Hyperledger+Ursa>`_,
insanların (ve projelerin) diğer kriptografik çalışmaları
yinelenmekten kaçınmasını ve umarım süreçte güvenliği artırmasını
sağlayan paylaşılan bir kriptografi kütüphanesidir. Kütüphane,
projelerin (ve potansiyel olarak başkalarının) kripto kodu
yerleştirip kullanması için bir opt-in deposudur. Hyperledger
Ursa, tutarlı kriptografik kod uygulamalarından veya kriptografik
koda arayüzlerden oluşan alt projelerden oluşur.

Derleme sırasında yalnızca `bir bayrak
ekleyerek <../build/index.html#main-parameters>`_ Iroha'yı Ursa
kütüphanesiyle kolayca derleyebilirsiniz. Bu, standart Iroha
kriptografisi yerine Ursa kütüphanesindeki kriptografik
algoritmaları kullanmanızı sağlar. Ursa'da yeni kütüphaneler
geliştirildikçe size daha fazla seçenek sunulacaktır!

.. note::
	Şu anda Ursa'dan yalnızca ed25519 SHA-2 algoritmasını
	alıyoruz. İsterseniz daha fazla seçenek eklemek için koda
	katkıda bulunabilirsiniz.

Hem varsayılan ed25519/sha3 kriptografisini hem de Ursa'dakileri
kullanabilmek için, sonuncular için Multihash genel anahtar
biçimini kullanıyoruz. `Anahtarlar
<../develop/keys.html>`_ hakkında daha fazla bilgi edinebilirsiniz.

.. _hyperledger-explorer:

Hyperledger Explorer — EOL 12 Mayıs 2022
-----------------------------------------

`Hyperledger Explorer
<https://wiki.hyperledger.org/display/explorer>`_, bir blokzincir
modülü ve The Linux Foundation tarafından barındırılan
Hyperledger projelerinden biridir. Kullanıcı dostu bir Web
uygulaması oluşturmak üzere tasarlanan Hyperledger Explorer,
blokları, işlemleri ve ilişkili verileri, ağ bilgilerini (ad,
durum, düğüm listesi), zincir kodlarını ve işlem ailelerini ve
defterde saklanan diğer ilgili bilgileri görüntüleyebilir, çağırabilir,
dağıtabilir veya sorgulayabilir.

`Burada
<https://github.com/turuslan/blockchain-explorer/blob/iroha-explorer-integration/iroha-explorer-integration.md>`_
Explorer'ı Iroha ile nasıl kullanacağınızı öğrenebilirsiniz.

.. _hyperledger-burrow:

Hyperledger Burrow — EOL 12 Mayıs 2022
---------------------------------------

`Hyperledger Burrow
<https://wiki.hyperledger.org/display/burrow>`_, Ethereum Virtual
Machine (EVM) spesifikasyonuna kısmen göre geliştirilmiş izin
tabanlı akıllı sözleşme yorumlayıcısına sahip modüler bir
blokzincir istemcisi sağlar.

Yani HL Burrow ile Iroha üzerinde Solidity akıllı sözleşmelerini
kullanabilirsiniz. Daha fazla bilgi için aşağıya tıklayın.

.. toctree::
    :maxdepth: 2

    burrow.rst
    burrow_example.rst

.. _hyperledger-quilt:

Hyperledger Quilt — EOL 2 Haziran 2022
---------------------------------------

`Hyperledger Quilt
<https://lf-hyperledger.atlassian.net/wiki/spaces/INTERN/pages/21954679/HL+Iroha+and+HL+Quilt+Integration>`_,
Iroha'nın diğer defterlerle ortak bir işlem ve sorgu modeli
aracılığıyla birlikte çalışmasını sağlayan bir
birlikte çalışabilirlik çerçevesiydi. Proje 23 Haziran 2021'de
Dormant durumuna geçirildi ve 2 Haziran 2022'de End of Life'a
ulaştı; depo arşivlendi. Bağlantılı wiki sayfası, orijinal
Iroha–Quilt entegrasyon çalışmasını belgelendirir.
