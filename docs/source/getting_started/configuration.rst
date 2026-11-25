.. _node-configuration:

======================
Node configuration
======================

Every ``irohad`` instance is configured by a single JSON file passed
with the ``--config`` flag (together with ``--genesis_block`` and
``--keypair_name`` on first start — see :ref:`node-appimage`).

The repository ships ready-made examples you can copy and adapt:

- ``example/config.sample`` — generic setup (PostgreSQL on localhost)
- ``example/config.docker`` — used by the Docker image
- ``example/config.postgres.sample`` — explicit PostgreSQL variant
- ``example/config-win.sample`` — Windows-oriented variant

.. note:: This guide uses the community-maintained fork
  `baziorek/iroha1-cpp <https://github.com/baziorek/iroha1-cpp>`_.

Key settings
------------

- ``database`` — storage backend: ``postgres`` (host, port, credentials,
  working/maintenance databases) or RocksDB via ``block_store_path``
- ``torii_port`` — client-facing gRPC port (default ``50051``)
- ``internal_port`` — inter-peer gRPC port (default ``10001``)
- ``max_proposal_size`` — maximum transactions per block/proposal
- ``vote_delay`` / ``proposal_creation_timeout`` — consensus timing
- ``mst_enable`` / ``mst_expiration_time`` — multisignature transaction
  support (see :ref:`architecture`)
- ``metrics`` / ``healthcheck_port`` — observability endpoints

Full parameter reference
------------------------

Every configuration parameter is documented in detail — types,
defaults, units and side effects — in the
:ref:`configuration` section. In particular see the
`environment-specific parameters <../configure/index.html#environment-specific-parameters>`_
part for tuning guidance.

.. hint:: For a multi-node network each peer needs its own config file,
  keypair and genesis block listing all peers — the
  :ref:`deploy-guide` covers that setup.
