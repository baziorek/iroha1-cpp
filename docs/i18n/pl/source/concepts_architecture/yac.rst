.. _yac:

======================
Konsensus (YAC)
======================

YAC — *Yet Another Consensus* — to algorytm konsensusu używany przez
Irohę 1 do uzgadniania uporządkowanej listy transakcji (bloków) między
wszystkimi peerami. To protokół oparty na liderze i głosowaniu z
synchronicznymi rundami: każda runda produkuje albo zatwierdzony blok,
albo pusty wynik ("nic do zatwierdzenia"), albo odrzucenie
przesuwające rundę dalej.

Algorytm opisano w artykule
`YAC: BFT Consensus Algorithm for Blockchain <https://arxiv.org/abs/1809.00554>`_
oraz w `tym wideo <https://youtu.be/mzuAbalxOKo>`_.
Implementacja C++ znajduje się w ``irohad/consensus/yac/``.

Modele spójności
----------------

YAC obsługuje dwa modele spójności, zaimplementowane jako *supermajority
checkery* (``irohad/consensus/yac/supermajority_checker.hpp``):

- **CFT** (*Crash Fault Tolerant*) — wymaga prostej większości głosów.
  Wybierany przez ``ConsistencyModel::kCft``.
- **BFT** (*Byzantine Fault Tolerant*) — wymaga superwiększości ~2/3
  głosów, tolerując peery wysyłające dowolne (bizantyjskie) wiadomości.
  Wybierany przez ``ConsistencyModel::kBft``.

Oba modele używają wzoru ``Kf+1``: superwiększość jest osiągana, gdy
``number * K >= (K - 1) * (all - 1) + K``, gdzie ``all`` to liczba
peerów w rundzie, a ``K`` wynosi 2 dla CFT i 3 dla BFT.

.. csv-table::
    :header: "Peery (N)", "Głosy CFT", "Głosy BFT"
    :widths: 20, 25, 25

    "1", "1", "1"
    "2", "2", "2"
    "3", "2", "3"
    "4", "3", "3"
    "5", "3", "4"
    "7", "4", "5"
    "10", "6", "7"
    "13", "7", "9"
    "16", "9", "11"
    "31", "16", "21"

.. note::
    Iroha 1 obecnie używa modelu **CFT** — to stała kompilacyjna
    (``kConsensusConsistencyModel`` w ``irohad/main/application.cpp``,
    przełączona na CFT w 2019). Checker BFT pozostaje w kodzie, ale
    nie da się go wybrać konfiguracją.

Peery i ordering
----------------

Każdy peer ma stabilną tożsamość (swój rekord ``Peer``: adres, klucz
publiczny, opcjonalny certyfikat TLS) utrzymywaną w world state view.
Peery zarejestrowane jako **syncing peery** są trzymane w osobnej tabeli
``sync_peer``: nie dołączają do cluster ordering, nie proponują ani nie
głosują, ale otrzymują stan konsensusu bezpośrednio, więc mogą
podążać za ledgerem — przydatne dla węzłów backupowych i audytowych.

Dla każdej rundy peery są układane w *cluster ordering* — sekwencję
wywiedzioną z listy peerów w ledgerze, w której każdy peer występuje
dokładnie raz. **Liderem** rundy jest pierwszy peer w tym porządku;
przy odrzuceniu rundy porządek rotuje i następnym liderem staje się
kolejny peer (``ClusterOrdering::switchToNext``). To gwarantuje, że
kolejne rundy odrzutowe próbują różnych peerów jako liderów, więc
pojedynczy wadliwy lub nieosiągalny lider nie może zablokować łańcucha.

Rundy
-----

Konsensusowa ``Round`` to para ``(block_round, reject_round)``:

- ``block_round`` — wysokość ledgeru, o którą toczy się decyzja; równa
  się ``wysokość top bloku + 1``. Zwiększa się tylko przy zatwierdzeniu
  bloku (lub pustego commita).
- ``reject_round`` — licznik *wewnątrz* tej samej ``block_round``.
  Zwiększa się przy każdym odrzuceniu rundy, żeby peery nie mieszały
  głosów z różnych prób na tej samej wysokości.

Hash głosu
----------

Każdy peer głosuje na ``YacHash``, który wiąże trzy rzeczy:

- ``vote_round`` (żeby głosy nie mogły być replayowane między rundami),
- ``proposal_hash`` — hash propozycji (uporządkowanego zbioru
  transakcji), którą peer widział,
- ``block_hash`` — hash bloku, który ``Simulator`` zbudował z
  zweryfikowanej propozycji.

Sam głos to ``VoteMessage``: ``YacHash`` plus podpis peera. Ponieważ
głos pokrywa zarówno propozycję, jak i blok, peer nie może zostać
oszukany, by poparł blok nieodpowiadający propozycji, którą zatwierdził.

Przepływ protokołu
------------------

1. **Propozycja.** Usługa ordering zbiera oczekujące transakcje (do
   ``max_proposal_size``) i formuje propozycję dla bieżącej
   ``block_round``. Peery, które zostają w tyle, są doganianie przez
   ``Synchronizer`` zamiast uczestniczyć.
2. **Tworzenie bloku.** ``Simulator`` wykonuje walidację stateless i
   stateful transakcji propozycji i produkuje kandydujący blok
   (``BlockCreator`` + ``VerifiedProposalCreator``).
3. **Głosowanie.** Każdy peer zgadzający się z danymi rundy czeka
   ``vote_delay`` milisekund (żeby wolniejsi peery zdążyli
   odpowiedzieć), a potem rozgłasza swój ``VoteMessage`` dla
   ``YacHash``.
4. **Wynik.** Głosy są gossipowane między peerami. Dla każdej rundy peer
   śledzi grupy głosów per ``YacHash`` i stosuje checker superwiększości:

   - **Commit** — jeden ``YacHash`` zebrał superwiększość. Wszystkie
     peery, które na niego głosowały, zbiegają się na tym samym bloku;
     zebrane głosy stają się podpisami bloku. Peery, które w tej samej
     rundzie głosowały na inny hash, i tak zatwierdzają zwycięzcę —
     wiadomość commit niesie głosy zwycięzcy jako dowód.
   - **Commit na "nic"** — superwiększość zagłosowała za pustą rundę
     (bez transakcji), co nie produkuje bloku, ale nadal przesuwa
     ``block_round``.
   - **Reject** — głosy są tak rozłożone, że *żadna* opcja nie może już
     osiągnąć superwiększości (``canHaveSupermajority`` zawodzi).
     Runda jest odrzucana, ``reject_round`` rośnie, lider rotuje i
     zaczyna się nowa próba.
   - **Future** — peer otrzymuje głosy dla rundy większej niż jego
     bieżąca; synchronizuje swój stan rundy do przodu.
5. **Obsługa commita.** Na ``CommitMessage`` ``HashGate`` weryfikuje
   zbiór głosów (podpisy, rundę, hashe), peer dopisuje blok do ledgeru
   przez ``Ametsuchi`` i zapisuje zebrane podpisy na nim. Bloki, których
   głosów nie da się zweryfikować, są ignorowane; brakujące bloki
   pobiera później ``Synchronizer``.

Obsługa awarii
--------------

- Padnięty lub podzielony peer po prostu przestaje głosować; pozostałe
  peery kontynuują, dopóki próg superwiększości jest osiągalny (zobacz
  tabelę powyżej).
- Lider, który nie produkuje użytecznej propozycji, wywołuje rundy
  odrzutowe; rotujący ordering przenosi przywództwo na następnego
  peera.
- Jeśli peery nie zgadzają się co do stanu ledgeru (np. po partycji),
  głosy za różne ``block_hash`` dzielą elektorat, dopóki jedna opcja
  nie stanie się nieosiągalna — runda jest odrzucana i ponawiana.
- Zatwierdzone bloki są finalne: gdy superwiększość zagłosuje, wynik
  jest deterministyczny i identyczny na wszystkich uczciwych peerach.

Pokrętła konfiguracji
---------------------

Zachowanie YAC stroi się przez konfigurację węzła (zobacz
:ref:`configuration`):

- ``vote_delay`` — jak długo peer czeka przed głosowaniem, dając wolnym
  peerom czas na odpowiedź. Za mała wartość spowalnia produkcję bloków
  w sieciach z nieosiągalnymi peerami.
- ``proposal_creation_timeout`` — maksymalny czas, przez jaki ordering
  czeka przy składaniu propozycji (transakcje gromadzące się do
  ``max_proposal_size``) przed odpowiedzią na prośbę o propozycję;
  domyślnie 3000 ms. Dawniej nazywało się ``max_rounds_delay``.
  Drugi, wyprowadzony timeout — sieciowy deadline dla RPC
  ``RequestProposal`` — jest obliczany automatycznie jako
  ``2 × proposal_creation_timeout`` i nie jest osobno konfigurowalny.
- ``max_proposal_size`` — maksymalna liczba transakcji na propozycję,
  czyli maksymalny rozmiar bloku.

.. note::
    ``stale_stream_max_rounds`` wpływa tylko na klientowy strumień
    statusów, nie na sam konsensus.

Zobacz też
----------

- `Konsensus w architekturze <architecture.html#block-consensus-yac>`__
- `Wpis o konsensusie w glosariuszu <glossary.html#consensus>`__
- `Konfiguracja <../configure/index.html>`__
- Artykuł: `YAC: BFT Consensus Algorithm for Blockchain <https://arxiv.org/abs/1809.00554>`__
