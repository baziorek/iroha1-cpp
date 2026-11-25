********************************
Hyperledger Iroha Belgeleri
********************************

.. image:: ../image_assets/iroha_logo.png

.. warning::
    Dikkat: Hyperledger Iroha v1 artık **aktif olarak
    geliştirilmiyor**; Iroha v1 desteği sınırlıdır. Çekirdek
    geliştirme ekibi `Hyperledger Iroha v2
    <https://github.com/hyperledger/iroha/tree/iroha2-dev#hyperledger-iroha>`_
    — Iroha'nın Rust ile baştan yazımı — üzerinde çalışıyor.

    Yeni projeler için **Iroha v2 kullanmanızı öneririz**.
    Iroha v1 kullanıyorsanız geçiş planlamanızı ve Iroha v2'ye
    geçmeyi değerlendirmenizi öneririz.

    Uzun vadeli destekli Iroha v2 sürümü (`iroha2-lts`),
    üretimde Iroha kullanmak ve bizden teknik destek almak
    istiyorsanız en iyi seçimdir. Maalesef Iroha v1 için benzer
    bir destek sunamıyoruz; lütfen Iroha v2'yi değerlendirin.

    **Iroha v1 ve Iroha v2 uyumlu değildir**. İkisi arasındaki
    farklar `Iroha 2 belgelerinde
    <https://hyperledger.github.io/iroha-2-docs/guide/iroha-2.html>`_
    açıklanmıştır.

Hoş geldiniz! Hyperledger Iroha 1, izin tabanlı bir blokzincir ve
çökme hatasına dayanıklı konsensüsün gücünden yararlanan, güvenilir,
güvenli ve hızlı uygulamalar oluşturmanıza olanak tanıyan basit bir
blokzincir platformudur. Ücretsiz, açık kaynaklıdır ve Linux ile
macOS'ta çalışır; çeşitli mobil ve masaüstü kütüphaneleri sunar.

Hyperledger Iroha 1 kaynak kodu ve en son sürümler, topluluk
çatallanmasının `iroha1-cpp
<https://github.com/baziorek/iroha1-cpp>`_ sayfasında mevcuttur
(`releases
<https://github.com/baziorek/iroha1-cpp/releases>`_ sayfasında
hazır ikililer, ``.deb`` paketleri ve AppImage'ler bulunur;
C++ kaynak kodu ayrıca upstream deposu
`hyperledger-iroha/iroha
<https://github.com/hyperledger-iroha/iroha/tree/iroha1-main>`_
``iroha1-main`` dalında da mevcuttur).

Bu belgeler, bir Iroha ağını kurma, dağıtma ve çalıştırma ile ona
uygulama yazma sürecinde size rehberlik eder. Ayrıca şu anda hangi
kullanım senaryolarının mümkün olduğunu ve hangilerinin
planlandığını da ele alacağız.

.. note::
    `iroha.readthedocs.io
    <https://iroha.readthedocs.io/>`_ adresindeki resmi
    ReadTheDocs belgelerinden **daha güncelsiniz**. Bu, Iroha v1'i
    derlenebilir durumda tutan topluluk çatallanması `iroha1-cpp
    <https://github.com/baziorek/iroha1-cpp>`_ belgeleridir; resmi
    sürüm Iroha v1 için artık güncellenmiyor.

    İlgili Python istemcisi, `iroha1-python
    <https://github.com/baziorek/iroha1-python>`_ çatallanmasıdır:
    orijinal ``iroha-python`` kaynağı **değiştirilmeden**, modern
    bağımlılıklarla yeniden derlenmiş olup mevcut sistemlerde
    kurulup çalışır.

.. note::
    Iroha v1, üretimde kanıtlanmış olgun bir platformdur. Kamboçya
    Ulusal Bankası'nın ulusal ödeme sistemi `Bakong
    <https://www.nbc.gov.kh/english/economic_research/bakong.php>`_
    Ekim 2020'den beri üretimde çalıştırmaktadır — onlarca milyon
    hesap ve yılda bir milyardan fazla işlem, bankalar arası gerçek
    zamanlı mutabakat ile. Teknoloji ayrıca akademik ve araştırma
    projelerinde de kullanılmaktadır.

.. warning::
    ``iroha1-cpp`` ve ``iroha1-python`` çatallanması **Iroha'nın
    kendisinde işlevsel değişiklik içermez** — yalnızca derleme
    sistemi güncellemeleri, bağımlılık yükseltmeleri ve bakım
    işleri. Bunlar, sistemi yıllarca tasarlayan, test eden ve işleten
    orijinal ekip **tarafından değil**, bir kişi (AI yardımıyla)
    tarafından sürdürülmektedir. Değişiklikler güvenli olmalıdır
    ancak **üretim veya ticari uygunluk garantisi vermiyoruz**.
    Ticari bir dağıtım planlıyorsanız çatallanmayı dikkatle
    değerlendirin veya resmi olarak desteklenen `Hyperledger Iroha v2
    <https://github.com/hyperledger/iroha>`_'yi göz önünde
    tutun.

Iroha açık kaynaklı bir projedir; bu nedenle koda katkıda bulunma
ilkelerini ve değişiklik gönderme sürecini de ele alacağız.

.. toctree::
    :maxdepth: 2
    :numbered:
    :caption: İçindekiler

    overview.rst
    concepts_architecture/index.rst
    getting_started/index.rst
    integrations/index.rst
    build/index.rst
    configure/index.rst
    deploy/index.rst
    maintenance/index.rst
    develop/index.rst
    community/index.rst
    faq/index.rst
