========
Shepherd
========

Shepherd to narzędzie wiersza poleceń, które pomaga wykonywać
zadania konserwacyjne na działającym demonie irohad.

Wymagania wstępne
=================

Aby uzyskać dostęp do demona irohad, musi być w nim
skonfigurowana ``utility service``.
Zobacz `szczegóły konfiguracji
<../configure/index.html#deployment-specific-parameters>`_.

Następnie, wywołując ``shepherd``, przekaż argument wiersza
poleceń ``--irohad`` z adresem i portem usługi utility irohad:

.. code-block:: shell

   ./shepherd --irohad 127.0.0.1:11001 <...>

Obsługiwane akcje
=================

Oto rzeczy, które możesz zrobić z ``shepherd``, określając
dodatkowe argumenty wiersza poleceń.

Zatrzymanie kontrolowane
^^^^^^^^^^^^^^^^^^^^^^^^
Jak wcześniej zatrzymywałeś irohę?
Co, naprawdę ją zabijałeś?
Och, proszę nigdy tego więcej nie rób — to nieuprzejme i
niemiłe!

.. code-block:: shell

   ./shepherd <...> --shutdown

Z argumentem ``--shutdown`` shepherd uprzejmie poprosi Irohę o
zatrzymanie.

Oglądanie jej pracy
^^^^^^^^^^^^^^^^^^^^
Powszechnie uważane za jedną z największych przyjemności jest
oglądanie, jak inni pracują.
Z shepherd możesz oglądać, jak Iroha pracuje!

.. code-block:: shell

   ./shepherd <...> --status

To zasubskrybuje aktualizacje statusu cyklu pracy.
Otrzymasz jednoznaczne komunikaty, gdy demon się uruchamia,
działa, kończy pracę lub właśnie się zatrzymał.

Inne parametry
==============

Możesz też ustawić poziom logowania:

.. code-block:: shell

   ./shepherd <...> --verbosity debug <...>

Obsługiwane wartości to ``trace``, ``debug``, ``info``,
``warning``, ``error`` i ``critical``.
