===
FAQ
===

Jestem nowy. Od czego zacząć?
-----------------------------

Witaj, nowicjuszu! Serdecznie witamy :)
Są 2 sposoby, aby zacząć z Iroha:

 1. Możesz zobaczyć, czym jest Iroha i jak działa, budując prostą
    przykładową sieć według naszego `Przewodnika szybkiego startu
    <../getting_started/index.html>`_
 2. Możesz zapoznać się z `podstawowymi pojęciami Irohy
    <../concepts_architecture/index.html>`_ i zacząć budować własną
    `sieć Irohy <../deploy/index.html>`_

Masz już swój blockchain Irohy! Gratulacje!
Jeśli masz jakiekolwiek pytania, nie wahaj się skontaktować z naszą
społecznością tutaj: https://chat.hyperledger.org/channel/iroha

Jakie typy danych można przesyłać?
----------------------------------

Hyperledger Iroha pozwala wysyłać nie tylko aktywa (możesz odnieść
takie wrażenie z powodu rozbudowanego zestawu komend i zapytań
obsługujących takie aktywa), ale dowolne dane, które zostaną
zapisane w łańcuchu.

Obecna implementacja zapewnia tę możliwość przynajmniej przez komendę
`SetAccountDetail <../develop/api/commands.html#set-account-detail>`_
i zapytanie `GetAccountDetail
<../develop/api/queries.html#get-account-detail>`_.

Czy urządzenie mobilne może być węzłem?
---------------------------------------

Są dwie opcje w zależności od tego, co rozumiesz przez urządzenie
mobilne.

Jeśli mówimy o sprzęcie opartym na ARM z jakimś Linuksem na pokładzie
(jak Raspberry PI) lub zrootowanym urządzeniu Android, to możliwe
jest uruchomienie Irohy jako węzła (peera w sieci) na tym urządzeniu.
Wtedy Iroha będzie działać jako natywny binarny plik platformy.

Jeśli mówimy o standardowym urządzeniu iOS lub Android z nietkniętym
fabrycznym shell-em (GUI), to generalnie nie jest to możliwe i nie
udostępniamy instrukcji w tym zakresie. Nadal możesz jednak tworzyć
aplikacje mobilne korzystające z Irohy — będą to klienci Irohy i nie
będą pełnić roli peerów.

Aby uruchomić Irohę na urządzeniu ARM, musisz ją zbudować na
platformie docelowej. Budowa Irohy wymaga znacznej ilości RAM — dla
32-bitowego hosta ARM potrzebujesz 8 GB RAM. Budowę można wykonać
wewnątrz kontenera Docker. Aby przygotować kontener, musisz:

1. Sklonować repozytorium git Irohy: https://github.com/baziorek/iroha1-cpp
2. Wykonać `docker build -t iroha-build-env .` w `iroha1-cpp/docker/develop`
3. Uruchomić nowo zbudowany kontener i zbudować w nim samą Irohę

Nie zapomnij zamontować folderu z repozytorium git Irohy do kontenera.

Jaka jest przepustowość (TPS)? Czy są wyniki testów wydajności?
----------------------------------------------------------------

Wydajność Twojej sieci Iroha będzie zależeć od konfiguracji,
sprzętu i liczby węzłów. Zachęcamy do wypróbowania testu obciążenia
w katalogu `test/load` i zgłoszenia wyników.

Jaka jest różnica między Iroha a Iroha 2?
-----------------------------------------

Iroha 2 to przepisanie Irohy w Rust z kilkoma istotnymi zmianami
mechanizmu konsensusu. `Dokumentacja Iroha 2
<https://hyperledger.github.io/iroha-2-docs/guide/iroha-2.html>`_
oferuje podsumowanie różnic między tymi dwoma projektami.
