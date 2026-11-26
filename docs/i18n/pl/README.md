# Witamy!

**Języki:** [English](../../../README.md) · **Polski** · [Русский](../ru/README.md) · [日本語](../ja/README.md) · [Türkçe](../tr/README.md)

## O tym repozytorium

To jest fork społecznościowy **Hyperledger Iroha 1** (implementacja w C++).
Rozwój upstream praktycznie się zatrzymał — ostatni publiczny commit pochodzi
z **maja 2024** — mimo to Iroha 1 jest intensywnie używana w produkcji
przez kilka inicjatyw, w tym **Narodowy Bank Kambodży**
(system płatności Bakong).

Celem tego forka jest utrzymanie Irohy 1 w stanie pozwalającym budować,
używać i rozwijać ją nowoczesnymi toolchainami — oraz nadanie projektowi
zdrowego drugiego życia.

### Czym ten fork różni się od upstream

W porównaniu z ostatnim commitem upstream (`118fced3f3`, maj 2024) ten fork
wprowadza:

**Modernizacja budowania**

- Standard C++ podniesiony do **C++23**; build weryfikowany na
  **GCC 14 / Ubuntu 24.04**
- **vcpkg** zaktualizowany z 2021 roku do listopada 2026 (protobuf, gRPC,
  Boost, OpenSSL 3.x, soci 4.x)
- Przestarzałe zależności zastąpione tam, gdzie wystarcza biblioteka
  standardowa (`fmt` → `std::format`, `boost::format`, `boost::variant`,
  `boost::filesystem` oraz kilka nieużywanych bibliotek Boost usunięte)
- Build bez warningów (~70 warningów wyczyszczonych)

**Nowe komponenty**

- **`iroha-lib/`** — samodzielna biblioteka klienta C++ dla Irohy 1
  (transakcje, query, paginacja, streaming, transport TLS,
  narzędzia do plików kluczy/genesis/peer list) z 9 przykładami
  przeniesionymi z `iroha-python`. Zobacz `iroha-lib/README.md` i
  `docs/source/develop/libraries/cpp.rst`.
- **`iroha-cli/`** — klasyczny interaktywny/nieinteraktywny CLI,
  przeniesiony na transport i budowanie modeli z `iroha-lib`,
  z obsługą `--tls_root_cert`.
- **`iroha-python`** — biblioteka klienta Pythona znajduje się
  w osobnym repozytorium: [baziorek/iroha1-python](https://github.com/baziorek/iroha1-python)
  (oryginalne źródła, zaktualizowane zależności).

**Poprawki serwerowe**

- **Keepalive** gRPC na wszystkich serwerach — naprawia zrywane
  połączenia za NAT-ami/LB-ami, przez które strumienie długo-żyjące
  psuły się mniej więcej co 24h (upstream issue hyperledger/iroha#1351)
- Naprawa zawieszania `FetchCommits`, poprawka oneof w
  `GetPendingTransactions`, poprawki `creator_account_id` —
  wszystko znalezione dzięki nowym testom integracyjnym

**Release i CI**

- Pakiety **CPack** DEB/RPM + artefakty AppImage; **GitHub Releases**
  na tagach wersji (`1.6.1-reanimation` kontynuuje linię wersji
  upstream; wersje pakietów derywowane z `git describe`)
- Zmodernizowane GitHub Actions: runnery Node 24, akcje przypięte
  do SHA commitów, zależności pip zablokowane haszami, wspólny job
  cache'u zależności vcpkg
- **OpenSSF Scorecard** + **SECURITY.md** + Dependabot; branch
  protection na `iroha1-main`
- Stack testów obciążeniowych zmigrowany z `locustio` na `locust` 2.x

**Dokumentacja**

- Dokumentacja Sphinx przebudowana i opublikowana na **GitHub Pages**:
  <https://baziorek.github.io/iroha1-cpp/>
- Pełne tłumaczenia na **polski, rosyjski, japoński i turecki** —
  w tym ten README (zobacz linki językowe u góry)

**Usunięty martwy kod**

- Pakowanie `snap/` (niemożliwe do zbudowania na współczesnych
  zależnościach)
- Skrypty `housekeeping/` (martwe CI Soramitsu), skrypty `utils/`
  (zastąpione; parsowanie genesis przeniesione do forka Pythona)
- Nieaktualne patche vcpkg i workflow'y CI Irohy 2

> **Zastrzeżenie:** te forki społecznościowe są utrzymywane przez jedną
> osobę przy pomocy AI. Zawierają aktualizacje utrzymaniowe,
> systemu budowania i zależności — nie zmiany funkcjonalności Irohy —
> ale **nie dają gwarancji produkcyjnej ani komercyjnej**. Sama Iroha 1
> to dojrzały projekt z realnymi wdrożeniami (np. system płatności
> Bakong prowadzony przez Narodowy Bank Kambodży).

Reszta tego README opisuje projekt upstream i pozostaje aktualna.

## Czym jest Hyperledger Iroha?

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/960/badge)](https://bestpractices.coreinfrastructure.org/projects/960)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/baziorek/iroha1-cpp/badge.svg?branch=iroha1-main)](https://scorecard.dev/viewer/?uri=github.com/baziorek/iroha1-cpp)

Iroha to prosta technologia rozproszonych rejestrów (DLT), inspirowana
japońską zasadą Kaizen — eliminacją nadmiarowości (muri).
Iroha oferuje niezbędną funkcjonalność do zarządzania Twoimi aktywami,
informacjami i tożsamościami, będąc jednocześnie wydajnym i godnym
zaufania narzędziem odpornym na awarie (crash fault-tolerant) dla
potrzeb Twojego przedsiębiorstwa.

Zobacz stronę [przeglądu](https://baziorek.github.io/iroha1-cpp/) naszej
dokumentacji.
[Tutaj](https://www.youtube.com/channel/UCYlK9OrZo9hvNYFuf0vrwww) znajdziesz
kanał YouTube, na którym publikujemy spotkania i filmy objaśniające —
zajrzyj!

<img height="300px" src="../../../docs/image_assets/Iroha_3_sm.png"
 alt="Logo Irohy" title="Iroha" align="right" />

Iroha ma następujące funkcje:
1. Tworzenie i zarządzanie niestandardowymi aktywami fungowalnymi, takimi jak waluty, kilogramy złota itp.
2. Zarządzanie kontami użytkowników
3. Taksonomia kont oparta na _domenach_ w systemie
4. System praw i weryfikacji uprawnień użytkowników do wykonywania transakcji i zapytań w systemie
5. Walidacja reguł biznesowych dla transakcji i zapytań w systemie
6. Transakcje wielopodpisowe

Iroha jest _odporna na awarie_ (Crash Fault Tolerant) i ma własny
algorytm konsensusu — [YAC](https://arxiv.org/pdf/1809.00554.pdf).
W Iroha 2 został on zastąpiony nowym algorytmem —
[Sumeragi](https://iroha.tech/#sumeragi).

## Dokumentacja

Dokumentacja tego forka jest budowana za pomocą Sphinx i publikowana na
**GitHub Pages**: <https://baziorek.github.io/iroha1-cpp/> — po angielsku
oraz po polsku, rosyjsku, japońsku i turecku (przełącznik języka
na pasku bocznym dokumentacji i w tym README).

#### Mamy tłumaczenia dokumentacji!

Ten fork zawiera pełne tłumaczenia na **polski, rosyjski, japoński i
turecki**, utrzymywane w `docs/i18n/` (własne
[repozytorium lokalizacji](https://github.com/hyperledger/iroha-docs-l10n)
upstream obejmuje starsze migawki — śmiało pomagaj tłumaczyć na swój
język przez pull requesty).

### Jak szybko poznać Irohę?

Zobacz sekcję [getting started](https://baziorek.github.io/iroha1-cpp/getting_started/index.html)
dokumentacji, aby zacząć eksplorować system.
Stary sandbox Katacoda już nie działa, ale
[testy integracyjne](../../../test/) i
[przykłady `iroha-lib`](../../../iroha-lib/) pokazują działający węzeł
w akcji.

### Jak zbudować Irohę?

Skorzystaj z [przewodnika budowania](https://baziorek.github.io/iroha1-cpp/build/index.html),
który pomoże, jeśli chcesz zmodyfikować kod i kontrybuować.

### Czy jest dostępne SDK?

Tak — dla tego forka: nowoczesna **biblioteka klienta C++** w
[`iroha-lib/`](../../../iroha-lib/) (to repozytorium) oraz utrzymywana
biblioteka **Pythona** pod adresem
[baziorek/iroha1-python](https://github.com/baziorek/iroha1-python).
Upstream ma też SDK w
[Javie](https://github.com/hyperledger/iroha-java),
[Pythonie](https://github.com/hyperledger/iroha-python),
[JavaScript](https://github.com/hyperledger/iroha-javascript) i
[iOS](https://github.com/hyperledger/iroha-ios) (nieutrzymywane, zachowane
w celach referencyjnych).

### Czy są przykładowe aplikacje?

[Aplikacja punktowa na Androida](https://github.com/hyperledger/iroha-android/tree/master/iroha-android-sample)
i [portfel JavaScript](https://github.com/soramitsu/iroha-wallet-js).

Nie zapomnij też o [The Borsello App](https://github.com/claudiocandio/borsello) —
to portfel na Androida i iOS wraz z aplikacją webową, wniesiony przez
[Claudio](https://github.com/claudiocandio). Jest oparty na JS Wallet,
ale aktualny.

### Świetne narzędzia dla Irohy

[Iroha Explorer](https://codeberg.org/diva.exchange/iroha-explorer)
[Kontener Docker Iroha z Postgres 10](https://hub.docker.com/r/divax/iroha)
i jego [kod źródłowy](https://codeberg.org/diva.exchange/iroha)
[Narzędzie do wdrażania instancji Irohy](https://github.com/kuvaldini/iroha-swarm)

### Chcesz pomóc rozwijać Irohę?

Świetnie!
Zobacz [CONTRIBUTING.rst](../../../CONTRIBUTING.rst) — zgłoszenia problemów
i propozycje ulepszeń są mile widziane przez
[GitHub Issues](https://github.com/baziorek/iroha1-cpp/issues)
tego repozytorium.

## Potrzebujesz pomocy?

* Dołącz do [czatu Telegram](https://t.me/hyperledgeriroha) lub
[Hyperledger RocketChat](https://chat.hyperledger.org/channel/iroha),
gdzie opiekunowie, współtwórcy i użytkownicy chętnie pomogą.
Możesz też omawiać swoje uwagi i propozycje oraz po prostu rozmawiać
o Iroha tam lub na Gitterze
[![Dołącz do czatu na https://gitter.im/hyperledger-iroha/Lobby](https://badges.gitter.im/hyperledger-iroha/Lobby.svg)](https://gitter.im/hyperledger-iroha/Lobby)
* Zgłaszaj problemy i sugestie ulepszeń przez
[GitHub Issues](https://github.com/baziorek/iroha1-cpp/issues) —
w sprawach Irohy 2 upstream zobacz
[Hyperledger Jira](https://jira.hyperledger.org/secure/CreateIssue!default.jspa)
* Zasubskrybuj naszą [listę mailingową](https://lists.hyperledger.org/g/iroha),
aby otrzymywać najnowsze i najważniejsze wiadomości oraz dzielić się
swoją wiadomością w społeczności Irohy

## Licencja

Kod Irohy jest licencjonowany na licencji Apache,
w wersji 2.0 („Licencja"); nie możesz używać tego pliku inaczej
niż zgodnie z Licencją. Kopię Licencji możesz uzyskać pod adresem
http://www.apache.org/licenses/LICENSE-2.0

O ile nie jest to wymagane przez obowiązujące prawo lub uzgodnione
na piśmie, oprogramowanie rozprowadzane na licencji jest rozprowadzane
NA ZASADZIE „TAK JEST", BEZ RĘKOJMI I ZWAROKÓW JAKIEKOLWIEK, wyraźnych
lub dorozumianych. Zobacz Licencję, aby poznać szczególne postanowienia
dotyczące uprawnień i ograniczeń.

Pliki dokumentacji Irohy są udostępniane na licencji Creative Commons
Attribution 4.0 International License (CC-BY-4.0), dostępnej pod adresem
http://creativecommons.org/licenses/by/4.0/
