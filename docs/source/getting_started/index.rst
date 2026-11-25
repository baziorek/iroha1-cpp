.. _getting-started:

=================
Quick Start Guide
=================

To use Hyperledger Iroha you need two things:

#. **At least one Iroha node** — a running ``irohad`` process that forms
   (or joins) the network.
#. **A way to send transactions** — a client that talks to the node over
   gRPC: the ``iroha-cli`` command-line tool, one of the client
   libraries, or your own code.

This guide walks you through both. First pick how to obtain an Iroha
node, then how to talk to it.

.. note:: Ledger is the synonym for a blockchain, and Hyperledger Iroha is known
  also as Distributed Ledger Technology framework — which in essence is the same as "blockchain framework".
  You can check the rest of terminology used in the :ref:`core-concepts` section.

.. toctree::
   :maxdepth: 2
   :caption: Get an Iroha node

   node-docker.rst
   node-appimage.rst
   node-deb.rst
   node-build.rst

.. toctree::
   :maxdepth: 2
   :caption: Communication tool — ``iroha-cli``

   cli.rst

.. toctree::
   :maxdepth: 2
   :caption: Client libraries

   cli-guide.rst
   python-guide.rst

.. toctree::
   :maxdepth: 2
   :caption: Node configuration

   configuration.rst
