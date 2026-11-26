***********
Uprawnienia
***********

Hyperledger Iroha używa systemu kontroli dostępu opartego na
rolach, aby ograniczać działania swoich użytkowników.
Ten system bardzo pomaga we wdrażaniu przypadków użycia
obejmujących grupy użytkowników o różnych poziomach dostępu — od
słabych użytkowników, którzy nie mogą nawet otrzymać transferu
aktywów, po super-użytkowników.
Piękno naszego systemu uprawnień polega na tym, że nie musisz
mieć super-użytkownika w swoim wdrożeniu Irohy ani używać
wszystkich możliwych uprawnień: możesz tworzyć wydzielone i
lekkie role.

Utrzymanie systemu polega na konfigurowaniu ról i uprawnień
zawartych w rolach. Może to być zrobione na początkowym etapie
wdrożenia systemu — w bloku genesis — lub później, gdy sieć
Iroha już działa, role mogą być zmieniane (jeśli jest rola, która
może to zrobić :).

Ta sekcja pomoże Ci zrozumieć uprawnienia i da pomysł, jak
utworzyć role zawierające określone uprawnienia.
Każde uprawnienie jest opatrzone przykładem napisanym w Pythonie,
który demonstruje sposób tworzenia transakcji lub zapytania
wymagającego konkretnego uprawnienia. Każdy przykład używa
modułu *commons.py*, którego lista jest dostępna w sekcji
`Supplementary Sources`_.

*******************
Lista uprawnień
*******************

.. iroha_gen_permissions_index:: permissions/matrix.csv

********************
Szczegóły uprawnień
********************

.. iroha_gen_detailed_permissions:: permissions/matrix.csv
