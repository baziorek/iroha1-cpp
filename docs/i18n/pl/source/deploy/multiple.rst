==========================================
Uruchamianie wielu instancji (sieć peerów)
==========================================

Aby skonfigurować sieć peerów, należy postępować zgodnie z
procedurami opisanymi w tej sekcji. W tej wersji obsługujemy
wdrożenie ręczne i automatyczne za pomocą Ansible Playbook.
Wybierz opcję, która spełnia Twoje kryteria bezpieczeństwa i inne
potrzeby.

Ręcznie
-------

Przez wdrożenie ręczne rozumiemy konfigurację sieci peerów Iroha
bez automatyzacji. Jest to podobne do procesu uruchamiania
pojedynczej instancji lokalnej, z tą różnicą, że genesis block
zawiera więcej niż jednego peera. Aby utworzyć blok zawierający
więcej niż jednego peera lub wymagający dostosowania do Twoich
potrzeb, sprawdź sekcję :ref:`deploy_troubles`.

Automatycznie
-------------

`iroha-swarm <https://github.com/kuvaldini/iroha-swarm>`_
generuje pliki konfiguracyjne docker-compose do uruchomienia
kilku węzłów Iroha jednym poleceniem. Możesz wybrać typ bazy
danych — RocksDB lub Postgres — podać klucze peerów lub użyć
wygenerowanych. Bardzo wygodne do generowania podstawowego/pierwszego
pliku konfiguracyjnego i dla regularnie zmieniającego się
środowiska (różne zestawy węzłów).

.. note:: ``iroha-swarm`` to narzędzie społeczności, nieutrzymywane
  od ~2019 — sprawdź je dokładnie przed użyciem; może
  odwoływać się do tagów obrazów, które już nie istnieją (np.
  ``:latest``).

Jest też `przewodnik
<https://github.com/hyperledger/iroha-deploy/blob/master/ansible/roles/iroha-docker/README.md>`__
(zarchiwizowane repozytorium), który może być nieaktualny, ale
może dostarczyć przydatnych informacji.
