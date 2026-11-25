.. _deploy-flags:

irohad Flags
================

Możesz uruchomić ``irohad`` z różnymi flagami. Niektóre z głównych
`zostały już wymienione <single.html#run-iroha-daemon-irohad>`__,
ale są inne, które mogą być przydatne w Twojej wyjątkowej sytuacji.
Oto one:

+--------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------+------------+---------------+
| Flag                     | Opis                                                                                                                                                    | Typ        | Domyślnie     |
+==========================+=========================================================================================================================================================+============+===============+
| ``-config``              | określa ścieżkę provisioning Iroha                                                                                                                      | ``string`` | ""            |
| ``-drop_state``          | usuwa istniejące dane stanu przy starcie                                                                                                                | ``bool``   | false         |
| ``-genesis_block``       | określa plik z blokiem początkowym                                                                                                                      | ``string`` | ""            |
| ``-keypair_name``        | określa nazwę plików .pub i .priv                                                                                                                       | ``string`` | ""            |
| ``-metrics_addr``        | adres nasłuchu serwera HTTP Prometeus                                                                                                                   | ``string`` | "127.0.0.1"   |
| ``-metrics_port``        | port nasłuchu serwera HTTP Prometeus, domyślnie wyłączony                                                                                               | ``string`` | ""            |
| ``-overwrite_ledger``    | nadpisuje dane ledgeru, jeśli istnieją                                                                                                                  | ``bool``   | false         |
| ``-reuse_state``         | próbuje ponownie użyć istniejących danych stanu przy starcie (przestarzałe — start domyślnie używa stanu ponownie; użyj ``drop_state``, aby usunąć WSV) | ``bool``   | true          |
| ``-verbosity``           | poziom szczegółowości logów                                                                                                                             | ``string`` | "config_file" |
| ``-wait_for_new_blocks`` | polityka synchronizacji przy starcie — czeka na nowe bloki w blockstore, nie uruchamia sieci                                                            | ``bool``   | false         |
| ``-exit_after_init``     | odtwarza WSV z magazynu bloków i kończy pracę; przydatne do naprawy stanu                                                                               | ``bool``   | false         |
|                          | lub reindeksacji                                                                                                                                        |            |               |
| ``-syncing_node``        | uruchamia węzeł w trybie syncing: podąża za łańcuchem od peerów bez                                                                                     | ``bool``   | false         |
|                          | uczestnictwa w konsensusie (zobacz :ref:`syncing-node`)                                                                                                 |            |               |
+--------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------+------------+---------------+
