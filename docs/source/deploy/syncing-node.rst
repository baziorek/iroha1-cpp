.. _syncing-node:

=================
Syncing node
=================

A **syncing node** is an Iroha peer that follows the chain but does not
participate in consensus: it does not create proposals, does not vote and
does not count towards the quorum. Its role is to keep a local copy of the
world state view (WSV) quickly — for example to serve queries, to act as a
read replica, or to seed a new full node — without the cost of running the
full consensus machinery.

Syncing peers are declared in the network just like regular peers: the
``Peer`` message carries a ``syncing_peer`` flag, which is set through the
`Add Peer <../develop/api/commands.html#add-peer>`_ command (or in the genesis block). The account
sending the command needs the ``can_add_peer`` permission, exactly as for a
regular peer.

Running the node
----------------

Start ``irohad`` with the ``--syncing_node`` flag:

.. code-block:: shell

    irohad --config example/config.sample \
           --genesis_block example/genesis.block \
           --keypair_name example/node0 \
           --syncing_node

The flag maps to ``config.syncing_mode`` and is consumed by the ordering
and consensus initialization — the node skips YAC voting and instead pulls
committed blocks from its peers.

.. note:: A syncing node still needs a valid configuration, keypair and
  database — it stores blocks and the WSV like any other peer. It simply
  does not produce or vote on them.

Useful related flags
--------------------

- ``--exit_after_init`` — rebuilds the WSV from the block store and exits.
  Handy for reindexing after maintenance or when switching storage
  backends; see :ref:`deploy-flags`.
- ``--wait_for_new_blocks`` — startup policy that waits for new blocks in
  the local block store without running the network.

See also
--------

- :doc:`../concepts_architecture/glossary` — *syncing peer* entry under **Peer**
- :ref:`yac` — how regular (validating) peers reach consensus
- :doc:`../configure/index` — ``healthcheck_port`` exposes ``is_syncing``
  in the node status response
