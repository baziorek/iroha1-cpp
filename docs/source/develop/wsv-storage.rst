.. _wsv-storage:

=====================
WSV storage layout
=====================

Iroha can store the `World State View (WSV) <../concepts_architecture/glossary.html#wsv>`__
in two interchangeable backends: **PostgreSQL** (the default) and
**RocksDB** (mainly used for single-node deployments and testing).
They store exactly the same logical data, but in completely different
physical layouts — there are no "keys" in Postgres at all, while
RocksDB is a single flat key-value store in which every record is
addressed by a hand-crafted string key.

This page explains how to read both layouts, e.g. when inspecting the
database with ``ldb``/``psql``.

PostgreSQL backend
==================

Postgres uses a normalized relational schema — one table per entity
kind, joined by foreign keys. There is no key encoding: records are
found by SQL queries built in
``irohad/ametsuchi/impl/postgres_wsv_command.cpp`` and
``postgres_wsv_query.cpp``.

Main tables (created in ``irohad/main/impl/pg_connection_init.cpp``):

.. csv-table::
    :header: "Table", "Content"
    :widths: 35, 65

    "``domain``", "domains and their default roles"
    "``asset``", "assets and their precision"
    "``account``", "accounts"
    "``account_has_asset``", "asset balances per account"
    "``account_has_signatory``", "signatories per account"
    "``account_has_roles``", "roles attached to accounts"
    "``account_has_grantable_permissions``", "permissions an account may grant"
    "``role`` / ``role_has_permissions``", "roles and their permission bitfields"
    "``signatory``", "public keys of signatories"
    "``peer`` / ``sync_peer``", "validating and syncing peers"
    "``tx_positions`` / ``tx_status_by_hash``", "transaction positions and statuses"
    "``setting``", "node settings (e.g. description size limit)"
    "``engine_calls`` / ``burrow_*``", "CallEngine receipts and Burrow (EVM) storage"

RocksDB backend
===============

RocksDB keeps everything in one LSM-tree. Every record key is a
``/``-delimited string built from one-letter *mnemonic* segments, so
that a whole subtree can be scanned with a single prefix range scan.
The layout is defined once in
``irohad/ametsuchi/impl/rocksdb_common.hpp`` — its comment block
contains the full directory tree, the legend of mnemonics, and worked
examples; the format strings themselves live in the ``fmtstrings``
namespace of the same header and are filled by ``dbCall`` in
``rocksdb_common.cpp``.

Key anatomy
-----------

A key is a path of segments separated by ``/``::

    wD/<domain_id>/a/<account_name>/x/<asset_id>

Reading it with the legend map from ``rocksdb_common.hpp``:

.. csv-table::
    :header: "Segment", "Meaning"
    :widths: 15, 85

    "``w``", "WSV root (all world-state keys start with it)"
    "``D``", "domain directory"
    "``a``", "accounts directory inside a domain"
    "``x``", "assets directory inside an account"
    "``<asset_id>``", "the asset itself — the value is the balance"

More examples from the header:

- ``wta/<account>/T/<timestamp>/`` — transactions of an account by timestamp
  (``GetAccountTransactions``)
- ``wD/<domain>/a/<account>/x`` — asset balances of an account
  (``GetAccountAssets``)
- ``wr/<role_name>`` — role → permissions bitfield
- ``wu/<tx_hash>`` — transaction status by hash
- ``wn/M/<pubkey>`` — peer address by public key

Two structural levels matter when reading raw keys:

- **Directories** end with ``/`` and group records; iterating a prefix
  returns everything below it (e.g. all accounts of a domain).
- **Fields/files** are leaf keys; some carry a value, others act as
  markers (e.g. ``.../S/<pubkey>`` marks a signatory — the value is
  empty).

Counters and singletons use dedicated file-level keys such as
``.../O/q`` (account quorum), ``.../O/V`` (transaction count) and
``.../Z`` (peer count) — see the legend in the header for the full
list.

.. note::
    The complete mnemonic legend (``s`` store, ``w`` WSV, ``n`` network,
    ``t`` transactions, ``D`` domain, ``a`` accounts, ``x`` assets,
    ``r`` roles, ``S`` signatories, ``d`` details, ``g`` grantable
    permissions, ``O`` options, ``P``/``T`` position/timestamp,
    ``e``/``E``/``W``/``R``/``y``/``Y``/``A`` engine-call storage, …)
    is documented in the comment block at the top of
    ``irohad/ametsuchi/impl/rocksdb_common.hpp`` — consult it as the
    authoritative reference when decoding keys.

Do Postgres and RocksDB generate keys differently?
==================================================

Yes — fundamentally. Postgres has no string keys at all: identity is
given by table rows and columns, and all access goes through parameterized
SQL in ``postgres_wsv_command.cpp`` / ``postgres_wsv_query.cpp``.
RocksDB instead encodes the same entities into hierarchical string
paths via the ``fmtstrings`` templates in ``rocksdb_common.hpp``,
executed through the ``dbCall`` helper with ``kMustExist`` /
``kMustNotExist`` existence checks.

Both backends implement the same ``CommandExecutor`` /
``QueryExecutor`` interfaces, so the choice is transparent to the rest
of the node — only the on-disk/on-wire inspection differs.

Related
-------

- `RocksDB ↔ Postgres migration guide <../maintenance/migration-rocksdb.html>`__
- `Glossary: WSV <../concepts_architecture/glossary.html#wsv>`__
- Source of truth for key layout:
  ``irohad/ametsuchi/impl/rocksdb_common.hpp``
