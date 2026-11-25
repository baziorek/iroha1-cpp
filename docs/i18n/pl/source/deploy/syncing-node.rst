.. _syncing-node:

=====================
Węzeł synchronizujący
=====================

**Węzeł synchronizujący** (syncing node) to peer Irohy, który podąża za
łańcuchem, ale nie uczestniczy w konsensusie: nie tworzy propozycji, nie
głosuje i nie liczy się do kworum. Jego rolą jest szybkie utrzymywanie
lokalnej kopii world state view (WSV) — na przykład do obsługi zapytań,
jako replika odczytu albo do zasilenia nowego pełnego węzła — bez
kosztu pełnej maszyny konsensusu.

Syncing peery deklaruje się w sieci tak jak zwykłe peery: komunikat
``Peer`` niesie flagę ``syncing_peer``, ustawianą przez komendę
`Add Peer <../develop/api/commands.html#add-peer>`_ (albo w genesis
blocku). Konto wysyłające komendę potrzebuje uprawnienia
``can_add_peer``, dokładnie tak jak dla zwykłego peera.

Uruchomienie węzła
------------------

Uruchom ``irohad`` z flagą ``--syncing_node``:

.. code-block:: shell

    irohad --config example/config.sample \
           --genesis_block example/genesis.block \
           --keypair_name example/node0 \
           --syncing_node

Flaga mapuje się na ``config.syncing_mode`` i jest wykorzystywana przy
inicjalizacji ordering i konsensusu — węzeł pomija głosowanie YAC i
zamiast tego pobiera zatwierdzone bloki od peerów.

.. note:: Węzeł synchronizujący wciąż potrzebuje prawidłowej
  konfiguracji, pary kluczy i bazy danych — przechowuje bloki i WSV jak
  każdy inny peer. Po prostu ich nie produkuje ani nie głosuje.

Przydatne powiązane flagi
-------------------------

- ``--exit_after_init`` — przebudowuje WSV z block store i kończy
  działanie. Przydatne przy reindeksacji po maintenance albo przy
  zmianie backendu storage; zobacz :ref:`deploy-flags`.
- ``--wait_for_new_blocks`` — polityka startowa czekająca na nowe bloki
  w lokalnym block store bez uruchamiania sieci.

Zobacz też
----------

- :doc:`../concepts_architecture/glossary` — wpis *syncing peer* w
  **Peer**
- :ref:`yac` — jak zwykłe (walidujące) peery osiągają konsensus
- :doc:`../configure/index` — ``healthcheck_port`` udostępnia
  ``is_syncing`` w odpowiedzi statusu węzła
