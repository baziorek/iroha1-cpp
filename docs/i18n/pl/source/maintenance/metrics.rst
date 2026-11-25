=======
Metryki
=======

Aby wygodnie i dokładnie monitorować wydajność sieci, możesz
teraz używać metryk. Są to dane liczbowe zbierane w czasie o Twojej
sieci Iroha. Możesz następnie analizować dane, aby uczynić swój
projekt jeszcze bardziej wydajnym!

Jak używać metryk?
==================

Aby używać metryk, musisz dodać je do swojej `konfiguracji Iroha
<../configure/index.html#deployment-specific-parameters>`_ i
użyć wersji Irohy nowszej niż 1.2.1.

.. note:: Jeśli uruchamiasz Irohę w Dockerze, aby uzyskać dostęp
   do metryk spoza kontenera musisz: 1) W `konfiguracji
   <../configure/index.html>`_ — ustawić ``"metrics":0.0.0.0:PORT``;
   2) Udostępnić odpowiedni port w Dockerze podczas wykonywania
   ``run ... -pPORT:PORT ...``

Następnie możesz po prostu użyć adresu IP, aby uzyskać dostęp do
danych z uruchomionej instancji Iroha.

Oto przykład:

.. code-block:: shell

  > curl http://127.0.0.1:8080/metrics

zwróci wyniki takie jak:

.. code-block:: shell

  # HELP blocks_height Total number of blocks in chain
  # TYPE blocks_height gauge
  blocks_height 135543
  # HELP peers_number Total number peers to send transactions and request proposals
  # TYPE peers_number gauge
  peers_number 7
  # HELP number_of_domains Total number of domains in WSV
  # TYPE number_of_domains gauge
  number_of_domains 14
  # HELP total_number_of_transactions Total number of transactions in blockchain
  # TYPE total_number_of_transactions gauge
  total_number_of_transactions 216499
  # HELP number_of_signatures_in_last_block Number of signatures in last block
  # TYPE number_of_signatures_in_last_block gauge
  number_of_signatures_in_last_block 5

Metryki transakcji wielopodpisowych (MST)
=========================================

Gdy używane są transakcje wielopodpisowe, endpoint dodatkowo
udostępnia liczniki dla MST oczekujących na podpisy:

.. code-block:: shell

  # HELP number_of_pending_mst_batches Number of pending MST batches
  # TYPE number_of_pending_mst_batches gauge
  number_of_pending_mst_batches 0
  # HELP number_of_pending_mst_transactions Number of pending MST transactions
  # TYPE number_of_pending_mst_transactions gauge
  number_of_pending_mst_transactions 0

Te liczniki odzwierciedlają lokalną pamięć podręczną pending-MST —
są najbardziej przydatne na peerach z ``mst_enable`` ustawionym
na ``true`` w konfiguracji (zobacz :ref:`configuration`), gdzie
stan MST jest współdzielony między peerami.
