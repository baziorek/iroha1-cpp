.. _node-deb:

=================================
Iroha node from release packages
=================================

Besides Docker images, AppImages and source builds, the Iroha 1 release
pipeline attaches native packages to `GitHub Releases
<https://github.com/baziorek/iroha1-cpp/releases>`__ of the community
fork:

- ``iroha_*.deb`` — Debian/Ubuntu package built with CPack; contains
  ``irohad`` and the maintenance tools (migration tool, WSV checker)
- ``iroha-*.rpm`` — the same for RPM-based distributions, when published
- ``irohad-x86_64.AppImage`` / ``iroha-cli-x86_64.AppImage`` — see
  :doc:`node-appimage`

.. note:: Packages appear on the Releases page starting with the ``1.6.1``
  tag. Until then, use the :ref:`Docker <node-docker>` or
  :ref:`build-from-source <node-build>` options.

Installing the .deb package
---------------------------

Download the ``.deb`` file from the Releases page, then install it with
``apt`` (which also resolves dependencies):

.. code-block:: shell

  sudo apt install ./iroha_*.deb

or with ``dpkg`` directly:

.. code-block:: shell

  sudo dpkg -i iroha_*.deb
  # if apt reports missing dependencies:
  sudo apt -f install

The package installs the binaries system-wide (CPack default prefix
``/usr``). Verify with:

.. code-block:: shell

  which irohad
  irohad --help

Installing the .rpm package
---------------------------

On RPM-based distributions (Fedora, RHEL, openSUSE):

.. code-block:: shell

  sudo dnf install ./iroha-*.rpm
  # or: sudo rpm -i iroha-*.rpm && sudo dnf -f install

Running the node
----------------

A packaged ``irohad`` takes the same arguments as any other build —
configuration file, genesis block and keypair name:

.. code-block:: shell

  sudo irohad --config /etc/iroha/config.json \
              --genesis_block /etc/iroha/genesis.block \
              --keypair_name /etc/iroha/node0

There is no bundled configuration — copy the examples from the
repository's ``example/`` directory (see :doc:`configuration`) and
adjust them to your environment, including the ``database`` section
(PostgreSQL or RocksDB) which must be reachable before the node starts.

.. note:: The packaged node still needs PostgreSQL (or RocksDB) and,
  for a production deployment, TLS termination in front of Torii —
  see :doc:`../configure/torii-tls`.
