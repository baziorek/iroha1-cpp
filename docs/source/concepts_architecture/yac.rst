.. _yac:

======================
Consensus (YAC)
======================

YAC — *Yet Another Consensus* — is the consensus algorithm used by Iroha 1
to agree on the ordered list of transactions (blocks) across all peers.
It is a leader-based, vote-driven protocol with synchronous rounds:
each round produces either a committed block, an empty ("nothing to
commit") result, or a rejection that advances the round.

The algorithm is described in the paper
`YAC: BFT Consensus Algorithm for Blockchain <https://arxiv.org/abs/1809.00554>`_
and in `this video <https://youtu.be/mzuAbalxOKo>`_.
The C++ implementation lives in ``irohad/consensus/yac/``.

Consistency models
------------------

YAC supports two consistency models, implemented as *supermajority
checkers* (``irohad/consensus/yac/supermajority_checker.hpp``):

- **CFT** (*Crash Fault Tolerant*) — requires a simple majority of
  votes. Selected by ``ConsistencyModel::kCft``.
- **BFT** (*Byzantine Fault Tolerant*) — requires a ~2/3 supermajority
  of votes, tolerating peers that send arbitrary (Byzantine) messages.
  Selected by ``ConsistencyModel::kBft``.

Both models use the ``Kf+1`` formula: a supermajority is reached when
``number * K >= (K - 1) * (all - 1) + K``, where ``all`` is the number
of peers in the round and ``K`` is 2 for CFT and 3 for BFT.

.. csv-table::
    :header: "Peers (N)", "CFT votes needed", "BFT votes needed"
    :widths: 20, 25, 25

    "1", "1", "1"
    "2", "2", "2"
    "3", "2", "3"
    "4", "3", "3"
    "5", "3", "4"
    "7", "4", "5"
    "10", "6", "7"
    "13", "7", "9"
    "16", "9", "11"
    "31", "16", "21"

.. note::
    Iroha 1 currently uses the **CFT** model — it is a compile-time
    constant (``kConsensusConsistencyModel`` in
    ``irohad/main/application.cpp``, switched to CFT in 2019). The BFT
    checker remains in the codebase but is not selectable via
    configuration.

Peers and ordering
------------------

Every peer has a stable identity (its ``Peer`` record: address, public
key, optional TLS certificate) maintained in the world state view.

Peers registered as **syncing peers** are kept in a separate ``sync_peer``
table: they do not join the cluster ordering, do not propose or vote, but
receive the consensus state directly so they can follow the ledger —
useful for backup and audit nodes.

For each round, peers are arranged into a *cluster ordering* — a
sequence derived from the ledger peer list in which every peer appears
exactly once. The **leader** of a round is the first peer in that
ordering; if a round is rejected, the ordering rotates and the next
peer becomes the leader (``ClusterOrdering::switchToNext``). This
guarantees that consecutive reject rounds try different peers as
leaders, so a single faulty or unreachable leader cannot stall the
chain.

Rounds
------

A consensus ``Round`` is a pair ``(block_round, reject_round)``:

- ``block_round`` — the ledger height being decided; it equals
  ``top_block height + 1``. It increments only when a block (or an
  empty commit) is committed.
- ``reject_round`` — a counter *within* the same ``block_round``. It
  increments every time a round is rejected, so that peers do not mix
  votes from different attempts at the same height.

Vote hash
---------

Each peer votes for a ``YacHash`` which binds three things together:

- the ``vote_round`` (so votes cannot be replayed across rounds),
- the ``proposal_hash`` — hash of the proposal (ordered transaction
  set) the peer saw,
- the ``block_hash`` — hash of the block the ``Simulator`` built from
  the verified proposal.

The vote itself is a ``VoteMessage``: the ``YacHash`` plus the peer's
signature over it. Because the vote covers both the proposal and the
block, a peer cannot be tricked into endorsing a block that does not
correspond to the proposal it approved.

Protocol flow
-------------

1. **Proposal.** The ordering service collects pending transactions
   (up to ``max_proposal_size``) and forms a proposal for the current
   ``block_round``. Peers that fall behind are caught up by the
   ``Synchronizer`` instead of participating.
2. **Block creation.** The ``Simulator`` runs stateless and stateful
   validation on the proposal's transactions and produces a candidate
   block (``BlockCreator`` + ``VerifiedProposalCreator``).
3. **Vote.** Every peer that agrees with the round data waits
   ``vote_delay`` milliseconds (to let slow peers respond) and then
   broadcasts its ``VoteMessage`` for the ``YacHash``.
4. **Outcome.** Votes are gossiped between peers. For each round a peer
   tracks vote groups per ``YacHash`` and applies the supermajority
   checker:

   - **Commit** — one ``YacHash`` collected a supermajority. All
     peers that voted for it converge on the same block; the collected
     votes become the block's signatures. Peers that voted for a
     different hash in the same round still commit the winner — the
     commit message carries the winning votes as proof.
   - **Commit on "nothing"** — a supermajority voted for an empty
     round (no transactions), producing no block but still advancing
     ``block_round``.
   - **Reject** — votes are split such that *no* option can still
     reach a supermajority (``canHaveSupermajority`` fails). The round
     is rejected, ``reject_round`` increments, the leader rotates, and
     a new attempt starts.
   - **Future** — a peer receives votes for a round greater than its
     current one; it syncs its round state forward.

5. **Commit handling.** On ``CommitMessage`` the ``HashGate`` verifies
   the vote set (signatures, round, hashes), the peer appends the block
   to the ledger via ``Ametsuchi`` and stores the gathered signatures
   on it. Blocks whose votes cannot be verified are ignored; missing
   blocks are later downloaded by the ``Synchronizer``.

Failure handling
----------------

- A crashed or partitioned peer simply stops voting; the remaining
  peers proceed as long as the supermajority threshold is still
  reachable (see the table above).
- A leader that does not produce a usable proposal causes reject
  rounds; the rotating ordering moves leadership to the next peer.
- If peers disagree about the ledger state (e.g. after a partition),
  votes for different ``block_hash`` values split the electorate until
  one option becomes unreachable — the round is rejected and retried.
- Committed blocks are final: once a supermajority has voted, the
  outcome is deterministic and identical on all honest peers.

Configuration knobs
-------------------

YAC behaviour is tuned through the node configuration
(see :ref:`configuration`):

- ``vote_delay`` — how long a peer waits before voting, giving slow
  peers time to respond. Too small a value slows block production on
  networks with unreachable peers.
- ``proposal_creation_timeout`` — the maximum time the ordering service
  waits while assembling a proposal (transactions accumulating up to
  ``max_proposal_size``) before answering a proposal request; default
  3000 ms. It was previously named ``max_rounds_delay``. A second,
  derived timeout — the network deadline for ``RequestProposal`` RPCs —
  is computed automatically as ``2 × proposal_creation_timeout`` and is
  not separately configurable.
- ``max_proposal_size`` — maximum number of transactions per proposal,
  i.e. the maximum block size.

.. note::
    ``stale_stream_max_rounds`` affects only the client-facing status
    stream, not consensus itself.

Related
-------

- `Consensus in architecture <architecture.html#block-consensus-yac>`__
- `Consensus glossary entry <glossary.html#consensus>`__
- `Configuration <../configure/index.html>`__
- Paper: `YAC: BFT Consensus Algorithm for Blockchain <https://arxiv.org/abs/1809.00554>`__
