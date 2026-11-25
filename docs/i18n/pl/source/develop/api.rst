Iroha API reference
===================

W sekcji API przyjrzymy się elementom budulcowym aplikacji
wchodzącej w interakcję z Irohą. Omówimy komendy i zapytania,
które system posiada, oraz zestaw bibliotek klienckich obejmujących
logikę transportu i warstwy aplikacji.

API Irohy podąża za zasadą `rozdzielenia komend od zapytań
<https://en.wikipedia.org/wiki/Command%E2%80%93query_separation>`_.

Komunikacja między peerem Iroha a aplikacją kliencką odbywa się
za pośrednictwem frameworka `gRPC <https://grpc.io/about/>`_.
Aplikacje klienckie powinny postępować zgodnie z opisanym
protokołem i formować transakcje zgodnie z ich
`opisem <../concepts_architecture/glossary.html#transaction>`_.


.. toctree::
    :maxdepth: 2
    :caption: Spis treści

    api/commands.rst
    api/queries.rst
    api/permissions.rst

Komendy są tutaj częściami `transakcji
<../concepts_architecture/glossary.html#transaction>`_ —
zmieniającego stan zestawu akcji w systemie. Gdy transakcja
przejdzie etapy walidacji i konsensusu, jest zapisywana w `bloku
<../concepts_architecture/glossary.html#block>`_ i zapisywana w
niezmienialnym magazynie bloków (blockchain).

Transakcje składają się z komend wykonujących akcję na
`encji <../concepts_architecture/er_model.html>`_ w systemie.
Encją może być konto, aktywo itp.
