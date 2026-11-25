Przewodnik CLI: wysyłanie pierwszych transakcji i zapytań
=========================================================

.. note:: Ta strona zakłada, że ``iroha-cli`` jest już dostępny —
  jest dołączony do dystrybucji węzła. Zobacz :doc:`cli`, jak go
  uzyskać przez Docker, AppImage lub build ze źródeł.

Z Irohą możesz wchodzić w interakcję na różne sposoby.
Możesz używać naszych bibliotek klienckich,
aby pisać kod w różnych językach programowania (np. Java, Python,
Javascript, Swift), który komunikuje się z Irohą.
Alternatywnie możesz użyć ``iroha-cli`` — naszego narzędzia
wiersza poleceń do interakcji z Irohą.
W ramach tego przewodnika zapoznajmy się z ``iroha-cli``.

.. Attention:: Chociaż ``iroha-cli`` jest niewątpliwie
  najprostszym sposobem rozpoczęcia pracy z Irohą, pokrywa tylko
  część możliwych komend/zapytań, więc doświadczenie użytkownika
  może nie być najlepsze. Jeśli chcesz nam pomóc zbudować lepszą
  wersję CLI, daj nam znać!

.. raw:: html

  <script src="https://asciinema.org/a/6dFA3CWHQOgaYbKfQXtzApDob.js" id="asciicast-6dFA3CWHQOgaYbKfQXtzApDob" async></script>

Otwórz nowy terminal (zauważ, że kontener Iroha i ``irohad``
powinny być uruchomione) i dołącz do kontenera ``iroha``:

.. code-block:: shell

  docker exec -it iroha /bin/bash

Jesteś ponownie w interaktywnej powłoce kontenera Iroha.
Musimy uruchomić ``iroha-cli`` i przekazać nazwę konta żądanego
użytkownika. W naszym przykładzie konto ``admin`` jest już
utworzone w domenie ``test``. Użyjmy tego konta do pracy z Irohą.

.. code-block:: shell

  iroha-cli -account_name admin@test

.. note:: Pełna nazwa konta ma symbol ``@`` między nazwą a domeną.
  Zauważ, że para kluczy ma tę samą nazwę.

Tworzenie pierwszej transakcji
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Widzisz teraz interfejs ``iroha-cli``.
Utwórzmy nowy asset, dodajmy trochę assetu na konto admina i
przelejmy go na inne konto. Aby to osiągnąć, wybierz opcję
``1. New transaction (tx)`` wpisując ``tx`` lub ``1`` do konsoli.

Widzisz teraz listę dostępnych komend.
Spróbujmy utworzyć nowy asset.
Wybierz ``14. Create Asset (crt_ast)``.
Teraz wpisz nazwę swojego assetu, na przykład ``coolcoin``.
Następnie wpisz ID domeny. W naszym przykładzie mamy już domenę
``test``, więc jej użyjmy.
Potem musimy wpisać precyzję assetu
– liczbę cyfr w części ułamkowej.
Ustawmy precyzję na ``2``.

Gratulacje, utworzyłeś swoją pierwszą komendę i dodałeś ją do
transakcji! Możesz ją wysłać do Irohy albo dodać więcej komend
``1. Add one more command to the transaction (add)``.
Dodajmy więcej komend, żeby zrobić wszystko za jednym zamachem.
Wpisz ``add``.

Spróbujmy teraz dodać trochę ``coolcoinów`` na nasze konto.
Wybierz ``16. Add Asset Quantity (add_ast_qty)``, wpisz ID assetu
– ``coolcoin#test``, część całkowitą i precyzję.
Na przykład, aby dodać 200.50 — wpisz część całkowitą jako
``20050`` i precyzję jako ``2``, więc wychodzi ``200.50``.

.. note:: Pełna nazwa assetu ma symbol ``#`` między nazwą a
  domeną.

Przelejmy 100.50 ``coolcoinów`` z ``admin@test`` na ``test@test``
dodając jeszcze jedną komendę i wybierając
``5. Transfer Assets (tran_ast)``.
Wpisz Source Account i Destination Account — w naszym przypadku
``admin@test`` i ``test@test`` — Asset ID (``coolcoin#test``),
część całkowitą i precyzję (odpowiednio ``10050`` i ``2``).

Teraz musimy wysłać naszą transakcję do peera Irohy
(``2. Send to Iroha peer (send)``). Wpisz adres peera (w naszym
przypadku ``localhost``) i port (``50051``).
Twoja transakcja została właśnie przesłana i możesz zobaczyć jej
hash. Możesz go użyć do sprawdzenia statusu transakcji.

Wróć do terminala, w którym działa ``irohad``.
Możesz zobaczyć logi swojej transakcji.

Jupi! Przesłałeś swoją pierwszą transakcję do Irohy.

Tworzenie pierwszego zapytania
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sprawdźmy teraz, czy ``coolcoiny`` zostały pomyślnie przelane z
``admin@test`` na ``test@test``. Wybierz
``2. New query (qry)``.
``8. Get Account's Assets (get_acc_ast)`` pomoże Ci sprawdzić, czy
``test@test`` ma teraz ``coolcoin``. Sformułuj zapytanie w
podobny sposób, jak robiłeś to z komendami, i
``1. Send to Iroha peer (send)``.
Możesz teraz zobaczyć informacje o tym, ile ``coolcoinów`` ma
``test@test``. Będzie to wyglądać podobnie:

.. code-block:: shell

  [2018-03-21 12:33:23.179275525][th:36][info] QueryResponseHandler [Account Assets]
  [2018-03-21 12:33:23.179329199][th:36][info] QueryResponseHandler -Account Id:- test@test
  [2018-03-21 12:33:23.179338394][th:36][info] QueryResponseHandler -Asset Id- coolcoin#test
  [2018-03-21 12:33:23.179387969][th:36][info] QueryResponseHandler -Balance- 100.50

Czy to nie jest awesome?
Przesłałeś swoje pierwsze zapytanie do Irohy i otrzymałeś
odpowiedź!

.. hint:: Aby uzyskać informacje o wszystkich dostępnych komendach
  i zapytaniach, sprawdź naszą sekcję API.

Bycie złym
^^^^^^^^^^

Spróbujmy być złymi i oszukać Irohę. Na przykład spróbujmy
przelać więcej ``coolcoinów``, niż ma ``admin@test``.
Spróbuj przelać 100000.00 ``coolcoinów`` z ``admin@test`` na
``test@test``. Znowu przejdź do ``1. New transaction (tx)``,
``5. Transfer Assets (tran_ast)``, wpisz Source Account i
Destination Account — w naszym przypadku ``admin@test`` i
``test@test`` — Asset ID (``coolcoin#test``), część całkowitą i
precyzję (odpowiednio ``10000000`` i ``2``).
Wyślij transakcję do peera Irohy jak wcześniej.
No cóż, mówi

.. code::

  [2018-03-21 12:58:40.791297963][th:520][info] TransactionResponseHandler Transaction successfully sent
  Congratulation, your transaction was accepted for processing.
  Its hash is fc1c23f2de1b6fccbfe1166805e31697118b57d7bb5b1f583f2d96e78f60c241

`Twoja transakcja została przyjęta do przetworzenia`.
Czy to znaczy, że pomyślnie oszukaliśmy Irohę?
Spróbujmy zobaczyć status transakcji.
Wybierz ``3. New transaction status request (st)`` i wpisz hash
transakcji, który możesz zdobyć w konsoli po poprzedniej komendzie.
Wyślij go do Irohy.
Odpowiada:

.. code::

  Transaction has not passed stateful validation.

Widocznie nie.
Nasza transakcja nie została przyjęta, bo nie przeszła stanowej
walidacji i ``coolcoiny`` nie zostały przelane.
Możesz sprawdzić status ``admin@test`` i ``test@test``
zapytaniami, żeby się upewnić (jak robiiliśmy wcześniej).

Opcje ``iroha-cli``
-------------------

Oprócz trybu interaktywnego pokazanego powyżej, ``iroha-cli``
przyjmuje flagi wiersza poleceń do użycia nieinteraktywnego i
skryptowego:

+==========================+==============================================================+
| Flaga                    | Opis                                                         |
+==========================+==============================================================+
| ``-account_name``        | ID konta, na którym działamy (np. ``admin@test``)            |
+================----------+--------------------------------------------------------------+
| ``-new_account``         | tworzy parę kluczy konta przy pierwszym uruchomieniu         |
+--------------------------+--------------------------------------------------------------+
| ``-key_path``            | katalog zawierający klucze ``<name>.pub``/``<name>.priv``    |
+--------------------------+--------------------------------------------------------------+
| ``-pass_phrase``         | hasło do zaszyfrowanych kluczy prywatnych                    |
+--------------------------+--------------------------------------------------------------+
| ``-peer_ip``             | adres węzła Iroha (domyślnie ``127.0.0.1``)                  |
+--------------------------+--------------------------------------------------------------+
| ``-torii_port``          | port Torii węzła (domyślnie ``50051``)                       |
+--------------------------+--------------------------------------------------------------+
| ``-tls_root_cert``       | ścieżka do certyfikatu PEM root; włącza transport TLS        |
+--------------------------+--------------------------------------------------------------+
| ``-json_transaction``    | wysyła transakcję opisaną jako JSON zamiast interaktywnej    |
+--------------------------+--------------------------------------------------------------+
| ``-json_query``          | wysyła zapytanie opisane jako JSON zamiast interaktywnego    |
+--------------------------+--------------------------------------------------------------+
| ``-genesis_block``       | generuje plik bloku genesis i kończy pracę                  |
+--------------------------+--------------------------------------------------------------+
| ``-genesis_transaction`` | transakcja używana przy generowaniu bloku genesis            |
+--------------------------+--------------------------------------------------------------+
| ``-peers_address``       | lista peerów używana w generowanym bloku genesis             |
+--------------------------+--------------------------------------------------------------+
| ``-interactive``         | uruchamia powłokę interaktywną (domyślnie ``true``; ustaw     |
+--------------------------+--------------------------------------------------------------+
|                          | ``false`` dla trybu JSON jednorazowego)                      |
+--------------------------+--------------------------------------------------------------+

Na przykład, aby wysłać gotową transakcję JSON bez powłoki
interaktywnej:

.. code-block:: shell

  iroha-cli -account_name admin@test \
            -peer_ip 127.0.0.1 -torii_port 50051 \
            -json_transaction tx.json -interactive=false

Gdy endpoint Torii węzła ma włączone TLS (zobacz
:doc:`../configure/torii-tls`), wskaż ``-tls_root_cert`` na
certyfikat PEM węzła — ta sama flaga działa też w trybie
interaktywnym.
