.. _iroha-cli:

======================
``iroha-cli``
======================

``iroha-cli`` is Iroha's command-line client: an interactive tool for
sending transactions and queries to a peer without writing any code.
It ships together with ``irohad`` in every distribution — pick the one
you already have.

Getting ``iroha-cli``
---------------------

Docker
^^^^^^

If you run the node from the official image, ``iroha-cli`` is already
inside the container:

.. code-block:: shell

  docker exec -it iroha iroha-cli -account_name admin@test

The ``-account_name`` argument selects which account's keypair signs
the transactions — in the example configuration that is
``admin@test`` (the key lives next to the other files in
``example/``).

AppImage
^^^^^^^^

Download ``iroha-cli-x86_64.AppImage`` from the
`GitHub Releases page <https://github.com/baziorek/iroha1-cpp/releases>`__
alongside ``irohad``, mark it executable and run it:

.. code-block:: shell

  chmod +x iroha-cli-x86_64.AppImage
  ./iroha-cli-x86_64.AppImage -account_name admin@test

.. note:: The ``iroha-cli`` AppImage is published with the ``1.6.1``
  release — see :ref:`node-appimage`.

Build from source
^^^^^^^^^^^^^^^^^

When you :ref:`build Iroha from source <node-build>`, ``iroha-cli`` is
produced next to ``irohad``:

.. code-block:: shell

  ./build/bin/iroha-cli -account_name admin@test

First run
---------

Whatever the distribution, the first launch asks for the peer address
and port — for a local node that is ``localhost`` and ``50051`` (the
``torii_port`` from the node configuration). From there the interactive
menu lets you compose transactions (``tx``) and queries (``qry``),
send them to the peer and check transaction statuses.

.. note:: ``iroha-cli`` covers only the most common commands — it is a
  convenience tool, not the full API. For a complete walkthrough of
  sending your first transaction see :doc:`cli-guide`; for programmatic
  access see the :doc:`python-guide` and the
  `API reference <../develop/api/commands.html>`_.
