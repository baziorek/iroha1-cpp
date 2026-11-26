.. contents:: **İçindekiler**
  :depth: 3

Öncelikle, katkıda bulunmak için zaman ayırdığınız için teşekkürler!

Aşağıda Iroha'ya katkıda bulunmak için kısa bir kılavuz seti yer
almaktadır.

Nasıl Katkıda Bulunabilirim?
----------------------------

Dokümantasyonu Çevirme
~~~~~~~~~~~~~~~~~~~~~~~

Bu dokümantasyonun çeviri sürümleri, İngilizce sürümün yanında
``/<language>/`` altında barındırılmaktadır. Şu anda mevcut olanlar:

* **Türkçe** — `Türkçe dokümantasyon <../tr/>`_ (arşivlenmiş
  :strikelink:`hyperledger-iroha/iroha-docs-l10n
  <https://github.com/hyperledger-iroha/iroha-docs-l10n>` deposundan
  çevrilmiştir; çeviri Iroha 1.2 dönemi dokümantasyonunu kapsar ve
  İngilizce sürümün gerisinde kalabilir)

Polonya dili de dahil olmak üzere daha fazla dil planlanmaktadır. Bir
çeviriye katkıda bulunmak isterseniz lütfen
`Discord <https://discord.com/channels/905194001349627914/905205848547155968>`_
üzerinden bizimle iletişime geçin.

Hata Bildirme
~~~~~~~~~~~~~

*Hata*, Iroha'da yanlış veya beklenmeyen bir sonuca yol açan ya da
istenmeyen biçimde davranmasına sebep olan bir kusur, tasarım hatası,
arıza veya bozukluktur.

Hatalar `GitHub issue'ları
<https://github.com/hyperledger-iroha/iroha/issues>`_ olarak takip
edilir.

Bir hata bildirmek için `yeni issue oluşturun
<https://github.com/hyperledger-iroha/iroha/issues/new>`_ ve issue
şablonunda şu ayrıntıları belirtin: sorunun özeti, yeniden üretme
adımları ve ilgili tüm günlükler. Lütfen ayrıca ortam bilgilerinizi
(işletim sistemi, cihaz özellikleri, Iroha sürümü vb.) sağlayın.

Güvenlik Açığı Bildirme
~~~~~~~~~~~~~~~~~~~~~~~

Güvenlik sorunlarını önlemede proaktif olmaya çalışsak da, hiç ortaya
çıkmayacaklarını varsaymıyoruz.

Bir sorunu düzeltmenin hazırlanabilmesi ve güvenlik açığından
doğabilecek zararın en aza indirilebilmesi için, bir güvenlik sorununu
kamuoyuna duyurmadan önce tedarikçiye (Hyperledger organizasyonu)
sorumlu ve gizli şekilde bildirmek standart bir uygulamadır.

İlk Büyük Sürümden (1.0) önce tüm güvenlik açıkları hata olarak kabul
edilir; yukarıda açıklandığı şekilde göndermekten çekinmeyin. İlk Büyük
Sürümdan sonra lütfen güvenlik açıklarını göndermek ve ödülünüzü almak
için `buradaki hata ödülü programını
<https://hackerone.com/hyperledger>`__ kullanın.

Her durumda, keşfinizin bir güvenlik açığı mı yoksa bir hata mı
olduğunu tartışmak isterseniz Discord özel mesajlarından veya e-posta
yoluyla (CONTRIBUTORS.md dosyasına bakın) mevcut maintainer'lardan
herhangi birine ulaşmaktan çekinmeyin.

İyileştirme Önerileri
~~~~~~~~~~~~~~~~~~~~~

*İyileştirme*, **mevcut** kodu veya tasarımı daha hızlı, daha kararlı,
taşınabilir, güvenli ya da başka bir şekilde daha iyi hale getiren bir
kod ya da fikirdir.

İyileştirmeler `GitHub issue'ları
<https://github.com/hyperledger-iroha/iroha/issues>`_ olarak takip
edilir.

Yeni bir iyileştirme önermek için `yeni issue oluşturun
<https://github.com/hyperledger-iroha/iroha/issues/new>`_ ve fikri
açıklayın: neyi iyileştirdiği ve neden değerli olduğu. Kod önerileriniz
varsa dahil etmekten çekinmeyin.

Soru Sorma
~~~~~~~~~~

*Soru*, tipik olarak ne bir hata, ne bir özellik isteği ne de bir
iyileştirme olan herhangi bir tartışmadır. "X'i nasıl yaparım?" gibi
bir sorunuz varsa bu paragraf sizin için.

Lütfen sorunuzu topluluk üyelerinin size yardımcı olabilmesi için
`en sevdiğiniz mesajlaşma uygulamasında
<Topluluğun Aktif Olduğu Yerler_>`__ paylaşın. Siz de başkalarına
yardımcı olabilirsiniz!

İlk Kod Katkınız
~~~~~~~~~~~~~~~~

`C++ Stil Kılavuzu'nuzu <C++ Stil Kılavuzu_>`__ okuyun ve `GitHub
panosuna <https://github.com/hyperledger-iroha/iroha/projects/1>`_ bakarak
veya `good-first-issue etiketi
<https://github.com/hyperledger-iroha/iroha/issues?q=is%3Aissue+is%3Aopen+label%3Agood-first-issue>`_
taşıyan issue'larla başlayın. Bu görev üzerinde çalıştığınızı bir şekilde
belirtin: maintainer ekibiyle veya toplulukla iletişime geçin ya da
basitçe bu issue'yu kendinize atayın.

Pull Request'ler
~~~~~~~~~~~~~~~~

-  Gerekli `şablonu doldurun
   <https://github.com/hyperledger-iroha/iroha/blob/iroha1-main/.github/PULL_REQUEST_TEMPLATE.md>`_

-  Tüm dosyalar satır sonu ile bitsin

-  Yeni kod için **test yazın**. Yeni kodun test kapsamı en az
   %70 olmalıdır

-  Her pull request incelenmeli ve **maintainer ekibinden en az iki
   onay almalıdır**. `MAINTAINERS.md
   <https://github.com/hyperledger-iroha/iroha/blob/iroha1-main/MAINTAINERS.md>`_
   dosyasından mevcut maintainer'ların kim olduğunu kontrol edin

-  Çalışmanızı bitirdiğinizde tüm CI kontrollerinin geçtiğinden emin
   olun; ardından pull request'inizi **squash and merge** yapın

-  `C++ Stil Kılavuzu'na <C++ Stil Kılavuzu_>`_ uyun

-  `Git Stil Kılavuzu'na <Git Stil Kılavuzu_>`_ uyun

-  Yeni kodu `Dokümantasyon Stil
   Kılavuzu'na <Dokümantasyon Stil Kılavuzu_>`__ göre belgelendirin

-  **Fork'lardan gelen PR'ler** üzerinde çalışırken `bu
   kılavuzu <https://help.github.com/articles/checking-out-pull-requests-locally>`_
   inceleyin

Stil Kılavuzları
----------------

Git Stil Kılavuzu
~~~~~~~~~~~~~~~~~

-  `DCO <https://github.com/apps/dco>`_ ile **her commit'i imzalayın**:
   ``Signed-off-by: $NAME <$EMAIL>``. Bunu ``git commit -s`` kullanarak
   otomatik olarak yapabilirsiniz
-  **Şimdiki zaman kullanın** ("Add feature", "Added feature" değil).
-  **Emir kipi kullanın** ("Deploys docker to..." değil, "Deploy docker
   to...").
-  Anlamlı bir commit mesajı yazın.
-  Commit mesajının ilk satırını en fazla 50 karakterle sınırlayın
-  Commit mesajının ilk satırı yapılan işin özetini içermeli, ikinci
   satır boş olmalı, üçüncü ve sonraki satırlar commit değişikliklerinin
   listesini içerebilir
-  Hedef dala birleştirirken: **Squash and merge** kullanın

C++ Stil Kılavuzu
~~~~~~~~~~~~~~~~~

-  clang-format `ayarları
   <https://github.com/hyperledger-iroha/iroha/blob/iroha1-main/.clang-format>`_
   dosyasını kullanın. İnternette kılavuzlar mevcuttur (örn. `Kratos
   wiki <https://github.com/KratosMultiphysics/Kratos/wiki/How-to-configure-clang%E2%80%90format>`_)
-  `CppCoreGuidelines
   <http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines>`_
   ve `Cpp Best
   Practices <https://lefticus.gitbooks.io/cpp-best-practices>`_
   ilkelerine uyun.
-  `Platforma bağımlı
   <https://stackoverflow.com/questions/1558194/learning-and-cross-platform-development-c>`_
   kodlardan kaçının.
-  `C++23 <https://en.wikipedia.org/wiki/C%2B%2B23>`_ kullanın.
-  Sınıf adları ve metotlar için `camelCase
   <https://en.wikipedia.org/wiki/Camel_case>`_, değişkenler için
   `snake\_case <https://en.wikipedia.org/wiki/Snake_case>`_
   kullanın.

Dokümantasyon Stil Kılavuzu
~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  `Doxygen <http://www.doxygen.nl/>`_ kullanın.
-  Tüm genel API'yi belgelendirin: metotlar, fonksiyonlar, üyeler,
   şablonlar, sınıflar...

Topluluğun Aktif Olduğu Yerler
------------------------------

Topluluk üyelerimiz şu platformlarda aktiftir:

+------------------+-------------------------------------------------------------------------------------------------------+
| Servis           | Bağlantı                                                                                              |
+------------------+-------------------------------------------------------------------------------------------------------+
| Discord          | https://discord.com/channels/905194001349627914/905205848547155968 **(önerilen)**                     |
+------------------+-------------------------------------------------------------------------------------------------------+
| RocketChat       | :strikelink:`Rocket.Chat <https://chat.hyperledger.org/channel/iroha>`                                |
+------------------+-------------------------------------------------------------------------------------------------------+
| Wiki (Iroha 1)   | :strikelink:`Iroha 1 <https://lf-hyperledger.atlassian.net/wiki/spaces/iroha/pages/21015959/Iroha+1>` |
+------------------+-------------------------------------------------------------------------------------------------------+
| StackOverflow    | https://stackoverflow.com/questions/tagged/hyperledger-iroha                                          |
+------------------+-------------------------------------------------------------------------------------------------------+
| E-posta listesi  | hyperledger-iroha@lists.hyperledger.org                                                               |
+------------------+-------------------------------------------------------------------------------------------------------+
| Gitter           | https://gitter.im/hyperledger-iroha/Lobby (2023'ten beri devre dışı)                                  |
+------------------+-------------------------------------------------------------------------------------------------------+
| Telegram         | https://t.me/hl\_iroha                                                                                |
+------------------+-------------------------------------------------------------------------------------------------------+
| YouTube          | https://www.youtube.com/channel/UCYlK9OrZo9hvNYFuf0vrwww                                              |
+------------------+-------------------------------------------------------------------------------------------------------+


--------------

Dokümanı okuduğunuz için teşekkürler!
