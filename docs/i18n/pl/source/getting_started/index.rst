.. _getting-started:

=============================
Przewodnik szybkiego startu
=============================

Aby korzystać z Hyperledger Iroha potrzebujesz dwóch rzeczy:

#. **Co najmniej jednego węzła Iroha** — działającego procesu ``irohad``,
   który tworzy (lub dołącza do) sieć.
#. **Sposobu wysyłania transakcji** — klienta komunikującego się z węzłem
   przez gRPC: narzędzia ``iroha-cli``, jednej z bibliotek klienta albo
   własnego kodu.

Ten przewodnik przeprowadzi Cię przez oba elementy. Najpierw wybierz,
jak zdobyć węzeł Iroha, a potem jak się z nim komunikować.

.. note:: Ledger to synonim blockchaina, a Hyperledger Iroha jest znana
  również jako framework Distributed Ledger Technology — co w istocie
  jest tym samym co "framework blockchain". Pozostałą terminologię
  znajdziesz w sekcji :ref:`core-concepts`.

.. toctree::
   :maxdepth: 2
   :caption: Zdobądź węzeł Iroha

   node-docker.rst
   node-appimage.rst
   node-deb.rst
   node-build.rst

.. toctree::
   :maxdepth: 2
   :caption: Narzędzie komunikacji — ``iroha-cli``

   cli.rst

.. toctree::
   :maxdepth: 2
   :caption: Biblioteki klienta

   cli-guide.rst
   python-guide.rst

.. toctree::
   :maxdepth: 2
   :caption: Konfiguracja węzła

   configuration.rst
