.. _node-appimage:

============================
Iroha node with AppImage
============================

AppImages are self-contained executables — no installation or dependency
setup required. The Iroha 1 release pipeline produces two of them:

- ``irohad-x86_64.AppImage`` — the node itself
- ``iroha-cli-x86_64.AppImage`` — the command-line client
  (see :doc:`cli`)

.. note:: AppImages are attached to `GitHub Releases
  <https://github.com/baziorek/iroha1-cpp/releases>`__ of the
  community fork starting with the ``1.6.1`` tag. Until a release
  containing them is published, use the :ref:`Docker <node-docker>` or
  :ref:`build-from-source <node-build>` options instead.

Downloading
-----------

Download the AppImage for your platform from the
`Releases page <https://github.com/baziorek/iroha1-cpp/releases>`__.

Running the node
----------------

AppImages are not executable by default — mark the file as executable
first:

.. code-block:: shell

  chmod +x irohad-x86_64.AppImage

The node needs the same inputs as any other ``irohad`` build: a JSON
configuration file, a genesis block and the node keypair. The
repository ships ready-made examples under ``example/``
(``config.sample``, ``genesis.block``, ``node0.priv``/``node0.pub``) —
clone the fork or copy that directory:

.. code-block:: shell

  git clone -b iroha1-main https://github.com/baziorek/iroha1-cpp --depth=1

For the first start use ``--overwrite_ledger`` so the genesis block is
applied to the empty storage:

.. code-block:: shell

  ./irohad-x86_64.AppImage \
    --config=iroha1-cpp/example/config.sample \
    --genesis_block=iroha1-cpp/example/genesis.block \
    --keypair_name=iroha1-cpp/example/node0 \
    --overwrite_ledger

.. note:: Adjust ``config.sample`` to your environment first — in
  particular the ``database`` section (PostgreSQL host/credentials or
  RocksDB) and ``torii_port``. See :doc:`configuration` for the full
  parameter reference.

On subsequent starts you can omit ``--genesis_block`` and
``--overwrite_ledger`` — the node restores its state from the block
store.

.. note:: ``config.sample`` in the repository uses PostgreSQL. To run
  with the embedded RocksDB storage instead, point ``block_store_path``
  at a writable directory and remove/adjust the ``database`` section —
  see :ref:`configuration` for details.
