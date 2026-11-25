.. _iroha-cli:

======================
``iroha-cli``
======================

``iroha-cli`` to wierszowy klient Irohy: interaktywne narzędzie do
wysyłania transakcji i zapytań do peera bez pisania kodu. Dostępny
jest w każdej dystrybucji razem z ``irohad`` — wybierz tę, którą już
masz.

Zdobycie ``iroha-cli``
----------------------

Docker
^^^^^^

Jeśli uruchamiasz węzeł z oficjalnego obrazu, ``iroha-cli`` jest już
wewnątrz kontenera:

.. code-block:: shell

  docker exec -it iroha iroha-cli -account_name admin@test

Argument ``-account_name`` wybiera, którego konta klucz podpisuje
transakcje — w przykładowej konfiguracji jest to ``admin@test``
(klucz leży obok pozostałych plików w ``example/``).

AppImage
^^^^^^^^

Pobierz ``iroha-cli-x86_64.AppImage`` ze strony
`GitHub Releases <https://github.com/baziorek/iroha1-cpp/releases>`__
razem z ``irohad``, nadaj prawa wykonywania i uruchom:

.. code-block:: shell

  chmod +x iroha-cli-x86_64.AppImage
  ./iroha-cli-x86_64.AppImage -account_name admin@test

.. note:: ``iroha-cli`` AppImage jest publikowany razem z wydaniem
  ``1.6.1`` — zobacz :ref:`node-appimage`.

Budowanie ze źródeł
^^^^^^^^^^^^^^^^^^^

Gdy :ref:`budujesz Irohę ze źródeł <node-build>`, ``iroha-cli`` powstaje
obok ``irohad``:

.. code-block:: shell

  ./build/bin/iroha-cli -account_name admin@test

Pierwsze uruchomienie
---------------------

Niezależnie od dystrybucji, pierwsze uruchomienie pyta o adres i port
peera — dla lokalnego węzła to ``localhost`` i ``50051`` (``torii_port``
z konfiguracji węzła). Dalej menu interaktywne pozwala składać
transakcje (``tx``) i zapytania (``qry``), wysyłać je do peera i
sprawdzać statusy transakcji.

.. note:: ``iroha-cli`` obejmuje tylko najczęstsze komendy — to narzędzie
  pomocnicze, nie pełne API. Pełny przewodnik wysyłania pierwszej
  transakcji znajdziesz w :doc:`cli-guide`; dostęp
  programistyczny opisują :doc:`python-guide` i
  `dokumentacja API <../develop/api/commands.html>`_.
