.. _node-build:

======================================
Węzeł Iroha budowany ze źródeł
======================================

Budowanie Irohy ze źródeł daje ``irohad`` (węzeł) i ``iroha-cli``
(klient wiersza poleceń) jako zwykłe binaria — tak samo jak powstają
pakiety release'owe, ale z pełną kontrolą nad flagami kompilatora,
patchami i konfiguracją.

Kompletny, zależny od platformy przewodnik budowania — wymagania
wstępne, budowa zależności vcpkg, opcje CMake i rozwiązywanie problemów —
znajduje się w sekcji :ref:`build-guide`. W skrócie:

#. Zainstaluj wymagania toolchaina dla swojej platformy
   (Linux/macOS albo Windows przez WSL).
#. Zbuduj zależności raz przez ``./vcpkg/build_iroha_deps.sh``.
#. Skonfiguruj i zbuduj:

   .. code-block:: shell

     cmake -S . -B build \
         -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg-build/scripts/buildsystems/vcpkg.cmake \
         -DCMAKE_BUILD_TYPE=Release
     cmake --build build --target irohad iroha-cli -j$(nproc)

Powstałe binaria lądują w ``build/bin/`` (``irohad``, ``iroha-cli``).

Uruchomienie węzła
------------------

Samodzielnie zbudowany ``irohad`` przyjmuje te same flagi wiersza
poleceń co build AppImage — pełny przykład pierwszego startu znajdziesz
w :ref:`node-appimage`. Potrzebne będą też przykładowe pliki
konfiguracji z ``example/`` (config, genesis block, klucze).

.. hint:: Jeśli potrzebujesz tylko szybko działającego węzła, opcje
  :ref:`Docker <node-docker>` lub :ref:`AppImage <node-appimage>` są
  szybsze — budowanie ze źródeł jest głównie dla deweloperów planujących
  modyfikować samą Irohę.
