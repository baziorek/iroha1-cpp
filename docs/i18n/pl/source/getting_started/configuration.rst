.. _node-configuration:

======================
Konfiguracja węzła
======================

Każda instancja ``irohad`` jest konfigurowana jednym plikiem JSON
przekazywanym flagą ``--config`` (wraz z ``--genesis_block`` i
``--keypair_name`` przy pierwszym starcie — zobacz :ref:`node-appimage`).

Repozytorium zawiera gotowe przykłady, które możesz skopiować i
zaadaptować:

- ``example/config.sample`` — ogólna konfiguracja (PostgreSQL na localhost)
- ``example/config.docker`` — używana przez obraz Docker
- ``example/config.postgres.sample`` — wariant jawnie na PostgreSQL
- ``example/config-win.sample`` — wariant dla Windows

.. note:: Ten przewodnik dotyczy forka utrzymywanego przez społeczność
  `baziorek/iroha1-cpp <https://github.com/baziorek/iroha1-cpp>`_.

Kluczowe ustawienia
-------------------

- ``database`` — backend storage: ``postgres`` (host, port, dane
  logowania, bazy working/maintenance) albo RocksDB przez
  ``block_store_path``
- ``torii_port`` — port gRPC dla klientów (domyślnie ``50051``)
- ``internal_port`` — port gRPC między peerami (domyślnie ``10001``)
- ``max_proposal_size`` — maksymalna liczba transakcji na blok/propozycję
- ``vote_delay`` / ``proposal_creation_timeout`` — timingi konsensusu
- ``mst_enable`` / ``mst_expiration_time`` — obsługa transakcji
  wielopodpisowych (zobacz :ref:`architecture`)
- ``metrics`` / ``healthcheck_port`` — endpointy obserwowalności

Pełna referencja parametrów
---------------------------

Każdy parametr konfiguracji jest szczegółowo udokumentowany — typy,
wartości domyślne, jednostki i skutki uboczne — w sekcji
:ref:`configuration`. W szczególności zobacz
`parametry zależne od środowiska <../configure/index.html#environment-specific-parameters>`_
z poradami dot. strojenia.

.. hint:: Dla sieci wielowęzłowej każdy peer potrzebuje własnego pliku
  konfiguracji, pary kluczy i genesis blocka wymieniającego wszystkie
  peery — konfigurację taką opisuje :ref:`deploy-guide`.
