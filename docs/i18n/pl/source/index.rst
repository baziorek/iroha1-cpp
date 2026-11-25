********************************
Dokumentacja Hyperledger Iroha
********************************

.. image:: ../image_assets/iroha_logo.png

.. warning::
    Należy pamiętać, że Hyperledger Iroha v1 jest **już nie rozwijana**;
    wsparcie dla Iroha v1 jest ograniczone. Główny zespół programistów
    pracuje nad `Hyperledger Iroha v2 <https://github.com/hyperledger/iroha/tree/iroha2-dev#hyperledger-iroha>`_,
    kompletnym przepisaniem Irohy w Rust.

    Do nowych projektów **zalecamy użycie Iroha v2 zamiast Iroha v1**.
    Jeśli używasz Iroha v1, zachęcamy do zaplanowania migracji na Iroha v2.

    Wersja Iroha v2 z długoterminowym wsparciem (`iroha2-lts`) jest
    najlepszym wyborem, jeśli chcesz używać Irohy w produkcji i otrzymywać
    wsparcie techniczne z naszej strony. Niestety, nie możemy zaoferować
    takiego samego wsparcia dla Iroha v1, więc rozważ użycie Iroha v2.

    **Iroha v1 i Iroha v2 są ze sobą niekompatybilne**. Różnice opisano
    w `dokumentacji Iroha 2 <https://hyperledger.github.io/iroha-2-docs/guide/iroha-2.html>`_.

Witaj! Hyperledger Iroha 1 to prosta platforma blockchain, której możesz
użyć do budowania zaufanych, bezpiecznych i szybkich aplikacji —
dzięki blockchainowi opartej na uprawnieniach i konsensusie
tolerancyjnym na awarie (Crash fault-tolerant). Jest darmowa,
open-source i działa na Linuxie i macOS, z dostępnymi bibliotekami
mobilnymi i desktopowymi.

Kod źródłowy Hyperledger Iroha 1 i najnowsze wydania pobierzesz ze strony
`forka społeczności iroha1-cpp <https://github.com/baziorek/iroha1-cpp>`_
(w `wydaniach <https://github.com/baziorek/iroha1-cpp/releases>`_ znajdziesz
prekompilowane binaria, pakiety ``.deb`` i AppImage; źródła C++ są
dostępne także na gałęzi ``iroha1-main`` upstreamowego
`hyperledger-iroha/iroha <https://github.com/hyperledger-iroha/iroha/tree/iroha1-main>`_).

Ta dokumentacja przeprowadzi Cię przez instalację, wdrożenie i uruchomienie
sieci Iroha oraz pokaże, jak napisać dla niej aplikację. Zobaczysz też,
jakie scenariusze zastosowań są obecnie wykonalne, a jakie planowane.

.. note::
    Czytasz dokumentację `forka społeczności iroha1-cpp
    <https://github.com/baziorek/iroha1-cpp>`_, który utrzymuje Iroha v1
    w stanie pozwalającym na budowanie. Jest ona **bardziej aktualna** niż
    oficjalna dokumentacja ReadTheDocs pod adresem
    `iroha.readthedocs.io <https://iroha.readthedocs.io/>`_, która nie jest
    już uaktualniana dla Iroha v1.

    Odpowiadający klient Python to fork `iroha1-python
    <https://github.com/baziorek/iroha1-python>`_ — oryginalne źródła
    ``iroha-python`` **bez zmian**, przebudowane na nowoczesnych
    zależnościach, dzięki czemu instalują się i działają na obecnych
    systemach.

.. note::
    Iroha v1 to dojrzała, potwierdzona w produkcji platforma. Obsługuje
    `Bakong <https://www.nbc.gov.kh/english/economic_research/bakong.php>`_,
    krajowy system płatności Narodowego Banku Kambodży, w produkcji od
    października 2020 — dziesiątki milionów kont i ponad miliard
    transakcji rocznie, z rozliczeniami międzybankowymi w czasie
    rzeczywistym. Technologia jest wykorzystywana także w projektach
    akademickich i badawczych.

.. warning::
    Forki ``iroha1-cpp`` i ``iroha1-python`` **nie zawierają zmian
    funkcjonalnych** w samej Iroha — jedynie aktualizacje systemu
    buildowania, odświeżenie zależności i prace porządkowe. Są
    utrzymywane przez jedną osobę (z pomocą AI), **a nie** przez
    oryginalny zespół, który projektował, testował i eksploatował system
    przez lata. Choć zmiany mają być bezpieczne, **nie udzielamy żadnej
    gwarancji produkcyjnej ani gwarancji przydatności komercyjnej**. Jeśli
    planujesz wdrożenie komercyjne, dokładnie oceń forka albo rozważ
    oficjalnie wspieraną `Hyperledger Iroha v2
    <https://github.com/hyperledger/iroha>`_.

Iroha jest projektem open-source, więc omówimy też zasady współpracy nad
kodem i proces zgłaszania zmian.

.. toctree::
    :maxdepth: 2
    :numbered:
    :caption: Spis treści

    overview.rst
    concepts_architecture/index.rst
    getting_started/index.rst
    integrations/index.rst
    build/index.rst
    configure/index.rst
    deploy/index.rst
    maintenance/index.rst
    develop/index.rst
    community/index.rst
    faq/index.rst
