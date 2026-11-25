===================
Przegląd Irohy 1
===================

Jakie są kluczowe cechy Irohy?
------------------------------

- Prosta instalacja i utrzymanie
- Różnorodne biblioteki dla deweloperów
- Kontrola dostępu oparta na rolach
- Modułowa architektura oparta na zasadzie rozdziału komenda–zapytanie
- Zarządzanie aktywami i tożsamością

W naszym modelu jakości skupiamy się na ciągłym doskonaleniu:

- Niezawodności (odporność na awarie, odtwarzalność)
- Wydajności (w szczególności czasie wykonania i wykorzystaniu zasobów)
- Użyteczności (łatwości nauki, ochronie przed błędami użytkownika,
  rozpoznawalności)

Gdzie można używać Irohy?
-------------------------

Hyperledger Iroha to ogólnego przeznaczenia system blockchain z
uprawnieniami, który można wykorzystać do zarządzania aktywami
cyfrowymi, tożsamością i danymi serializowanymi. Przydatny w
aplikacjach takich jak rozliczenia międzybankowe, cyfrowe waluty banku
centralnego, systemy płatności, krajowe ID czy logistyka.

Szczegółowy opis znajdziesz w sekcji
`Scenariusze zastosowań <develop/cases.html>`_.

Czym różni się od Bitcoina czy Ethereum?
----------------------------------------

.. list-table::
   :header-rows: 1
   :widths: 20 35 35

   * - 
     - Bitcoin / Ethereum
     - Iroha 1
   * - Model dostępu
     - Permissionless — każdy może dołączyć i czytać wszystkie dane
     - Permissioned — tylko znani peery z nadanymi uprawnieniami mogą wchodzić w interakcje; zapytania też są permissioned
   * - Natywna kryptowaluta
     - Tak — wymagana do opłat za gas / opłaty transakcyjne
     - Nie — aktywa to zdefiniowane przez użytkownika aktywa domenowe, bez monety
   * - Logika niestandardowa
     - Smart kontrakty (Turing-complete, trzeba je pisać i audytować)
     - Wbudowane komendy i zapytania pokrywają typowe operacje; bez kodu kontraktów dla standardowych zadań
   * - Finalność konsensusu
     - Probabilistyczna (potwierdzenia narastają w czasie)
     - Deterministyczna — zatwierdzone bloki są finalne po commicie konsensusu

W Iroha nie ma natywnej kryptowaluty. Zamiast tego interakcja z
systemem jest permissioned — tylko osoby z odpowiednim dostępem mogą
z nim wchodzić w interakcje. Zapytania również są permissioned, więc
można kontrolować dostęp do wszystkich danych.

Jedną z największych różnic względem Ethereum jest to, że Hyperledger
Iroha pozwala użytkownikom wykonywać typowe funkcje — tworzenie i
transferowanie aktywów cyfrowych — za pomocą wbudowanych komend.
Eliminuje to potrzebę pisania nieporęcznych i trudnych w testowaniu
smart kontraktów, pozwalając deweloperom szybciej i bezpieczniej
wykonywać proste zadania.

Czym różni się od pozostałych frameworków Hyperledger i innych
blockchainów permissioned?
---------------------------------------------------------------

Iroha ma nowatorski algorytm konsensusu odporny na awarie (zwany YAC
[#f1]_), który jest wydajny i zapewnia finalność transakcji z niską
latencją.

Wbudowane komendy Irohy to duża zaleta względem innych platform —
typowe zadania jak tworzenie aktywów cyfrowych, rejestracja kont czy
transfery są niezwykle proste. Zawężają też wektor ataku, poprawiając
ogólne bezpieczeństwo systemu.

Wreszcie, Iroha to jedyny ledger z rozbudowanym systemem uprawnień,
pozwalającym ustawiać permissiony dla wszystkich komend, zapytań i
dołączania do sieci.

.. [#f1] Yet Another Consensus

Jak tworzyć aplikacje wokół Irohy?
----------------------------------

Najszybszy start daje przewodnik :doc:`Getting Started
<../getting_started/index>`, który prowadzi przez budowę węzła i
uruchomienie pierwszych transakcji.

Aby wnieść moc blockchaina do swojej aplikacji, najpierw przemyśl, jak
będzie ona komunikować się z peerami Irohy. Dobrym punktem startowym
jest sekcja `Pojęcia i architektura
<concepts_architecture/index.html>`_, wyjaśniająca czym jest transakcja
i zapytanie oraz jak użytkownicy Twojej aplikacji mają z nimi
wchodzić w interakcje.

Mamy też kilka bibliotek klienta, które dostarczają deweloperom
klocków budulcowych — podpisy, komendy, wysyłanie wiadomości do peerów
Irohy i sprawdzanie statusów.

Znane ograniczenia i status projektu
------------------------------------

Zanim zbudujesz coś na Iroha 1, warto znać aktualny stan projektu —
kilka ograniczeń ma charakter architektoniczny lub historyczny, a nie
bugów:

- **Cykl życia**: upstreamowa Hyperledger Iroha 1 jest EOL; ta
  dokumentacja opisuje forka społecznościowego ``baziorek/iroha1-cpp``,
  utrzymywanego przez jednego maintainera (z pomocą AI) i **bez
  gwarancji produkcyjnych** — zobacz ostrzeżenie na :doc:`index`.
- **Model konsensusu**: wdrożenia Irohy 1 uruchamiają YAC w **trybie
  CFT** (odporność na awarie, prosta większość) — model spójności jest
  stałą kompilacyjną i nie da się go przełączyć na BFT konfiguracją,
  choć algorytm to umie. Zobacz :ref:`yac`.
- **Biblioteki klienta**: utrzymywane są tylko C++ ``iroha-lib`` i fork
  Pythona ``iroha1-python``; biblioteki Java, JavaScript i iOS są
  nieutrzymywane (zobacz :doc:`develop/libraries`).
- **Brak obrazu kontenerowego runtime**: CI forka publikuje pakiety
  release'owe (deb/rpm/AppImage) i obraz buildera, ale nie gotowy obraz
  Dockera z ``irohad`` — zobacz :ref:`build-guide`.
- **Wydajność**: Iroha 1 nie ma utrzymywanego zestawu testów
  wydajnościowych dla obecnego toolchaina; liczby przepustowości ze
  starych release'ów mogą nie obowiązywać.
- **Powierzchnia API**: Torii nie ma wbudowanego rate limitingu —
  przed publicznymi endpointami postaw reverse proxy.
- **Kompatybilność**: Iroha 1 i Iroha 2 są **niezgodne na poziomie
  protokołu i stanu** — nie ma ścieżki migracji w miejscu; zobacz
  notkę na :doc:`index`.
