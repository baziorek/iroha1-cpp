.. _node-build:

======================================
Kaynak koddan derlenen Iroha düğümü
======================================

Kaynak koddan derlenen Iroha, sürüm paketlerinin ürettiği gibi normal
çalıştırılabilir dosyalar olarak ``irohad`` (düğüm) ve ``iroha-cli``
(komut satırı istemcisi) üretir — ancak derleyici bayrakları,
yamalar ve yapılandırma üzerinde tam kontrolle.

Platforma özgü eksiksiz derleme kılavuzu — gereksinimler, vcpkg
dependencies derlemesi, CMake seçenekleri ve sorun giderme —
:ref:`build-guide` bölümündedir. Özetle:

#. Platformunuz (Linux/macOS veya WSL üzerinden Windows) için
   toolchain gereksinimlerini karşılayın.
#. ``./vcpkg/build_iroha_deps.sh`` ile bağımlılıkları bir kez derleyin.
#. Yapılandırın ve derleyin:

   .. code-block:: shell

     cmake -S . -B build \
         -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg-build/scripts/buildsystems/vcpkg.cmake \
         -DCMAKE_BUILD_TYPE=Release
     cmake --build build --target irohad iroha-cli -j$(nproc)

Hazır ikililer ``build/bin/`` dizinine yazılır (``irohad``,
``iroha-cli``).

Düğümü çalıştırma
-----------------

Kendiniz derlediğiniz ``irohad``, AppImage derlemesiyle aynı komut
satırı bayraklarını kabul eder — ilk çalıştırmanın tam örneği için
:ref:`node-appimage` sayfasına bakın. Ayrıca ``example/`` dizinindeki
örnek yapılandırmaya (config, genesis block, anahtar çifti) de
ihtiyacınız olacak.

.. hint:: Çalışan bir düğüm gerekiyorsa, :ref:`Docker <node-docker>`
  veya :ref:`AppImage <node-appimage>` seçenekleri daha hızlıdır —
  kaynak koddan derleme esas olarak Iroha'nın kendisini değiştirmeyi
  planlayan geliştiriciler içindir.
