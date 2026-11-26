.. _build-guide:

==================
Kompilowanie Irohy
==================

W tym przewodniku nauczymy się, jak zainstalować wszystkie
zależności wymagane do kompilowania Irohy i jak ją faktycznie
skompilować.

Będą 3 kroki:

#. Instalacja wymagań wstępnych środowiska

#. Instalacja zależności Irohy (zostanie wykonana
   automatycznie dla Dockera)

#. Kompilowanie Irohy

.. note:: Nie musisz kompilować Irohy, aby zacząć jej
  używać. Zamiast tego możesz pobrać gotowy obraz Dockera
  z Huba — proces ten jest szczegółowo opisany na
  stronie :ref:`getting-started` tej dokumentacji.

Wymagania wstępne
=================

Aby pomyślnie skompilować Irohę, musimy skonfigurować
środowisko. Istnieje kilka sposobów, aby to zrobić i
dopiszemy je wszystkie.

Obecnie obsługujemy systemy podobne do Unix (w
zasadzie celujemy w popularne dystrybucje Linuksa i
MacOS). Jeśli masz Windows lub nie chcesz spędzać czasu
na instalowaniu wszystkich zależności, możesz
rozważyć użycie środowiska Dockera. Użytkownicy Windows
mogą również rozważyć użycie
`WSL <https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux>`_.

Technicznie Iroha może być kompilowana pod Windows
natywnie w trybie eksperymentalnym. Ten przewodnik
obejmuje również ten sposób. Wszystkie etapy
związane z natywną kompilacją na Windows są oddzielone
od głównego przepływu ze względu na ich znaczne
różnice.

Wybierz preferowaną platformę poniżej, aby uzyskać
szybki dostęp:

    - :ref:`docker-pre`
    - :ref:`linux-pre`
    - :ref:`MacOS-pre`
    - :ref:`Windows-pre`


.. hint:: Masz problemy? Sprawdź sekcję FAQ lub
  skontaktuj się z nami bezpośrednio, jeśli utknąłeś
  gdzieś. Nie spodziewamy się, że to się stanie, ale
  niektóre problemy ze środowiskiem są możliwe.

.. _docker-pre:

Docker
^^^^^^

Ideą posiadania aktualnych obrazów Dockera jest
możliwość uruchomienia Irohy bez potrzeby jej
kompilowania. Ale teraz masz opcję uruchomienia nie
tylko gotowych do użycia obrazów, ale także specjalnego
**Iroha Builder** w Dockerze, aby skompilować Irohę
po swojemu.

Przede wszystkim musisz zainstalować ``docker`` i
``docker-compose``. Możesz przeczytać, jak to
zainstalować na `stronie Dockera
<https://www.docker.com/community-edition/>`_.

.. note:: Prosimy używać najnowszego dostępnego
  demona dockera i docker-compose.

Obrazy Iroha
""""""""""""

Wszystkie obrazy Dockera Irohy znajdziesz
`przeszukując Docker Hub
<https://hub.docker.com/search?q=hyperledger%2Firoha&type=image>`_
lub na `GitHubie
<https://github.com/orgs/hyperledger/packages?repo_name=iroha>`_.

Obecnie istnieją następujące obrazy:

- ``iroha`` — ogólna kompilacja Iroha 1.x (artefakt
  upstream na Docker Hub);
- ``iroha-burrow`` — kompilacja z `integracją
  Burrow <../integrations/index.html#hyperledger-burrow>`_
  w Iroha 1.x (przestarzała: Burrow jest EOL);
- ``iroha-ursa`` — kompilacja z `integracją
  Ursa <../integrations/index.html#hyperledger-ursa>`_
  w Iroha 1.x (przestarzała: Ursa jest EOL);
- ``iroha2`` — kompilacja Iroha 2;
- ``iroha-builder`` — specjalny obraz pozwalający
  uruchomić builder Iroha w Dockerze.

.. hint:: Więcej o uruchamianiu obrazów przeczytasz w
  `Przewodniku Szybkiego Startu
  <../getting_started/index.html>`_.

Każdy obraz może być używany z odpowiednim tagiem
wskazującym gałąź, z której obraz jest zbudowany.
Wszystkie dostępne tagi znajdziesz na Docker Hub.
`Oto wszystkie tagi <https://hub.docker.com/r/hyperledger/iroha/tags>`_
dla obrazu ``iroha``.

Na przykład ``iroha:1.6.0`` to ostatnia kompilacja
Iroha 1. **Nie** używaj tagów ``latest``, ``develop``
lub ``main`` — teraz wskazują na kompilacje Iroha 2
(Rust) lub nieaktualne artefakty z 2022.

Obrazy community-forka na GHCR
""""""""""""""""""""""""""""""

CI forka ``baziorek/iroha1-cpp`` publikuje dodatkowo
obrazy w **GitHub Container Registry** pod
``ghcr.io/baziorek/iroha1-cpp``. Obecnie publikowany
jest tylko obraz **builder** (zadania obrazu runtime
``docker/release`` są wyłączone w workflow), z tagami
dopasowanymi do nazw gałęzi — np. ``iroha1-main`` —
oraz tagów release:

.. code-block:: shell

  docker pull ghcr.io/baziorek/iroha1-cpp:iroha1-main

Używaj go dokładnie jak ``hyperledger/iroha-builder``
poniżej, ale pamiętaj, że jest zbudowany z bieżących
źródeł forka, więc jego toolchain odpowiada temu,
czego repozytorium potrzebuje dziś.


Iroha Builder
"""""""""""""

.. note:: Obraz ``hyperledger/iroha-builder`` jest
  artefaktem upstream, który nie był przebudowywany
  od czasu, gdy Iroha 1 została przestarzała upstream;
  jego toolchain może być zbyt stary dla obecnych
  źródeł. Preferuj kompilację natywną lub we własnym
  kontenerze — kroki poniżej zadziałają, jeśli obraz
  jest wystarczająco nowy.

Iroha builder pozwala kompilować Irohę z `dowolnymi
parametrami dostępnymi <#cmake-parameters>`_ dla
innej platformy, ale wygodnie i bezpiecznie w
Dockerze.

Oto kroki:

1. Przede wszystkim uruchommy builder:

.. code-block:: shell

  docker run -it hyperledger/iroha-builder:latest

W tym kroku uruchomisz obraz i uruchomisz go w
kontenerze w trybie interaktywnym. Możesz użyć
dowolnych dostępnych tagów, domyślnym jest ``latest``,
a tag developerski to ``develop``. Zauważ, że
możesz potrzebować wykonania niektórych akcji z
prawami ``sudo``.

2. Gdy jesteś wewnątrz kontenera, sklonuj repozytorium
   Iroha:

.. code-block:: shell

  git clone https://github.com/baziorek/iroha1-cpp.git

3. Gdy Iroha jest sklonowana, przejdź do folderu
   Iroha:

.. code-block:: shell

  cd iroha1-cpp

4. Następnie uruchom skrypt, który zbuduje wszystkie
   niezbędne zależności przez vcpkg:

.. code-block:: shell

  ./vcpkg/build_iroha_deps.sh $PWD/vcpkg-build



.. _linux-pre:

Linux
^^^^^

Aby skompilować Irohę, będziesz potrzebować
następujących pakietów:

``build-essential`` ``git`` ``ca-certificates`` ``tar``
``ninja-build`` ``curl`` ``unzip`` ``cmake``

Użyj tego kodu, aby zainstalować zależności
środowiska na dystrybucji Linuksa opartej na Debianie.

.. code-block:: shell

  apt-get update; \
  apt-get -y --no-install-recommends install \
  build-essential ninja-build \
  git ca-certificates tar curl unzip cmake \
  pkg-config zip

.. note:: Narzędzie testowe WSV-diff
  ``test/tool/test_tool_iroha_wsv_diff.sh`` wymaga
  dodatkowo ``jq`` i ``postgresql-client`` (dla ``psql``)
  na maszynie uruchamiającej testy.

.. Important:: Jeśli chcesz użyć `integracji
  Burrow <../integrations/burrow.html>`_, będziesz
  potrzebować także GO. Zainstaluj go zgodnie z
  instrukcjami na `oficjalnej stronie
  <https://golang.org/doc/install>`_ i użyj
  następującego polecenia:

.. code-block:: shell

  go get github.com/golang/protobuf/protoc-gen-go

.. note::  Jeśli planujesz aktywnie rozwijać Irohę i
  kompilować biblioteki współdzielone, rozważ
  zainstalowanie `najnowszej wersji
  <https://cmake.org/download/>`_ CMake.

RaspberryPi 4
"""""""""""""""""""""""""

Aby skompilować Irohę na Raspberry Pi 4, postępuj zgodnie
z tymi samymi instrukcjami co przy kompilacji Linuksa.

**UWAGA**: Iroha wymaga 8GiB RAM. Jeśli Twoja
kompilacja kończy się ``SIGKILL``, rozważ utworzenie
pliku swap lub partycji swap na urządzeniu hosta lub
kompilację krzyżową.

Teraz jesteś gotowy, aby `zainstalować zależności
Iroha <#installing-dependencies-with-vcpkg-dependency-manager>`_.

.. _macos-pre:

MacOS
^^^^^

Jeśli chcesz skompilować Irohę od zera i aktywnie ją
rozwijać, użyj poniższego kodu, aby zainstalować wszystkie
zależności środowiska za pomocą Homebrew:

.. code-block:: shell

  xcode-select --install
  brew install cmake go pkg-config coreutils ninja git gcc@9

.. hint:: Aby zainstalować sam Homebrew, uruchom

  ``ruby -e "$(curl -fsSL https://raw.githubusercontent.com/homebrew/install/master/install)"``

.. Important:: Jeśli chcesz użyć `integracji
  Burrow <../integrations/burrow.html>`_, będziesz
  potrzebować także GO. Zainstaluj go zgodnie z
  instrukcjami na `oficjalnej stronie
  <https://golang.org/doc/install>`_ i użyj
  następującego polecenia:

.. code-block:: shell

  go get github.com/golang/protobuf/protoc-gen-go

Teraz jesteś gotowy, aby `zainstalować zależności
Iroha <#installing-dependencies-with-vcpkg-dependency-manager>`_.

.. _windows-pre:

Windows
^^^^^^^

.. note:: Wszystkie wymienione polecenia są
  przeznaczone do kompilacji 64-bitowej wersji Irohy.

Menedżer pakietów Chocolatey
""""""""""""""""""""""""""""

Przede wszystkim potrzebujesz zainstalowanego
menedżera pakietów Chocolatey. Zapoznaj się z
`przewodnikiem <https://chocolatey.org/install>`_,
aby zainstalować chocolatey.

Kompilacja zestawu narzędzi
"""""""""""""""""""""""""""

Zainstaluj CMake, Git, kompilatory Microsoft przez
chocolatey, będąc w trybie administracyjnym wiersza
poleceń:

.. code-block:: shell

  choco install cmake git visualstudio2019-workload-vctools ninja


PostgreSQL nie jest zależnością kompilacji, ale
zalecamy zainstalowanie go teraz dla późniejszych
testów:

  .. code-block:: shell

    choco install postgresql
    # Nie zapomnij hasła, które ustawiłeś!

Teraz jesteś gotowy, aby `zainstalować zależności
Iroha <#installing-dependencies-with-vcpkg-dependency-manager>`_.

Instalacja zależności przez Vcpkg Dependency Manager
====================================================

Obecnie używamy Vcpkg jako menedżera zależności dla
wszystkich platform — Linux, Windows i MacOS.
Używamy ustalonej wersji Vcpkg (przypiętej w
``vcpkg/VCPKG_COMMIT_SHA``), aby upewnić się, że
potrzebne nam łatki będą działać.

Kompletna lista wymaganych zależności jest utrzymywana
w jednym miejscu — ``vcpkg/VCPKG_DEPS_LIST`` — i
odtworzona poniżej. Vcpkg rozwiązuje każdy port do
najnowszej wersji dostępnej w przypiętej linii
bazowej vcpkg; są to **minimalne obsługiwane wersje**
dla pomyślnej kompilacji:

.. literalinclude:: ../../../vcpkg/VCPKG_DEPS_LIST
   :language: text

.. note:: Jeśli kompilujesz poza vcpkg (np. pakiety
  systemowe), upewnij się, że Twoje wersje **nie są
  starsze** niż te dostarczane przez przypiętą linię
  bazową vcpkg. W szczególności kod wymaga **soci
  4.x** (używa ``soci/std-optional.h``), **protobuf
  z codegen gRPC**, **Boost >= 1.73** i **OpenSSL
  3.x**.

Ta stabilna wersja może być znaleziona tylko wewnątrz
repozytorium Iroha, więc będziemy musieli sklonować
Irohę. Cały proces jest dość podobny dla wszystkich
platform, ale dokładne polecenia są nieco inne.

Linux i MacOS
^^^^^^^^^^^^^

Uruchom w terminalu:

.. code-block:: shell

  git clone https://github.com/baziorek/iroha1-cpp.git
  cd iroha
  ./vcpkg/build_iroha_deps.sh $PWD/vcpkg-build

I to wszystko! Możesz teraz przejść do sekcji
`Kompilowanie Irohy <#build-process>`_.

Windows
^^^^^^^

Wykonaj z Power Shell:

.. code-block:: shell

  git clone https://github.com/baziorek/iroha1-cpp.git
  cd iroha
  powershell -ExecutionPolicy ByPass -File .\.packer\win\scripts\vcpkg.ps1 .\vcpkg .\iroha\vcpkg


Świetna robota! Możesz teraz przejść do sekcji
`Kompilowanie Irohy <#build-process>`_.

.. note:: Jeśli planujesz kompilację 32-bitowej
  wersji Irohy — będziesz musiał zainstalować wszystkie
  wymienione wyżej biblioteki poprzedzone terminem
  ``x86`` zamiast ``x64``.

Proces kompilacji
=================

Kompilowanie Irohy
^^^^^^^^^^^^^^^^^^

1. Więc po zbudowaniu zależności możemy zacząć
   kompilować samą Irohę:

.. code-block:: shell

  cmake -B build -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg-build/scripts/buildsystems/vcpkg.cmake . -DCMAKE_BUILD_TYPE=RELEASE   -GNinja -DUSE_BURROW=OFF -DUSE_LIBURSA=OFF -DTESTING=OFF -DPACKAGE_DEB=OFF

Parametry cmake takie jak ``-DUSE_BURROW=ON`` to
dokładnie parametry, które możesz wybrać dla swojej
bardzo specjalnej kompilacji. Pełną listę i opis tych
parametrów zobaczysz `tutaj <#cmake-parameters>`_.

2. Uruchom

.. code-block:: shell

  cmake --build ./build --target irohad

.. warning:: Jeśli chcesz później użyć testów,
  zamiast budować cel ``irohad``, musisz użyć tego:

.. code-block:: shell

  cmake --build ./build --target all 


3. Sprawdź wynik, uruchamiając help:

.. code-block:: shell

  ./build/bin/irohad --help

Ten krok pokaże Ci wszystkie parametry. I to
wszystko!

.. note:: Podczas kompilacji na Windows nie wykonuj
  tego z Power Shell. Lepiej użyj x64 Native tools
  command prompt.

Teraz Iroha jest skompilowana. Chociaż, jeśli chcesz,
możesz ją skompilować z dodatkowymi parametrami
dopisanymi poniżej.

Jeśli jesteś zadowolony z wyników, możesz przejść do
następnego kroku i `uruchomić instancję Irohy
<../deploy/single.html>`_.

Parametry CMake
^^^^^^^^^^^^^^^

Używamy CMake do generowania plików kompilacji
dependentnych od platformy. Ma liczne flagi do
konfigurowania finalnej kompilacji. Zauważ, że poza
wymienionymi parametrami przydatne mogą być również
zmienne cmake. Ponadto, dopóki ta strona może być
przestarzała (lub po prostu niekompletna), możesz
przeglądać niestandardowe flagi przez ``cmake -L``,
``cmake-gui`` lub ``ccmake``.

.. hint::  Możesz określić parametry na etapie
  konfigurowania cmake (np. cmake -DTESTING=ON).

Główne parametry
""""""""""""""""""

.. list-table::
   :header-rows: 1

   * - Parametr
     - Możliwe wartości
     - Domyślnie
     - Opis
   * - TESTING
     - ON/OFF
     - ON
     - Włącza lub wyłącza kompilację testów
   * - BENCHMARKING
     - ON/OFF
     - OFF
     - Włącza lub wyłącza kompilację biblioteki Google
       Benchmarks
   * - COVERAGE
     - ON/OFF
     - OFF
     - Włącza lub wyłącza ustawienie lcov do
       generowania pokrycia kodu
   * - USE_LIBURSA
     - ON/OFF
     - OFF
     - Włącza użycie kryptografii HL Ursa zamiast
       standardowej
   * - USE_BURROW
     - ON/OFF
     - OFF
     - Włącza integrację HL Burrow EVM

.. warning:: Opcje ``USE_LIBURSA`` i ``USE_BURROW`` są
  **przestarzałe**: Hyperledger Ursa osiągnęła End of
  Life 27 kwietnia 2023, a Hyperledger Burrow
  12 maja 2022. Obie integracje są zachowane tylko dla
  zgodności ze starszymi wdrożeniami.

.. note:: Jeśli chcesz użyć kryptografii HL Ursa dla
  swojej kompilacji, zainstaluj `Rust
  <https://www.rust-lang.org/tools/install>`_ dodatkowo
  do innych zależności. Więcej o integracji HL Ursa
  `tutaj <../integrations/index.html#hyperledger-ursa>`_.

  Jeśli chcesz użyć integracji HL Burrow, nie
  zapomnij najpierw zainstalować `Go
  <https://golang.org/doc/install>`_, a następnie
  `protoc-gen-go
  <https://developers.google.com/protocol-buffers/docs/reference/go-generated>`_.
  Więcej o `integracji HL Burrow
  <../integrations/index.html#hyperledger-burrow>`_.

Parametry specyficzne dla pakietowania
""""""""""""""""""""""""""""""""""""""

.. list-table::
   :header-rows: 1

   * - Parametr
     - Możliwe wartości
     - Domyślnie
     - Opis
   * - PACKAGE_ZIP
     - ON/OFF
     - OFF
     - Włącza lub wyłącza pakowanie zip
   * - PACKAGE_TGZ
     - ON/OFF
     - OFF
     - Włącza lub wyłącza pakowanie tar.gz
   * - PACKAGE_RPM
     - ON/OFF
     - OFF
     - Włącza lub wyłącza pakowanie rpm
   * - PACKAGE_DEB
     - ON/OFF
     - OFF
     - Włącza lub wyłącza pakowanie deb

Uruchamianie testów (opcjonalnie)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Przede wszystkim upewnij się, że `skompilowałeś Irohę
poprawnie <#build-process>`_ dla testów.

Po skompilowaniu Irohy dobrym pomysłem jest uruchomienie
testów, aby sprawdzić działanie demonu. Możesz
uruchomić testy tym kodem:

.. code-block:: shell

  cmake --build build --target test

Alternatywnie możesz uruchomić następujące polecenie
w folderze ``build``:

.. code-block:: shell

  cd build
  ctest . --output-on-failure

.. note:: Niektóre testy zawiodą bez uruchomionego
  magazynu PostgreSQL, więc jeśli nie używasz skryptu
  ``scripts/run-iroha-dev.sh``, uruchom kontener
  Dockera lub utwórz lokalne połączenie z następującymi
  parametrami:

  .. code-block:: shell

    docker run --name some-postgres \
    -e POSTGRES_USER=postgres \
    -e POSTGRES_PASSWORD=mysecretpassword \
    -p 5432:5432 \
    -d postgres:9.5 \
    -c 'max_prepared_transactions=100'
