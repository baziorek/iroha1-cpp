# Hoş geldiniz!

**Diller:** [English](../../../README.md) · [Polski](../pl/README.md) · [Русский](../ru/README.md) · [日本語](../ja/README.md) · **Türkçe**

## Bu depo hakkında

Bu, **Hyperledger Iroha 1**'in (C++ uygulaması) topluluk fork'udur.
Upstream geliştirme fiilen durmuş durumda — son herkese açık commit
**Mayıs 2024** tarihli — ancak Iroha 1, Kamboçya Ulusal Bankası
(Bakong ödeme sistemi) dahil birçok girişim tarafından production'da
hâlâ yoğun olarak kullanılmaktadır.

Bu fork'un amacı, Iroha 1'i modern toolchain'lerle derlenebilir,
kullanılabilir ve sürdürülebilir tutmak — ve projeye sağlıklı bir
ikinci hayat vermektir.

### Bu fork upstream'den nasıl farklıdır?

Son upstream commit'i (`118fced3f3`, Mayıs 2024) ile karşılaştırıldığında,
bu fork şunları ekler:

**Derleme modernizasyonu**

- C++ standardı **C++23**'e yükseltildi; derleme **GCC 14 /
  Ubuntu 24.04** üzerinde doğrulandı
- **vcpkg** taban çizgisi 2021'den Kasım 2026'ya güncellendi
  (protobuf, gRPC, Boost, OpenSSL 3.x, soci 4.x)
- Standart kütüphane yeterli olduğu yerde eski bağımlılıklar
  değiştirildi (`fmt` → `std::format`, `boost::format`,
  `boost::variant`, `boost::filesystem` ve kullanılmayan birkaç Boost
  kütüphanesi kaldırıldı)
- Uyarısız derleme (~70 uyarı temizlendi)

**Yeni bileşenler**

- **`iroha-lib/`** — Iroha 1 için bağımsız C++ istemci kütüphanesi
  (transaction'lar, query'ler, sayfalama, streaming, TLS transport,
  anahtar/genesis/peer listesi yardımcıları), `iroha-python`'dan
  taşınmış 9 örnekle birlikte. `iroha-lib/README.md` ve
  `docs/source/develop/libraries/cpp.rst` dosyalarına bakın.
- **`iroha-cli/`** — klasik interaktif/non-interaktif CLI;
  transport ve model oluşturma için `iroha-lib`'e taşındı,
  `--tls_root_cert` desteğiyle.
- **`iroha-python`** — Python istemci kütüphanesi ayrı bir depoda
  bulunuyor: [baziorek/iroha1-python](https://github.com/baziorek/iroha1-python)
  (orijinal kaynaklar, güncellenmiş bağımlılıklar).

**Sunucu düzeltmeleri**

- Tüm sunucularda gRPC **keepalive** — NAT/LB arkasında bağlantıların
  kopmasına ve uzun ömürlü stream'lerin yaklaşık her 24 saatte bir
  başarısız olmasına yol açan sorunu düzeltir (upstream issue
  hyperledger/iroha#1351)
- `FetchCommits` takılma düzeltmesi, `GetPendingTransactions` oneof
  düzeltmesi, `creator_account_id` düzeltmeleri — hepsi yeni
  entegrasyon testleriyle bulundu

**Release ve CI**

- **CPack** DEB/RPM paketleri + AppImage artefaktları; sürüm
  etiketlerinde **GitHub Releases** (`1.6.1-reanimation` upstream
  sürüm hattını sürdürür; paket sürümleri `git describe`'dan
  türetilir)
- Modernize GitHub Actions: Node 24 runner'ları, commit SHA'sına
  sabitlenmiş action'lar, hash-kilitli pip bağımlılıkları, paylaşılan
  vcpkg bağımlılık önbellek job'u
- **OpenSSF Scorecard** + **SECURITY.md** + Dependabot;
  `iroha1-main` üzerinde branch protection
- Yük testi yığını `locustio`'dan `locust` 2.x'e taşındı

**Dokümantasyon**

- Sphinx dokümantasyonu yeniden oluşturuldu ve **GitHub Pages**'e
  yayınlandı: <https://baziorek.github.io/iroha1-cpp/>
- **Lehçe, Rusça, Japonca ve Türkçe**'ye tam çeviriler — bu README
  dahil (üstteki dil bağlantılarına bakın)

**Kaldırılan ölü kod**

- `snap/` paketleme (modern bağımlılıklarla derlenemiyor)
- `housekeeping/` betikleri (işlevsiz Soramitsu CI'sı), `utils/`
  betikleri (yerini aldı; genesis ayrıştırma Python fork'una
  taşındı)
- Bayat vcpkg yama dosyaları ve Iroha 2 CI workflow'ları

> **Sorumluluk reddi:** Bu topluluk fork'ları, yapay zekâ desteğiyle
> tek bir kişi tarafından sürdürülmektedir. Bakım, derleme sistemi ve
> bağımlılık güncellemeleri içerir — Iroha'nın işlevselliğinde değişiklik
> yoktur — ancak **production veya ticari kullanım garantisi vermez**.
> Iroha 1'in kendisi, gerçek dağıtımları olan olgun bir projedir
> (örn. Kamboçya Ulusal Bankası'nın işlettiği Bakong ödeme sistemi).

Bu README'nin geri kalanı upstream projesini anlatır ve geçerliliğini
korur.

## Hyperledger Iroha nedir?

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/960/badge)](https://bestpractices.coreinfrastructure.org/projects/960)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/baziorek/iroha1-cpp/badge.svg?branch=iroha1-main)](https://scorecard.dev/viewer/?uri=github.com/baziorek/iroha1-cpp)

Iroha, Japon Kaizen ilkesinden — aşırılığın (muri) ortadan kaldırılması —
ilham alan sade bir dağıtık defter teknolojisidir (DLT). Iroha,
varlık, bilgi ve kimlik yönetimi ihtiyaçlarınız için temel işlevselliğe
sahiptir ve aynı zamanda kurumsal ihtiyaçlarınız için verimli ve
güvenilir bir çökme hatası toleranslı (crash fault-tolerant) araçtır.

[Dokümantasyonumuzun](https://baziorek.github.io/iroha1-cpp/) genel
bakış sayfasına bakın.
Toplantıları ve açıklayıcı videoları yüklediğimiz
[YouTube kanalımız](https://www.youtube.com/channel/UCYlK9OrZo9hvNYFuf0vrwww)
burada — bir göz atın!

<img height="300px" src="../../../docs/image_assets/Iroha_3_sm.png"
 alt="Iroha logosu" title="Iroha" align="right" />

Iroha'nın şu özellikleri vardır:
1. Para birimleri, kilogram altın vb. özel değiştirilebilir varlıkların oluşturulması ve yönetimi
2. Kullanıcı hesaplarının yönetimi
3. Sistemdeki _alan adlarına_ dayalı hesap taksonomisi
4. Sistemde transaction ve query yürütme için kullanıcı izinlerinin doğrulanması ve doğrulama sistemi
5. Sistemdeki transaction ve query'ler için iş kurallarının doğrulanması
6. Çoklu imzalı transaction'lar

Iroha _Crash Fault Tolerant_'tır ve kendi konsensüs algoritmasına —
[YAC](https://arxiv.org/pdf/1809.00554.pdf) — sahiptir.
Iroha 2'de, yeni bir algoritma —
[Sumeragi](https://iroha.tech/#sumeragi) — ile değiştirilmiştir.

## Dokümantasyon

Bu fork'un dokümantasyonu Sphinx ile oluşturulur ve **GitHub Pages**'te
yayınlanır: <https://baziorek.github.io/iroha1-cpp/> — İngilizce'nin
yanında Lehçe, Rusça, Japonca ve Türkçe'de de (dokümantasyondaki dil
seçici ve bu README'deki dil bağlantıları).

#### Dokümantasyon çevirilerimiz var!

Bu fork, `docs/i18n/` altında tutulan **Lehçe, Rusça, Japonca ve
Türkçe**'ye tam çeviriler içerir (upstream'in kendi
[yerelleştirme deposu](https://github.com/hyperledger/iroha-docs-l10n)
daha eski anlık görüntüleri kapsar — pull request'lerle kendi dilinize
çevirmenize yardımcı olmaktan çekinmeyin).

### Iroha'yı gerçekten hızlıca nasıl keşfedersiniz?

Sistemi keşfetmeye başlamak için dokümantasyondaki
[başlarken](https://baziorek.github.io/iroha1-cpp/getting_started/index.html)
bölümüne bakın.
Eski Katacoda sandbox'ı artık yok, ancak
[entegrasyon testleri](../../../test/) ve
[`iroha-lib` örnekleri](../../../iroha-lib/) canlı bir düğümü
hareket halinde gösterir.

### Iroha nasıl derlenir?

Kodu değiştirmek ve katkıda bulunmak istiyorsanız
[derleme kılavuzunu](https://baziorek.github.io/iroha1-cpp/build/index.html)
kullanın — yardımcı olabilir.

### SDK mevcut mu?

Evet — bu fork için: bu depodaki [`iroha-lib/`](../../../iroha-lib/)
içinde modern bir **C++ istemci kütüphanesi** ve bakımı yapılan bir
**Python** istemcisi
[baziorek/iroha1-python](https://github.com/baziorek/iroha1-python).
Upstream'de ayrıca
[Java](https://github.com/hyperledger/iroha-java),
[Python](https://github.com/hyperledger/iroha-python),
[JavaScript](https://github.com/hyperledger/iroha-javascript) ve
[iOS](https://github.com/hyperledger/iroha-ios) SDK'ları da var
(bakımı yapılmıyor, referans için tutuluyor).

### Örnek uygulamalar var mı?

[Android point uygulaması](https://github.com/hyperledger/iroha-android/tree/master/iroha-android-sample)
ve [JavaScript cüzdanı](https://github.com/soramitsu/iroha-wallet-js).

[The Borsello App](https://github.com/claudiocandio/borsello)'i de
unutmayın — Android ve iOS için bir cüzdan ile birlikte web tarayıcı
uygulaması, [Claudio](https://github.com/claudiocandio) tarafından
katkıda bulunuldu. JS Wallet tabanlıdır ama günceldir.

### Iroha için harika araçlar

[Iroha Explorer](https://codeberg.org/diva.exchange/iroha-explorer)
[Postgres 10 ile Iroha Docker konteyneri](https://hub.docker.com/r/divax/iroha)
ve [kaynak kodu](https://codeberg.org/diva.exchange/iroha)
[Iroha örneklerini dağıtmak için araç](https://github.com/kuvaldini/iroha-swarm)

### Iroha'nın geliştirilmesine yardımcı olmak ister misiniz?

Harika!
[CONTRIBUTING.rst](../../../CONTRIBUTING.rst)'ye bakın — bu deponun
[GitHub Issues](https://github.com/baziorek/iroha1-cpp/issues)
üzerinden sorunlar ve iyileştirme önerileri memnuniyetle karşılanır.

## Yardıma mı ihtiyacınız var?

* [Telegram sohbetine](https://t.me/hyperledgeriroha) veya
[Hyperledger RocketChat](https://chat.hyperledger.org/channel/iroha)'e
katılın; burada bakımcılar, katkıda bulunanlar ve kullanıcılar size
yardımcı olmaya hazır. Endişelerinizi ve önerilerinizi orada veya
Gitter'da da tartışabilir ve sadece Iroha hakkında sohbet edebilirsiniz
[![https://gitter.im/hyperledger-iroha/Lobby sohbetine katılın](https://badges.gitter.im/hyperledger-iroha/Lobby.svg)](https://gitter.im/hyperledger-iroha/Lobby)
* Sorunları ve iyileştirme önerilerini
[GitHub Issues](https://github.com/baziorek/iroha1-cpp/issues)
üzerinden gönderin — upstream Iroha 2 soruları için
[Hyperledger Jira](https://jira.hyperledger.org/secure/CreateIssue!default.jspa)'ya
bakın
* En son ve en önemli haberleri almak ve Iroha topluluğunda sözünüzü
  yaymak için [mailing listemize](https://lists.hyperledger.org/g/iroha)
  abone olun

## Lisans

Iroha kod tabanı Apache License,
Version 2.0 ("Lisans") altında lisanslanmıştır; bu dosyayı Lisans'a
uygun olarak kullanamazsınız. Lisansın bir kopyasını
http://www.apache.org/licenses/LICENSE-2.0 adresinden edinebilirsiniz

Yürürlükteki yasalarca zorunlu kılınmadıkça veya yazılı olarak
anlaşılmadıkça, Lisans kapsamında dağıtılan yazılım "OLDUĞU GİBİ"
dağıtılır, AÇIK VEYA ZIMNİ HİÇBİR TÜR GARANTİ VEYA KOŞUL İÇERMEZ.
Belirli dildeki haklar ve sınırlamalar için Lisans'a bakın.

Iroha dokümantasyon dosyaları Creative Commons
Attribution 4.0 International License (CC-BY-4.0) kapsamında
sunulmaktadır, http://creativecommons.org/licenses/by/4.0/
adresinden erişilebilir
