.. _node-build:

======================================
Iroha node built from source
======================================

Building Iroha from source gives you ``irohad`` (the node) and
``iroha-cli`` (the command-line client) as regular binaries — the same
way the release packages are produced, but with full control over
compiler flags, patches and configuration.

The complete, platform-specific build guide — prerequisites, vcpkg
dependency build, CMake options and troubleshooting — lives in the
:ref:`build-guide` section. In short:

#. Install the toolchain prerequisites for your platform
   (Linux/macOS, or Windows via WSL).
#. Build the dependencies once with ``./vcpkg/build_iroha_deps.sh``.
#. Configure and build:

   .. code-block:: shell

     cmake -S . -B build \
         -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg-build/scripts/buildsystems/vcpkg.cmake \
         -DCMAKE_BUILD_TYPE=Release
     cmake --build build --target irohad iroha-cli -j$(nproc)

The resulting binaries land in ``build/bin/`` (``irohad``,
``iroha-cli``).

Running the node
----------------

A self-built ``irohad`` takes the same command-line flags as the
AppImage build — see :ref:`node-appimage` for a complete first-start
example. You will also need the example configuration files from
``example/`` (config, genesis block, keypairs).

.. hint:: If you only need a working node quickly, the
  :ref:`Docker <node-docker>` or :ref:`AppImage <node-appimage>`
  options are faster — building from source is mainly for developers
  who plan to modify Iroha itself.
