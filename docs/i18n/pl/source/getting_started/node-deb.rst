.. _node-deb:

====================================
Węzeł Iroha z pakietów release'owych
====================================

Oprócz obrazów Docker, AppImage i buildów ze źródeł, pipeline release'owy
Irohy 1 dołącza natywne pakiety do `GitHub Releases
<https://github.com/baziorek/iroha1-cpp/releases>`__ forka
społecznościowego:

- ``iroha_*.deb`` — pakiet Debian/Ubuntu budowany przez CPack;
  zawiera ``irohad`` i narzędzia maintenance (migration tool, WSV
  checker)
- ``iroha-*.rpm`` — odpowiednik dla dystrybucji RPM, gdy jest
  publikowany
- ``irohad-x86_64.AppImage`` / ``iroha-cli-x86_64.AppImage`` — zobacz
  :doc:`node-appimage`

.. note:: Pakiety pojawiają się na stronie Releases począwszy od tagu
  ``1.6.1``. Do tego czasu użyj opcji :ref:`Docker <node-docker>`
  albo :ref:`budowania ze źródeł <node-build>`.

Instalacja pakietu .deb
-----------------------

Pobierz plik ``.deb`` ze strony Releases, a potem zainstaluj go przez
``apt`` (który rozwiąże też zależności):

.. code-block:: shell

  sudo apt install ./iroha_*.deb

albo bezpośrednio przez ``dpkg``:

.. code-block:: shell

  sudo dpkg -i iroha_*.deb
  # jeśli apt zgłosi brakujące zależności:
  sudo apt -f install

Pakiet instaluje binaria systemowo (domyślny prefiks CPack ``/usr``).
Sprawdź:

.. code-block:: shell

  which irohad
  irohad --help

Instalacja pakietu .rpm
-----------------------

Na dystrybucjach opartych o RPM (Fedora, RHEL, openSUSE):

.. code-block:: shell

  sudo dnf install ./iroha-*.rpm
  # albo: sudo rpm -i iroha-*.rpm && sudo dnf -f install

Uruchomienie węzła
------------------

Zapakowany ``irohad`` przyjmuje te same argumenty co każdy inny build —
plik konfiguracji, genesis block i nazwę pary kluczy:

.. code-block:: shell

  sudo irohad --config /etc/iroha/config.json \
              --genesis_block /etc/iroha/genesis.block \
              --keypair_name /etc/iroha/node0

Nie ma dołączonej konfiguracji — skopiuj przykłady z katalogu
``example/`` repozytorium (zobacz :doc:`configuration`) i dostosuj je
do swojego środowiska, w tym sekcję ``database`` (PostgreSQL albo
RocksDB), która musi być osiągalna przed startem węzła.

.. note:: Zapakowany węzeł nadal potrzebuje PostgreSQL (albo RocksDB)
  i — dla wdrożenia produkcyjnego — terminacji TLS przed Torii; zobacz
  :doc:`../configure/torii-tls`.
