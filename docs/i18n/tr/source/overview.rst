=====================
Iroha 1'e genel bakış
=====================

Iroha'nın temel özellikleri nelerdir?
-------------------------------------

- Basit dağıtım ve bakım
- Geliştiriciler için çeşitli kütüphaneler
- Rol tabanlı erişim kontrolü
- Komut ve sorguların ayrılmasına dayalı modüler mimari
- Varlık ve kimlik yönetimi

Kalite modelimizde şunlara odaklanıyor ve sürekli iyileştiriyoruz:

- Güvenilirlik (hata toleransı, kurtarılabilirlik)
- Verimlilik (özellikle zamanlama özellikleri ve kaynak kullanımı)
- Kullanılabilirlik (öğrenilebilirlik, kullanıcı hatalarını
  önleme, tanınabilirlik)

Iroha nerede kullanılabilir?
----------------------------

Hyperledger Iroha, dijital varlıkların, kimliklerin ve
serileştirilmiş verilerin yönetimi için uygulanabilen genel amaçlı
bir izin tabanlı blokzincir sistemidir. Bankalar arası mutabakat,
merkez bankası dijital paraları, ödeme sistemleri, ulusal kimlikler
ve tedarik zinciri gibi uygulamalar için yararlı olabilir.

Ayrıntılar için `kullanım senaryoları
<develop/cases.html>`_ bölümüne bakın.

Bitcoin veya Ethereum'dan farkı nedir?
--------------------------------------

.. list-table::
   :header-rows: 1
   :widths: 20 35 35

   * -
     - Bitcoin / Ethereum
     - Iroha 1
   * - Erişim modeli
     - Permissionless — herkes katılabilir ve tüm verileri
       okuyabilir
     - Permissioned — yalnızca bilinen peer'lar, verilen izinlerle
       etkileşebilir; sorgular da permissioned
   * - Yerel kripto para
     - Var — gas / işlem ücretlerini ödemek için gerekli
     - Yok — varlıklar kullanıcı tanımlı alan varlıklarıdır,
       para birimi gerekmez
   * - Kullanıcı mantığı
     - Akıllı sözleşmeler (Turing-tam, yazma ve denetim gerekir)
     - Yerleşik komutlar ve sorgular tipik işlemleri kapsar;
       standart görevler için sözleşme kodu gerekmez
   * - Konsensüs kesinliği
     - Olasılıksal (zamanla onaylar birikir)
     - Belirleyici — onaylanmış bloklar konsensüs commit'inden
       sonra kesindir

Iroha'da yerel kripto para yoktur. Bunun yerine sistemle etkileşim
permissioned'dir — yalnızca uygun erişime sahip kişiler
etkileşebilir. Sorgular da permissioned olduğundan tüm verilere
erişim kontrol edilebilir.

Ethereum'dan başlıca farklardan biri, Hyperledger Iroha'nın dijital
varlık oluşturma ve aktarma gibi tipik işlevleri yerleşik
komutlarla gerçekleştirmesidir. Bu, hantal ve test etmesi zor
akıllı sözleşmeler yazma gerekliliğini ortadan kaldırarak
geliştiricilerin basit görevleri daha hızlı ve güvenli şekilde
çözmesini sağlar.

Diğer Hyperledger çerçevelerinden ve diğer izin tabanlı
blokzincirlerden farkı nedir?
---------------------------------------------------------------

Iroha'nın, YAC [#f1]_ olarak adlandırılan yeni bir çökme hatasına
dayanıklı konsensüs algoritması vardır; yüksek performanslıdır ve
düşük gecikmeli işlem kesinliği sağlar.

Iroha'nın yerleşik komutları diğer platformlara göre büyük bir
avantajdır: dijital varlık oluşturma, hesap kaydı ve hesaplar
arası varlık aktarımı gibi tipik görevler son derece basit şekilde
yürütülür. Ayrıca bu, saldırı vektörünü daraltarak sistemin genel
güvenliğini artırır.

Son olarak, Iroha, tüm komutlara, sorgulara ve ağa katılıma izin
atanabilen güvenilir bir izin sistemine sahip tek ledger'dır.

.. [#f1] Yet Another Consensus

Iroha etrafında uygulamalar nasıl oluşturulur?
----------------------------------------------

En hızlı başlangıç, düğüm kurulumundan ilk işlem gönderimine kadar
rehberlik eden :doc:`Getting Started
<../getting_started/index>` kılavuzudur.

Uygulamanıza blokzincirin gücünü katmak için önce uygulamanın
Iroha peer'larıyla nasıl etkileşeceğini düşünün. İyi bir başlangıç
noktası, işlemin ve sorgunun ne olduğunu ve uygulamanızın
kullanıcılarının bunlarla nasıl etkileşmesi gerektiğini açıklayan
`Kavramlar ve Mimari
<concepts_architecture/index.html>`_ bölümüdür.

Ayrıca geliştiricilere yapı taşları sağlayan — imzalama,
komutlar, Iroha peer'larına mesaj gönderme ve durum kontrolü —
birkaç istemci kütüphanesi de mevcuttur.

Bilinen sınırlamalar ve proje durumu
-------------------------------------

Iroha 1 üzerinde bir şey oluşturmadan önce projenin mevcut durumunu
bilmelisiniz — bazı sınırlamalar hata değil, mimari veya tarihi
nedenlere dayanır:

- **Yaşam döngüsü**: upstream Hyperledger Iroha 1 EOL'dur; bu
  belgeler topluluk çatallanması ``baziorek/iroha1-cpp``'yi
  açıklar — tek bir bakımcı (AI yardımıyla) tarafından
  sürdürülür ve **üretim garantisi yoktur** — :doc:`index`
  sayfasındaki uyarıya bakın.
- **Konsensüs modeli**: Iroha 1 dağıtımları YAC'ı **CFT**
  (crash-fault tolerant, basit çoğunluk) modunda çalıştırır —
  tutarlılık modeli derleme zamanı sabitidir ve yapılandırmayla
  BFT'ye geçirilemez, ancak algoritma bunu destekler.
  :ref:`yac`'a bakın.
- **İstemci kütüphaneleri**: yalnızca C++ ``iroha-lib`` ve Python
  çatallanması ``iroha1-python`` sürdürülmektedir; Java,
  JavaScript ve iOS kütüphaneleri bakımsızdır (bkz.
  :doc:`develop/libraries`).
- **Çalışma zamanı konteyner imajı yok**: çatallanmanın CI'ı
  sürüm paketlerini (deb/rpm/AppImage) ve builder imajını
  yayınlar, ancak ``irohad`` içeren hazır bir Docker imajı yok —
  bkz. :ref:`build-guide`.
- **Performans**: Iroha 1'in mevcut toolchain için desteklenen
  bir performans testi paketi yoktur; eski sürüm aktarım
  rakamları geçerli olmayabilir.
- **API yüzeyi**: Torii'nin yerleşik hız sınırlaması yoktur —
  herkese açık uç noktaların önüne ters proxy koyun.
- **Uyumluluk**: Iroha 1 ve Iroha 2 **protokol ve durum
  düzeyinde uyumlu değildir** — yerinde geçiş yolu yoktur; bkz.
  :doc:`index` sayfasındaki not.
