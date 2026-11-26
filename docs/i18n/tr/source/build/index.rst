.. _build-guide:

================
Iroha'yı Derleme
================

Bu kılavuzda Iroha'yı derlemek için gereken tüm
bağımlılıkların nasıl kurulacağını ve onu nasıl
aslında derleyeceğimizi öğreneceğiz.

3 adım olacak:

#. Ortam ön koşullarını yükleme

#. Iroha bağımlılıklarını yükleme (Docker için
   otomatik olarak gerçekleştirilecek)

#. Iroha'yı derleme

.. note:: Iroha'yı kullanmaya başlamak için derlemeniz
  gerekmez. Bunun yerine Hub'dan hazır bir Docker
  imajı indirebilirsiniz; bu süreç bu dokümantasyonun
  :ref:`getting-started` sayfasında ayrıntılı olarak
  açıklanmıştır.

Ön Koşullar
===========

Iroha'yı başarıyla derlemek için ortamı
yapılandırmamız gerekir. Bunu yapmanın birkaç yolu
vardır ve hepsini açıklayacağız.

Şu anda Unix benzeri sistemleri destekliyoruz (temelde
popüler Linux dağıtımlarını ve MacOS'u hedefliyoruz).
Windows'unuz varsa veya tüm bağımlılıkları kurmak için
zaman harcamak istemiyorsanız Docker ortamını
kullanmayı düşünebilirsiniz. Windows kullanıcıları
`WSL <https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux>`_
kullanmayı da düşünebilir.

Teknik olarak Iroha, Windows altında deneysel modda
yerel olarak derlenebilir. Bu kılavuz o yolu da
kapsar. Yerel Windows derlemesiyle ilgili tüm
aşamalar, önemli farklılıkları nedeniyle ana akıştan
ayrılmıştır.

Hızlı erişim için lütfen aşağıdan tercih ettiğiniz
platformu seçin:

    - :ref:`docker-pre`
    - :ref:`linux-pre`
    - :ref:`MacOS-pre`
    - :ref:`Windows-pre`


.. hint:: Sorun mu yaşıyorsunuz? SSS bölümünü kontrol
  edin veya takılırsanız bizimle doğrudan iletişime
  geçin. Bunun olmasını beklemiyoruz ancak ortamla
  ilgili bazı sorunlar mümkün.

.. _docker-pre:

Docker
^^^^^^

Güncel Docker imajlarına sahip olma fikri, Iroha'yı
derlemeden çalıştırabilmektir. Ama artık yalnızca
kullanıma hazır imajları değil, aynı zamanda Iroha'yı
dilediğiniz gibi derlemek için Docker içinde özel bir
**Iroha Builder** çalıştırma seçeneğiniz de var.

Öncelikle ``docker`` ve ``docker-compose``'u
kurmanız gerekir. Nasıl kurulacağını
`Docker'ın web sitesinden
<https://www.docker.com/community-edition/>`_
okuyabilirsiniz.

.. note:: Lütfen en son mevcut docker daemon ve
  docker-compose'u kullanın.

Iroha İmajları
""""""""""""""

Tüm Iroha Docker imajlarını `Docker Hub'da
aranarak
<https://hub.docker.com/search?q=hyperledger%2Firoha&type=image>`_
veya `GitHub'da
<https://github.com/orgs/hyperledger/packages?repo_name=iroha>`_
bulabilirsiniz.

Şu anda aşağıdaki imajlar mevcut:

- ``iroha`` — Iroha 1.x genel derlemesi (Docker Hub
  üzerindeki upstream artefaktı);
- ``iroha-burrow`` — Iroha 1.x ile `Burrow
  entegrasyonu
  <../integrations/index.html#hyperledger-burrow>`_ içeren
  derleme (kullanımdan kaldırıldı: Burrow EOL);
- ``iroha-ursa`` — Iroha 1.x ile `Ursa
  entegrasyonu
  <../integrations/index.html#hyperledger-ursa>`_ içeren
  derleme (kullanımdan kaldırıldı: Ursa EOL);
- ``iroha2`` — Iroha 2 derlemesi;
- ``iroha-builder`` — Docker içinde bir Iroha
  derleyicisi çalıştırmanıza olanak tanıyan özel
  imaj.

.. hint:: İmajların çalıştırılması hakkında daha
  fazlasını `Hızlı Başlangıç Kılavuzunda
  <../getting_started/index.html>`_ okuyabilirsiniz.

Her imaj, imajın oluşturulduğu dalı gösteren ilgili
tag ile kullanılabilir. Tüm mevcut tagleri Docker
Hub'da bulabilirsiniz. ``iroha`` imajı için
`işte tüm tagler
<https://hub.docker.com/r/hyperledger/iroha/tags>`_.

Örneğin ``iroha:1.6.0`` son Iroha 1 derlemesidir.
``latest``, ``develop`` veya ``main`` taglerini
**kullanmayın** — bunlar artık Iroha 2 (Rust)
derlemelerini veya bayat 2022 artefaktlarını
gösterir.

GHCR'daki Topluluk-Forku İmajları
"""""""""""""""""""""""""""""""""

``baziorek/iroha1-cpp`` forkunun CI'si ayrıca imajları
``ghcr.io/baziorek/iroha1-cpp`` altında **GitHub
Container Registry**'ye yayınlar. Şu anda yalnızca
**builder** imajı yayınlanır (workflow'da
``docker/release`` runtime imaj işleri devre dışı),
dal adlarıyla eşleşen taglerle — örn.
``iroha1-main`` — ve release tagleri:

.. code-block:: shell

  docker pull ghcr.io/baziorek/iroha1-cpp:iroha1-main

Aşağıdaki ``hyperledger/iroha-builder`` ile aynı
şekilde kullanın, ancak mevcut fork kaynaklarından
derlendiğini, bu nedenle araç zincirinin bugün
deponun ihtiyaç duyduğu ile eşleştiğini unutmayın.


Iroha Builder
""""""""""""""

.. note:: ``hyperledger/iroha-builder`` imajı,
  Iroha 1 upstream'de kullanımdan kaldırıldığından
  beri yeniden derlenmemiş bir upstream artefaktıdır;
  araç zinciri mevcut kaynaklar için çok eski
  olabilir. Yerel olarak veya kendi konteynerinizde
  derlemeyi tercih edin — imaj yeterince yeniyse
  aşağıdaki adımlar yine de çalışır.

Iroha builder, Iroha'yı başka bir platform için
`mevcut parametrelerin
<#cmake-parameters>`_ herhangi biriyle, ancak
Docker'da kullanışlı ve güvenli bir şekilde
derlemenizi sağlar.

İşte adımlar:

1. Öncelikle builder'ı çalıştıralım:

.. code-block:: shell

  docker run -it hyperledger/iroha-builder:latest

Bu adımda imajı interaktif modda bir konteynerde
başlatıp çalıştıracaksınız. Mevcut herhangi bir
tag'i kullanabilirsiniz, varsayılan ``latest``,
geliştirme tagi ``develop``'dir. Bazı işlemleri
``sudo`` haklarıyla gerçekleştirmeniz gerekebileceğini
unutmayın.

2. Konteynerin içindeyken Iroha deposunu klonlayın:

.. code-block:: shell

  git clone https://github.com/baziorek/iroha1-cpp.git

3. Iroha klonlandıktan sonra Iroha klasörüne girin:

.. code-block:: shell

  cd iroha1-cpp

4. Ardından vcpkg ile tüm gerekli bağımlılıkları
   derleyecek betiği çalıştırın:

.. code-block:: shell

  ./vcpkg/build_iroha_deps.sh $PWD/vcpkg-build



.. _linux-pre:

Linux
^^^^^

Iroha'yı derlemek için aşağıdaki paketlere
ihtiyacınız olacak:

``build-essential`` ``git`` ``ca-certificates``
``tar`` ``ninja-build`` ``curl`` ``unzip`` ``cmake``

Debian tabanlı bir Linux dağıtımında ortam
bağımlılıklarını kurmak için bu kodu kullanın.

.. code-block:: shell

  apt-get update; \
  apt-get -y --no-install-recommends install \
  build-essential ninja-build \
  git ca-certificates tar curl unzip cmake \
  pkg-config zip

.. note:: WSV-diff test aracı
  ``test/tool/test_tool_iroha_wsv_diff.sh`` ayrıca
  testleri çalıştıran makinede ``jq`` ve
  ``postgresql-client`` (``psql`` için) gerektirir.

.. Important:: `Burrow
  entegrasyonunu
  <../integrations/burrow.html>`_ kullanmak
  istiyorsanız ayrıca GO'ya ihtiyacınız olacak.
  `Resmi web sitesindeki
  <https://golang.org/doc/install>`_ talimatları
  izleyerek kurun ve ardından şu komutu kullanın:

.. code-block:: shell

  go get github.com/golang/protobuf/protoc-gen-go

.. note::  Iroha'yı aktif olarak geliştirip paylaşımlı
  kitaplıklar derlemek istiyorsanız, CMake'in
  `en son sürümünü
  <https://cmake.org/download/>`_ kurmayı
  düşünün.

RaspberryPi 4
""""""""""""""""""""""""

Raspberry Pi 4'te Iroha derlemek için Linux'ta
derleme için aynı talimatları izleyin.

**DİKKAT**: Iroha 8GiB RAM gerektirir. Derlemeniz
`SIGKILL` ile sonlanırsa ana bilgisayarda bir takas
dosyası veya takas bölümü oluşturmaya veya çapraz
derlemeye bakın.

Artık `Iroha bağımlılıklarını
<#installing-dependencies-with-vcpkg-dependency-manager>`_
kurmaya hazırsınız.

.. _macos-pre:

MacOS
^^^^^

Iroha'yı sıfırdan derlemek ve aktif olarak
geliştirmek istiyorsanız, tüm ortam
bağımlılıklarını Homebrew ile kurmak için şu kodu
kullanın:

.. code-block:: shell

  xcode-select --install
  brew install cmake go pkg-config coreutils ninja git gcc@9

.. hint:: Homebrew'un kendisini kurmak için şunu
  çalıştırın

  ``ruby -e "$(curl -fsSL https://raw.githubusercontent.com/homebrew/install/master/install)"``

.. Important:: `Burrow
  entegrasyonunu
  <../integrations/burrow.html>`_ kullanmak
  istiyorsanız ayrıca GO'ya ihtiyacınız olacak.
  `Resmi web sitesindeki
  <https://golang.org/doc/install>`_ talimatları
  izleyerek kurun ve ardından şu komutu kullanın:

.. code-block:: shell

  go get github.com/golang/protobuf/protoc-gen-go

Artık `Iroha bağımlılıklarını
<#installing-dependencies-with-vcpkg-dependency-manager>`_
kurmaya hazırsınız.

.. _windows-pre:

Windows
^^^^^^^

.. note:: Listelenen tüm komutlar Iroha'nın 64-bit
  sürümünü derlemek için tasarlanmıştır.

Chocolatey Paket Yöneticisi
"""""""""""""""""""""""""""""

Öncelikle Chocolatey paket yöneticisinin kurulu olması
gerekir. Chocolatey kurulumu için `kılavuza
<https://chocolatey.org/install>`_ bakın.

Araç Setini Derleme
""""""""""""""""""""""""

Komut isteminin Yönetici modunda chocolatey ile CMake,
Git, Microsoft derleyicilerini kurun:

.. code-block:: shell

  choco install cmake git visualstudio2019-workload-vctools ninja


PostgreSQL bir derleme bağımlılığı değildir ancak
daha sonraki testler için şimdi kurmanız önerilir:

  .. code-block:: shell

    choco install postgresql
    # Ayarladığınız parolayı unutmayın!

Artık `Iroha bağımlılıklarını
<#installing-dependencies-with-vcpkg-dependency-manager>`_
kurmaya hazırsınız.

Vcpkg Bağımlılık Yöneticisiyle Bağımlılıkları Kurma
====================================================

Şu anda tüm platformlar — Linux, Windows ve MacOS —
için bağımlılık yöneticisi olarak Vcpkg kullanıyoruz.
Gereken yamaların çalışacağından emin olmak için Vcpkg'nin
sabit bir sürümünü (``vcpkg/VCPKG_COMMIT_SHA`` içinde
sabitlenmiş) kullanıyoruz.

Gerekli bağımlılıkların tam listesi tek bir yerde
tutulur — ``vcpkg/VCPKG_DEPS_LIST`` — ve aşağıda
yeniden üretilmiştir. Vcpkg her port'u sabitlenmiş
vcpkg taban çizgisinde mevcut en yeni sürüme çözer;
bunlar başarılı bir derleme için **minimum desteklenen
sürümlerdir**:

.. literalinclude:: ../../../vcpkg/VCPKG_DEPS_LIST
   :language: text

.. note:: vcpkg dışında derliyorsanız (örn. sistem
  paketleri), sürümlerinizin sabitlenmiş vcpkg taban
  çizgisinin sağladıklarından **daha eski olmadığından**
  emin olun. Özellikle kod **soci 4.x** (``soci/std-
  optional.h`` kullanır), **gRPC kod üretimli protobuf**,
  **Boost >= 1.73** ve **OpenSSL 3.x** gerektirir.

Bu kararlı sürüm yalnızca Iroha deposu içinde
bulunabilir, bu nedenle Iroha'yı klonlamamız
gerekecek. Tüm süreç tüm platformlar için oldukça
benzerdir ancak tam komutlar biraz farklıdır.

Linux ve MacOS
^^^^^^^^^^^^^^

Terminalde çalıştırın:

.. code-block:: shell

  git clone https://github.com/baziorek/iroha1-cpp.git
  cd iroha
  ./vcpkg/build_iroha_deps.sh $PWD/vcpkg-build

Hepsi bu! Artık `Iroha'yı Derleme
<#build-process>`_ bölümüne geçebilirsiniz.

Windows
^^^^^^^

Power Shell'den çalıştırın:

.. code-block:: shell

  git clone https://github.com/baziorek/iroha1-cpp.git
  cd iroha
  powershell -ExecutionPolicy ByPass -File .\.packer\win\scripts\vcpkg.ps1 .\vcpkg .\iroha\vcpkg


Harika iş! Artık `Iroha'yı Derleme
<#build-process>`_ bölümüne geçebilirsiniz.

.. note:: Iroha'nın 32-bit sürümünü derlemeyi
  planlıyorsanız — yukarıda bahsedilen tüm kitaplıkları
  ``x64`` yerine ``x86`` terimiyle öne ekleyerek
  kurmanız gerekecek.

Derleme Süreci
==============

Iroha'yı Derleme
^^^^^^^^^^^^^^^^

1. Bağımlılıklar derlendikten sonra Iroha'nın kendisini
dermeye başlayabiliriz:

.. code-block:: shell

  cmake -B build -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg-build/scripts/buildsystems/vcpkg.cmake . -DCMAKE_BUILD_TYPE=RELEASE   -GNinja -DUSE_BURROW=OFF -DUSE_LIBURSA=OFF -DTESTING=OFF -DPACKAGE_DEB=OFF

``-DUSE_BURROW=ON`` gibi cmake parametreleri tam da
çok özel derlemeniz için seçebileceğiniz
parametrelerdir. Bu parametrelerin tam listesini ve
açıklamasını `burada
<#cmake-parameters>`_ görebilirsiniz.

2. Çalıştırın

.. code-block:: shell

  cmake --build ./build --target irohad

.. warning:: Daha sonra testleri kullanmak istiyorsanız,
  ``irohad`` hedefini derlemek yerine bunu
  kullanmanız gerekir:

.. code-block:: shell

  cmake --build ./build --target all 


3. Yardım komutunu çalıştırarak sonucu kontrol edin:

.. code-block:: shell

  ./build/bin/irohad --help

Bu adım size tüm parametreleri gösterecektir. Hepsi
bu!

.. note:: Windows'ta derlerken bunu Power Shell'den
  çalıştırmayın. Bunun yerine x64 Native tools
  komut istemini kullanmak daha iyidir.

Artık Iroha derlendi. İsterseniz aşağıda açıklanan
ek parametrelerle de derleyebilirsiniz.

Sonuçlardan memnunsanız bir sonraki adıma geçip
`bir Iroha örneği çalıştırabilirsiniz
<../deploy/single.html>`_.

CMake Parametreleri
^^^^^^^^^^^^^^^^^^^

Platformdan bağımsız derleme dosyaları oluşturmak
için CMake kullanıyoruz. Nihai derlemeyi
yapılandırmak için çok sayıda bayrağa sahiptir.
Listelenen parametrelerin yanı sıra cmake
değişkenlerinin de yararlı olabileceğini unutmayın.
Ayrıca bu sayfa kullanımdan kalkabileceğinden (veya
tam olmayabileceğinden) özel bayrakları ``cmake -L``,
``cmake-gui`` veya ``ccmake`` ile
inceleyebilirsiniz.

.. hint::  Parametreleri cmake yapılandırma
  aşamasında belirtebilirsiniz (örn. cmake
  -DTESTING=ON).

Ana Parametreler
"""""""""""""""""

.. list-table::
   :header-rows: 1

   * - Parametre
     - Olası değerler
     - Varsayılan
     - Açıklama
   * - TESTING
     - ON/OFF
     - ON
     - Testlerin derlemesini etkinleştirir veya
       devre dışı bırakır
   * - BENCHMARKING
     - ON/OFF
     - OFF
     - Google Benchmarks kütüphanesinin derlemesini
       etkinleştirir veya devre dışı bırakır
   * - COVERAGE
     - ON/OFF
     - OFF
     - Kod kapsamı üretimi için lcov ayarını
       etkinleştirir veya devre dışı bırakır
   * - USE_LIBURSA
     - ON/OFF
     - OFF
     - Standart olanı yerine HL Ursa kriptografisinin
       kullanılmasını etkinleştirir
   * - USE_BURROW
     - ON/OFF
     - OFF
     - HL Burrow EVM entegrasyonunu etkinleştirir

.. warning:: ``USE_LIBURSA`` ve ``USE_BURROW``
  seçenekleri **kullanımdan kaldırılmıştır**:
  Hyperledger Ursa 27 Nisan 2023'te, Hyperledger
  Burrow 12 Mayıs 2022'de End of Life'a ulaştı. Her
  iki entegrasyon yalnızca eski dağıtımlarla uyumluluk
  için saklanmaktadır.

.. note:: Derlemeniz için HL Ursa kriptografisini
  kullanmak istiyorsanız, diğer bağımlılıkların
  yanına `Rust
  <https://www.rust-lang.org/tools/install>`_ de
  kurun. HL Ursa entegrasyonu hakkında daha fazla
  bilgi `burada
  <../integrations/index.html#hyperledger-ursa>`_.

  HL Burrow entegrasyonunu kullanmak istiyorsanız
  önce `Go
  <https://golang.org/doc/install>`_ ve ardından
  `protoc-gen-go
  <https://developers.google.com/protocol-buffers/docs/reference/go-generated>`_
  kurmayı unutmayın. `HL Burrow entegrasyonu
  hakkında daha fazla bilgi
  <../integrations/index.html#hyperledger-burrow>`_.

Paketlemeye Özgü Parametreler
""""""""""""""""""""""""""""""""

.. list-table::
   :header-rows: 1

   * - Parametre
     - Olası değerler
     - Varsayılan
     - Açıklama
   * - PACKAGE_ZIP
     - ON/OFF
     - OFF
     - Zip paketlemeyi etkinleştirir veya devre dışı
       bırakır
   * - PACKAGE_TGZ
     - ON/OFF
     - OFF
     - Tar.gz paketlemeyi etkinleştirir veya devre
       dışı bırakır
   * - PACKAGE_RPM
     - ON/OFF
     - OFF
     - Rpm paketlemeyi etkinleştirir veya devre dışı
       bırakır
   * - PACKAGE_DEB
     - ON/OFF
     - OFF
     - Deb paketlemeyi etkinleştirir veya devre dışı
       bırakır

Testleri Çalıştırma (isteğe bağlı)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Öncelikle testler için Iroha'yı `doğru
<#build-process>`_ derlediğinizden emin olun.

Iroha'yı derledikten sonra, demonun çalışabilirliğini
kontrol etmek için testleri çalıştırmak iyi bir
fikir. Testleri şu kodla çalıştırabilirsiniz:

.. code-block:: shell

  cmake --build build --target test

Alternatif olarak ``build`` klasöründe şu komutu
çalıştırabilirsiniz:

.. code-block:: shell

  cd build
  ctest . --output-on-failure

.. note:: Bazı testler PostgreSQL deposu çalışmadan
  başarısız olur, bu nedenle
  ``scripts/run-iroha-dev.sh`` betiğini kullanmıyorsanız
  bir Docker konteyneri çalıştırın veya aşağıdaki
  parametrelerle yerel bir bağlantı oluşturun:

  .. code-block:: shell

    docker run --name some-postgres \
    -e POSTGRES_USER=postgres \
    -e POSTGRES_PASSWORD=mysecretpassword \
    -p 5432:5432 \
    -d postgres:9.5 \
    -c 'max_prepared_transactions=100'
