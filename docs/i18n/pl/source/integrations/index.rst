.. _integrations:

=====================
Zintegrowane projekty
=====================

Jedną z idei Hyperledger Consortium jest tworzenie rozwiązań,
które mogą ze sobą współpracować, aby zapewnić najlepsze możliwe
doświadczenie blockchainowe. W Iroha wierzymy, że integracja z
innymi świetnymi narzędziami i rozwiązaniami Hyperledger to sposób
na ulepszenie Irohy dla Twoich przypadków użycia. Dlatego
pracowaliśmy nad integracjami z kilkoma projektami i chcemy
opowiedzieć więcej o tym, z czym Iroha może współpracować.

.. note::
	Wszystkie poniższe integracje są **zarchiwizowane**:
	projekty partnerskie osiągnęły End of Life lub wycofały
	wsparcie dla Irohy 1. Informacje zachowano dla celów
	historycznych i dla osób utrzymujących starsze wdrożenia.

Zarchiwizowane integracje
=========================

Następujące integracje zostały opracowane dla Irohy 1, ale nie są
już utrzymywane. Tam, gdzie to istotne, podano daty oficjalnych
decyzji End of Life podjętych przez Hyperledger TSC/TOC.

.. _hyperledger-cacti:

Hyperledger Cacti (dawniej Cactus) — konektor Irohy usunięty w Cacti 2.0 (2024)
-------------------------------------------------------------------------------

`Hyperledger Cacti <https://github.com/hyperledger-cacti/cacti>`_
(dawniej Hyperledger Cactus) to platforma interoperability,
która pozwala różnym blockchainom i rozproszonym ledgerom
komunikować się ze sobą przez wspólne API, dzięki czemu aplikacja
może pracować z wieloma ledgerami bez dedykowanych integracji.

Iroha miała dedykowany konektor Cactus, opisany na stronach wiki
`Integracja Iroha i Cactus
<https://lf-hyperledger.atlassian.net/wiki/spaces/INTERN/pages/21954723/HL+Iroha+and+HL+Cactus+Integration>`_
i `Rozszerzanie istniejącej integracji Iroha–Cactus
<https://lf-hyperledger.atlassian.net/wiki/spaces/INTERN/pages/21954791/Extend+existing+Iroha+-+Cactus+integration>`_.

.. note::
	Cacti samo w sobie jest nadal aktywnym projektem LF
	Decentralized Trust, ale **konektor Irohy 1 został usunięty
	w Cacti 2.0 (2024)**, a konektor Irohy 2 również jest
	wycofywany. Poniższe materiały mają więc znaczenie historyczne
	dla wdrożeń Irohy 1.

.. _hyperledger-ursa:

Hyperledger Ursa — EOL 27 kwietnia 2023
----------------------------------------

`Hyperledger Ursa
<https://wiki.hyperledger.org/display/ursa/Hyperledger+Ursa>`_ to
współdzielona biblioteka kryptograficzna, która pozwala
osobom (i projektom) unikać duplikowania pracy kryptograficznej i
— jak to jest celem — zwiększać bezpieczeństwo. Biblioteka jest
repozytorium opt-in dla projektów (i potencjalnie innych), aby
umieszczać i używać kryptografii. Hyperledger Ursa składa się z
podprojektów — spójnych implementacji kodu kryptograficznego lub
interfejsów do niego.

Możesz łatwo zbudować Irohę z biblioteką Ursa, dodając tylko
`jedną flagę podczas builda
<../build/index.html#main-parameters>`_. Pozwoli to używać
algorytmów kryptograficznych z biblioteki Ursa zamiast standardowej
kryptografii Irohy. Wraz z rozwojem nowych bibliotek w Ursa coraz
więcej opcji będzie Ci dostępnych!

.. note::
	Obecnie pobieramy z Ursa tylko algorytm ed25519 SHA-2. Jeśli
	chcesz, możesz przyczynić się do kodu, aby dodać więcej opcji.

Aby umożliwić używanie zarówno domyślnego algorytmu
ed25519/sha3, jak i tych z Ursa, używamy formatu klucza publicznego
Multihash dla tych ostatnich. Możesz dowiedzieć się więcej o
`kluczach <../develop/keys.html>`_.

.. _hyperledger-explorer:

Hyperledger Explorer — EOL 12 maja 2022
---------------------------------------

`Hyperledger Explorer
<https://wiki.hyperledger.org/display/explorer>`_ to moduł
blockchain i jeden z projektów Hyperledger prowadzonych przez The
Linux Foundation. Zaprojektowany do tworzenia przyjaznej dla
użytkownika aplikacji webowej, Hyperledger Explorer może
przeglądać, wywoływać, wdrażać lub odpytywać bloki, transakcje i
powiązane dane, informacje o sieci (nazwa, status, lista węzłów),
kody łańcuchowe i rodziny transakcji, a także wszelkie inne
istotne informacje przechowywane w ledgerze.

`Tutaj
<https://github.com/turuslan/blockchain-explorer/blob/iroha-explorer-integration/iroha-explorer-integration.md>`_
możesz dowiedzieć się, jak używać Explorera z Irohą.

.. _hyperledger-burrow:

Hyperledger Burrow — EOL 12 maja 2022
--------------------------------------

`Hyperledger Burrow
<https://wiki.hyperledger.org/display/burrow>`_ zapewnia
modułowy klient blockchain z interpreterem permissioned smart
contractów częściowo rozwiniętym według specyfikacji Ethereum
Virtual Machine (EVM).

Tak więc z HL Burrow możesz używać kontraktów Solidity na Iroha.
Kliknij poniżej, aby dowiedzieć się więcej.

.. toctree::
    :maxdepth: 2

    burrow.rst
    burrow_example.rst

.. _hyperledger-quilt:

Hyperledger Quilt — EOL 2 czerwca 2022
---------------------------------------

`Hyperledger Quilt
<https://lf-hyperledger.atlassian.net/wiki/spaces/INTERN/pages/21954679/HL+Iroha+and+HL+Quilt+Integration>`_
to framework interoperability, który pozwalał Iroha współpracować
z innymi ledgerami przez wspólny model transakcji i zapytań.
Projekt został przeniesiony do stanu Dormant 23 czerwca 2021 i
osiągnął End of Life 2 czerwca 2022; repozytorium jest zarchiwizowane.
Linkowana strona wiki dokumentuje pierwotną pracę nad integracją
Iroha–Quilt.
