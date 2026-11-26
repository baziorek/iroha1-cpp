.. contents:: **Spis treści**
  :depth: 3

Przede wszystkim dziękujemy za poświęcenie czasu na wniesienie wkładu!

Poniżej znajduje się krótki zestaw wytycznych dotyczących współtworzenia
Iroha.

Jak możesz się przyczynić?
--------------------------

Tłumaczenie dokumentacji
~~~~~~~~~~~~~~~~~~~~~~~~

Przetłumaczone wersje tej dokumentacji są hostowane obok wersji
angielskiej pod adresem ``/<język>/``. Obecnie dostępne:

* **Türkçe** — `turecka dokumentacja <../tr/>`_ (przetłumaczona na
  podstawie zarchiwizowanego repozytorium :strikelink:`hyperledger-iroha/iroha-docs-l10n
  <https://github.com/hyperledger-iroha/iroha-docs-l10n>`; tłumaczenie
  obejmuje dokumentację z okresu Iroha 1.2 i może być nieaktualne
  względem wersji angielskiej)

Planowane są kolejne języki — w tym polski. Jeśli chcesz
współtworzyć tłumaczenie, skontaktuj się z nami przez
`Discord <https://discord.com/channels/905194001349627914/905205848547155968>`_.

Zgłaszanie błędów
~~~~~~~~~~~~~~~~~

*Błąd* to błąd (w sensie usterki), wada projektowa, awaria lub usterka w
Iroha, która powoduje nieprawidłowy lub nieoczekiwany wynik albo
niezamierzone zachowanie.

Błędy są śledzone jako `issues w GitHub <https://github.com/hyperledger-iroha/iroha/issues>`_.

Aby zgłosić błąd, `utwórz nowy issue <https://github.com/hyperledger-iroha/iroha/issues/new>`_ i
uwzględnij w szablonie issue następujące informacje: podsumowanie problemu,
kroki reprodukcji oraz wszelkie istotne logi. Podaj proszę również dane o
swoim środowisku (system operacyjny, specyfikacja urządzenia, wersja Iroha
itp.).

Zgłaszanie luk w bezpieczeństwie
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Chociaż staramy się zapobiegać problemom bezpieczeństwa, nie zakładamy,
że nigdy się nie pojawią.

Standardową praktyką jest odpowiedzialne i prywatne zgłoszenie problemu
bezpieczeństwa dostawcy (organizacji Hyperledger) przed upublicznieniem,
aby można było przygotować poprawkę i zminimalizować szkody spowodowane
luka.

Przed pierwszym głównym wydaniem (1.0) wszystkie luki w bezpieczeństwie
są traktowane jak błędy, więc śmiało zgłaszaj je w sposób opisany powyżej.
Po pierwszym głównym wydaniu skorzystaj z `programu bug bounty
tutaj <https://hackerone.com/hyperledger>`__, aby zgłaszać
luki w bezpieczeństwie i otrzymać nagrodę.

W każdym przypadku — skontaktuj się z dowolnym z obecnych opiekunów
w wiadomościach prywatnych na Discordzie lub e-mailem (sprawdź plik
CONTRIBUTORS.md), jeśli chcesz omówić, czy Twoje odkrycie jest luką
w bezpieczeństwie, czy błędem.

Propozycje ulepszeń
~~~~~~~~~~~~~~~~~~~~

*Ulepszenie* to kod lub pomysł, który sprawia, że **istniejący** kod lub
projekt jest szybszy, stabilniejszy, przenośny, bezpieczniejszy albo
lepszy w jakikolwiek inny sposób.

Ulepszenia są śledzone jako `issues w GitHub <https://github.com/hyperledger-iroha/iroha/issues>`_.

Aby zgłosić nowe ulepszenie, `utwórz nowy issue <https://github.com/hyperledger-iroha/iroha/issues/new>`_ i
opisz pomysł: co ulepsza i dlaczego jest wartościowy. Jeśli masz
jakieś sugestie kodowe, zachęcamy do ich dołączenia.

Zadawanie pytań
~~~~~~~~~~~~~~~

*Pytanie* to dowolna dyskusja, która zwykle nie jest ani błędem, ani
propozycją funkcji lub ulepszeniem. Jeśli masz pytanie w stylu „Jak
zrobić X?” — ten akapit jest dla Ciebie.

Opublikuj swoje pytanie na `swoim ulubionym
komunikatorze <Gdzie społeczność jest aktywna_>`__, aby członkowie
społeczności mogli Ci pomóc. Możesz też pomagać innym!

Twój pierwszy wkład w kod
~~~~~~~~~~~~~~~~~~~~~~~~~

Przeczytaj nasz `Przewodnik stylu C++ <Przewodnik stylu C++_>`__ i zacznij od sprawdzenia `tablicy GitHub <https://github.com/hyperledger-iroha/iroha/projects/1>`_ lub issue z
etykietą `good-first-issue <https://github.com/hyperledger-iroha/iroha/issues?q=is%3Aissue+is%3Aopen+label%3Agood-first-issue>`_.
W jakiś sposób daj znać, że pracujesz nad tym zadaniem: skontaktuj się
z zespołem opiekunów, społecznością lub po prostu przypisz issue do
siebie.

Pull requesty
~~~~~~~~~~~~~

-  Wypełnij `wymagany szablon <https://github.com/hyperledger-iroha/iroha/blob/iroha1-main/.github/PULL_REQUEST_TEMPLATE.md>`_

-  Zakończ wszystkie pliki znakiem nowej linii

-  **Napisz testy** dla nowego kodu. Pokrycie testami nowego kodu musi
   wynosić co najmniej 70%

-  Każdy pull request powinien być zrecenzowany i **otrzymać co najmniej
   dwie aprobaty od zespołu opiekunów**. Sprawdź, kto jest obecnym
   opiekunem, w pliku
   `MAINTAINERS.md <https://github.com/hyperledger-iroha/iroha/blob/iroha1-main/MAINTAINERS.md>`_

-  Gdy skończysz pracę, upewnij się, że wszystkie kontrole CI
   przechodzą — po tym **squash and merge** Twój pull request

-  Przestrzegaj `Przewodnika stylu C++ <Przewodnik stylu C++_>`_

-  Przestrzegaj `Przewodnika stylu Git <Przewodnik stylu Git_>`_

-  **Dokumentuj nowy kod** na podstawie
   `Przewodnika dokumentacji <Przewodnik dokumentacji_>`__

-  Pracując z **PR-ami z forków** sprawdź `tę
   instrukcję <https://help.github.com/articles/checking-out-pull-requests-locally>`_

Przewodniki stylu
-----------------

Przewodnik stylu Git
~~~~~~~~~~~~~~~~~~~~

-  **Podpisuj każdy commit** za pomocą `DCO <https://github.com/apps/dco>`_:
   ``Signed-off-by: $NAME <$EMAIL>``. Możesz to zrobić automatycznie
   używając ``git commit -s``
-  **Używaj czasu teraźniejszego** („Add feature”, a nie „Added
   feature”).
-  **Używaj trybu rozkazującego** („Deploy docker to...” a nie
   „Deploys docker to...”).
-  Pisz znaczące komunikaty commita.
-  Ogranicz pierwszą linię komunikatu commita do 50 znaków lub mniej
-  Pierwsza linia komunikatu commita musi zawierać podsumowanie
   wykonanej pracy, druga linia musi być pusta, trzecia i kolejne linie
   mogą zawierać listę zmian w commicie
-  Przy scalaniu do docelowej gałęzi: użyj **Squash and merge**

Przewodnik stylu C++
~~~~~~~~~~~~~~~~~~~~

-  Używaj pliku z
   `ustawieniami <https://github.com/hyperledger-iroha/iroha/blob/iroha1-main/.clang-format>`_
   clang-format. W internecie dostępne są poradniki (np. `wiki
   Kratosa <https://github.com/KratosMultiphysics/Kratos/wiki/How-to-configure-clang%E2%80%90format>`_)
-  Przestrzegaj
   `CppCoreGuidelines <http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines>`_
   oraz `Cpp Best
   Practices <https://lefticus.gitbooks.io/cpp-best-practices>`_.
-  Unikaj kodu
   `zależnego od platformy <https://stackoverflow.com/questions/1558194/learning-and-cross-platform-development-c>`_.
-  Używaj `C++23 <https://en.wikipedia.org/wiki/C%2B%2B23>`_.
-  Używaj `camelCase <https://en.wikipedia.org/wiki/Camel_case>`_ dla
   nazw klas i metod, używaj
   `snake\_case <https://en.wikipedia.org/wiki/Snake_case>`_ dla
   zmiennych.

Przewodnik dokumentacji
~~~~~~~~~~~~~~~~~~~~~~~

-  Używaj
   `Doxygen <http://www.doxygen.nl/>`_.
-  Dokumentuj całe publiczne API: metody, funkcje, pola, szablony,
   klasy...

Gdzie społeczność jest aktywna
------------------------------

Członkowie naszej społeczności są aktywni w:

+------------------+-------------------------------------------------------------------------------------------------------+
| Usługa           | Link                                                                                                  |
+------------------+-------------------------------------------------------------------------------------------------------+
| Discord          | https://discord.com/channels/905194001349627914/905205848547155968 **(zalecane)**                     |
+------------------+-------------------------------------------------------------------------------------------------------+
| RocketChat       | :strikelink:`Rocket.Chat <https://chat.hyperledger.org/channel/iroha>`                                |
+------------------+-------------------------------------------------------------------------------------------------------+
| Wiki (Iroha 1)   | :strikelink:`Iroha 1 <https://lf-hyperledger.atlassian.net/wiki/spaces/iroha/pages/21015959/Iroha+1>` |
+------------------+-------------------------------------------------------------------------------------------------------+
| StackOverflow    | https://stackoverflow.com/questions/tagged/hyperledger-iroha                                          |
+------------------+-------------------------------------------------------------------------------------------------------+
| Lista mailingowa | hyperledger-iroha@lists.hyperledger.org                                                               |
+------------------+-------------------------------------------------------------------------------------------------------+
| Gitter           | https://gitter.im/hyperledger-iroha/Lobby (nieaktywny od 2023)                                        |
+------------------+-------------------------------------------------------------------------------------------------------+
| Telegram         | https://t.me/hl\_iroha                                                                                |
+------------------+-------------------------------------------------------------------------------------------------------+
| YouTube          | https://www.youtube.com/channel/UCYlK9OrZo9hvNYFuf0vrwww                                              |
+------------------+-------------------------------------------------------------------------------------------------------+


--------------

Dziękujemy za przeczytanie tego dokumentu!
