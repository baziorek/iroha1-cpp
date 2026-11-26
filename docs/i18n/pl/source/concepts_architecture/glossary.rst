Konto
=====

Encja Irohy, która może wykonywać określony zestaw działań.
Każde konto należy do jednego z istniejących `domen <Domena_>`__.

Konto ma pewną liczbę `ról <Rola_>`__ (może być null) — które są
kolekcją uprawnień. Tylko `uprawnienia nadawalne
<Uprawnienie nadawalne_>`__ są przypisywane kontu bezpośrednio.


Aktywo
======

Dowolny policzalny towar lub wartość.
Każde aktywo jest powiązane z jedną z istniejących `domen
<Domena_>`__. Na przykład aktywo może reprezentować dowolny
rodzaj jednostek — jednostkę waluty, sztabkę złota, jednostkę
nieruchomości itp.

Blok
====

Dane transakcji są trwale rejestrowane w plikach zwanych
blokami. Bloki są organizowane w liniową sekwencję w czasie
(znaną również jako łańcuch bloków) [#f1]_.

Bloki są podpisywane kryptograficznymi podpisami peerów
Irohy, głosujących za ten blok podczas `konsensusu <Konsensus_>`__.
Podpisywalna zawartość nazywana jest ładunkiem (payload), więc
struktura bloku wygląda następująco:

.. image:: ../../image_assets/block.png
    :width: 80%
    :align: center
    :alt: Struktura bloku Iroha

*Poza ładunkiem*

    - podpisy — podpisy peerów, którzy głosowali za blok podczas rundy konsensusu

*Wewnątrz ładunku*

    - wysokość — liczba bloków w łańcuchu do tego bloku
    - znacznik czasu — czas Unix (w milisekundach) utworzenia bloku przez peera
    - tablica transakcji, które pomyślnie przeszły walidację i krok konsensusu
    - hash poprzedniego bloku w łańcuchu
    - hashe odrzuconych transakcji — tablica hashy transakcji,
      które nie przeszły stanowej walidacji; to pole jest opcjonalne


Klient
======

Każda aplikacja używająca Irohy jest traktowana jako klient.

Charakterystyczną cechą Irohy jest to, że wszyscy klienci
używają prostych abstrakcji klient-serwer podczas interakcji
z siecią peerów: nie używają żadnych abstrakcji specyficznych
dla systemów związanych z blockchain. Na przykład w Bitcoin
klienci muszą walidować bloki, a w HL Fabric muszą odpytywać
kilka peerów, aby upewnić się, że transakcja została zapisana
w bloku, podczas gdy w HL Iroha klient wchodzi w interakcję
z dowolnym peerem podobnie jak z pojedynczym serwerem.

Polecenie
=========

Polecenie to intencja zmiany `stanu <architecture.html#world-state-view>`__ sieci.
Na przykład, aby utworzyć nową `rolę <Rola_>`__ w Iroha,
musisz wydać polecenie `Create role <../develop/api/commands.html#create-role>`__.

Konsensus
=========

Algorytm konsensusu to proces w informatyce służący do
osiągnięcia zgody na pojedynczą wartość danych między
procesami lub systemami rozproszonymi. Algorytmy konsensusu
są projektowane w celu osiągnięcia niezawodności w sieci
obejmującej wiele niezawodnych węzłów. Rozwiązanie tego
problemu — znanego jako problem konsensusu — jest ważne
w obliczeniach rozproszonych i systemach wieloagentowych.

*Konsensus jako algorytm*

Algorytm osiągania zgody na blok między peerami w sieci.
Posiadając go w systemie, niezawodność jest zwiększona.

Aby zobaczyć konsensus jako komponent Irohy, sprawdź
`ten link <architecture.html#block-consensus-yac>`__;
aby uzyskać szczegółowy opis protokołu YAC zobacz
`dokumentację YAC <yac.html>`_.

Domena
======

Nazwana abstrakcja do grupowania `kont <Konto_>`__ i
`aktywów <Aktywo_>`__. Na przykład może reprezentować
organizację w grupie organizacji pracujących z Iroha.

Peer
====

Węzeł będący częścią sieci Iroha.
Uczestniczy w procesie `konsensusu <Konsensus_>`_.

Peer może być również zarejestrowany jako **peer synchronizujący**
— węzeł, który jedynie podąża za ledgerem (np. w celach
kopii zapasowych lub audytu) bez udziału w głosowaniu
konsensusu. Peery synchronizujące są dodawane regularnym
poleceniem `AddPeer <../develop/api/commands.html#add-peer>`__
z rekordem peera oznaczonym jako synchronizujący; to samo
uprawnienie ``can_add_peer`` ma zastosowanie. Otrzymują
stan konsensusu bezpośrednio, więc mogą dogonić, ale nie
proponują bloków ani nie głosują.

Uprawnienie
===========

Nazwana reguła nadająca przywilej wykonywania polecenia.
Uprawnienie **nie może** być nadane `kontu <Konto_>`__
bezpośrednio; zamiast tego konto ma role, które są
kolekcjami uprawnień. Chociaż istnieje wyjątek — zobacz
`Uprawnienie nadawalne <Uprawnienie nadawalne_>`__.

`Lista uprawnień Iroha <../develop/api/permissions.html>`_.

Uprawnienie nadawalne
---------------------

Tylko uprawnienie nadawalne jest dane `kontu <Konto_>`__
bezpośrednio. Konto posiadające uprawnienie nadawalne może
wykonywać pewne konkretne działanie w imieniu innego konta.
Na przykład, jeśli konto a@domain1 nadaje kontu b@domain2
uprawnienie do transferowania aktywów — wtedy b@domain2 może
transferować aktywa a@domain1 każdemu.

Propozycja
==========

Zbiór `transakcji <Transakcja_>`__, które przeszły jedynie
`walidację bezstanową <Walidacja bezstanowa_>`__.

Zweryfikowana propozycja
------------------------

Zbiór transakcji, które przeszły zarówno `walidację
bezstanową <Walidacja bezstanowa_>`__, jak i `stanową
<Walidacja stanowa_>`__, ale nie zostały jeszcze
zatwierdzone.

Zapytanie
=========

Żądanie do Irohy, które **nie** zmienia `stanu
<architecture.html#world-state-view>`__ sieci. Wykonując
zapytanie, klient może zażądać danych ze stanu, na przykład
salda swojego konta, historii transakcji itp.

Kworum
======

W kontekście podpisywania transakcji liczba kworum to
minimalna liczba podpisów wymagana, aby uznać transakcję za
podpisaną. Wartość domyślna to 1. Dla `transakcji MST
<Transakcje wielopodpisowe_>`__ będziesz musiał zwiększyć
tę liczbę.

Każde konto może dołączyć dodatkowe klucze publiczne i
zwiększyć własną liczbę kworum.

Rola
====

Nazwana abstrakcja przechowująca zbiór `uprawnień
<Uprawnienie_>`__.

Sygnatariusz
============

Reprezentuje encję, która może potwierdzać transakcje
wielopodpisowe dla `konta <Konto_>`__. Może być dołączony
do konta przez `AddSignatory <../develop/api/commands.html#add-signatory>`__
i odłączony przez `RemoveSignatory <../develop/api/commands.html#remove-signatory>`__.

Transakcja
==========

Uporządkowany zbiór `poleceń <Polecenie_>`__, który jest
stosowany do ledgera atomowo. Każde nieprawidłowe polecenie
w transakcji prowadzi do odrzucenia całej transakcji podczas
procesu walidacji.

Struktura transakcji
--------------------

**Ładunek (Payload)** przechowuje wszystkie pola transakcji,
oprócz podpisów:

    - Czas utworzenia (czas unix, w milisekundach)
    - ID konta twórcy transakcji (nazwa_użytkownika@domena)
    - Pole kworum (wskazuje wymaganą liczbę podpisów)
    - Powtarzane polecenia opisane szczegółowo w `sekcji
      poleceń <../develop/api/commands.html>`__
    - Informacje meta partii (część opcjonalna). Szczegóły
      w `Partia transakcji`_


**Podpisy** zawierają jeden lub wiele podpisów (klucz
publiczny ed25519 + podpis)

Zredukowany hash transakcji
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Zredukowany hash jest obliczany na podstawie ładunku
transakcji z wyłączeniem informacji meta partii. Używany w
`Partii transakcji <Partia transakcji_>`__.


Statusy transakcji
------------------

Hyperledger Iroha obsługuje zarówno tryb interakcji push,
jak i pull z klientem. Klient używający trybu pull żąda
aktualizacji statusu transakcji od peera Irohy, wysyłając
hashe transakcji i oczekując odpowiedzi. Przeciwnie,
interakcja push jest wykonywana przez nasłuchiwanie strumienia
zdarzeń dla każdej transakcji. W obu trybach zbiór statusów
transakcji jest taki sam:

 .. image:: ./../../image_assets/tx_status.png

.. note::
    Napisaliśmy świetny artykuł na Medium `o funkcji
    strumieniowania statusu w Iroha <https://medium.com/iroha-contributors/status-streaming-in-hl-iroha-5503487ffcfd>`_.
    Sprawdź go i daj nam znać, co o tym myślisz w komentarzach!

Zbiór statusów transakcji
^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Status
     - Znaczenie
   * - ``NOT_RECEIVED``
     - Żądany peer nie ma tej transakcji.
   * - ``ENOUGH_SIGNATURES_COLLECTED``
     - Transakcja wielopodpisowa ma wystarczająco podpisów
       i będzie walidowana.
   * - ``MST_PENDING``
     - Transakcja wielopodpisowa wciąż potrzebuje więcej
       podpisów, aby osiągnąć kworum.
   * - ``MST_EXPIRED``
     - Transakcja wielopodpisowa nie jest już ważna i zostanie
       usunięta.
   * - ``STATELESS_VALIDATION_FAILED``
     - Transakcja jest źle sformułowana lub podpisy są
       nieprawidłowe; powód jest zwracany.
   * - ``STATELESS_VALIDATION_SUCCESS``
     - Transakcja przeszła walidację bezstanową; zwracane
       zaraz po jej wysłaniu.
   * - ``STATEFUL_VALIDATION_FAILED``
     - Polecenie narusza reguły zależne od stanu (saldo,
       uprawnienia itp.); powód jest zwracany.
   * - ``STATEFUL_VALIDATION_SUCCESS``
     - Transakcja przeszła walidację stanową.
   * - ``COMMITTED``
     - Transakcja jest częścią bloku, który uzyskał
       wystarczającą liczbę głosów i znajduje się w block
       store.
   * - ``REJECTED``
     - Transakcja została odrzucona podczas walidacji stanowej
       w poprzedniej rundzie konsensusu; jej hash jest
       przechowywany w block store `bloku <Blok_>`__ w celu
       zapobiegania atakom powtórkowym.

Transakcje oczekujące
^^^^^^^^^^^^^^^^^^^^^

Każda transakcja, która w danym momencie ma mniej podpisów
niż `kworum`_ konta twórcy transakcji, jest uznawana za
oczekującą. Oczekująca transakcja zostanie przesłana do
`walidacji stanowej <Walidacja stanowa_>`__, gdy tylko mechanizm
`wielopodpisowy <Transakcje wielopodpisowe_>`__ zbierze
wymaganą liczbę podpisów dla kworum.

Transakcja, która już ma kworum podpisów, również może być
uznawana za oczekującą w przypadkach, gdy transakcja jest
częścią `partii transakcji <Partia transakcji_>`__ i istnieje nie w pełni
podpisana transakcja.

Partia transakcji
=================

Partia transakcji to funkcja pozwalająca wysyłać kilka
transakcji do Irohy jednocześnie, zachowując ich kolejność.

Każda transakcja w partii zawiera informacje meta partii.
Meta partii zawiera identyfikator typu partii (atomowa lub
uporządkowana) i listę `zredukowanych hashy
<Zredukowany hash transakcji_>`_ wszystkich transakcji w partii.
Kolejność hashy definiuje sekwencję transakcji.

Partia może zawierać transakcje utworzone przez różne konta.
Każda transakcja w partii może wymagać jednego lub
`wielu <Transakcje wielopodpisowe_>`__ podpisów (zależy
od kworum ustawionego dla konta twórcy transakcji).
Co najmniej jedna transakcja w partii powinna mieć co
najmniej jeden podpis, aby partia przeszła `walidację
bezstanową <Walidacja bezstanowa_>`__.

Możesz przeczytać artykuł o partiach na naszej stronie
Contributors na `Medium <https://medium.com/iroha-contributors/batches-in-iroha-117614cf1e88>`__.

Partia atomowa
--------------

Wszystkie transakcje w partii atomowej powinny przejść
`walidację stanową <Walidacja stanowa_>`__, aby partia została zastosowana do
ledgera.

Gdy partia atomowa nie przechodzi walidacji stanowej, każda
transakcja w partii otrzymuje własne aktualizacje statusu,
więc transakcję, która zawiodła, można zidentyfikować ze
strumienia statusu:

- Transakcja zawierająca nieudane polecenie otrzymuje
  ``STATEFUL_VALIDATION_FAILED`` z niepustym
  ``err_or_cmd_name`` (nazwa nieudanego polecenia),
  specyficznym dla polecenia ``error_code`` (znaczenie
  kodów zobacz w `sekcji poleceń
  <../develop/api/commands.html>`__) i ``failed_cmd_index``
  ustawionym na pozycję nieudanego polecenia **w tej
  transakcji** (nie w partii).
- Każda inna transakcja w partii otrzymuje
  ``STATEFUL_VALIDATION_FAILED`` z pustym ``err_or_cmd_name``,
  ``error_code`` ``1`` i ``failed_cmd_index`` równym
  ``18446744073709551615`` (``SIZE_MAX``, oznaczający "brak
  konkretnego polecenia"), a następnie ``REJECTED`` po
  zatwierdzeniu partii do bloku.

Aby dowiedzieć się, która transakcja zepsuła partię,
szukaj aktualizacji statusu, której ``err_or_cmd_name``
nie jest puste; ``failed_cmd_index`` mówi wtedy, które
polecenie w tej transakcji zawiodło.

Partia uporządkowana
--------------------

Partia uporządkowana zachowuje jedynie sekwencję transakcji
stosowanych do ledgera. Wszystkie transakcje, które mogą
przejść walidację stanową w partii, zostaną zastosowane do
ledgera. Niepowodzenie walidacji jednej transakcji NIE
implikuje bezpośrednio niepowodzenia całej partii.

Transakcje wielopodpisowe
=========================

Transakcja, której `kworum`_ jest większe niż jeden, jest
uznawana za wielopodpisową (nazywaną też mst). Aby osiągnąć
`ważność stanową <Walidacja stanowa_>`__, wymagane jest
potwierdzenie przez `sygnatariuszy <Sygnatariusz_>`__ konta
twórcy. Ci uczestnicy muszą wysłać tę samą transakcję ze
swoim podpisem.

Walidacja
=========

Istnieją dwa rodzaje walidacji — bezstanowa i stanowa.

Walidacja bezstanowa
--------------------

Wykonywana w `Torii <architecture.html#torii>`__.
Sprawdza, czy obiekt jest poprawnie sformułowany, w tym
podpisy.

Walidacja stanowa
-----------------

Wykonywana w `Zweryfikowany twórca propozycji
<Zweryfikowana propozycja_>`__. Waliduje względem
`World State View <architecture.html#world-state-view>`__.


.. [#f1] https://en.bitcoin.it/wiki/Block
