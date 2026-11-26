Scenariusze użycia
==================

Wymieniamy szereg przypadków użycia i konkretne korzyści, jakie
Hyperledger Iroha może wnieść do tych aplikacji. Mamy nadzieję,
że te aplikacje i przypadki użycia zainspirują deweloperów i
twórców do dalszych innowacji z Hyperledger Iroha.


Certyfikaty w edukacji i opiece zdrowotnej
------------------------------------------

Hyperledger Iroha włącza do systemu wiele instytucji
certyfikujących, takich jak uniwersytety, szkoły i placówki
medyczne. Elastyczny model uprawnień Hyperledger Iroha pozwala
budować tożsamości certyfikujące i nadawane certyfikaty.
Przechowywanie jawnych i niejawnych informacji na koncie
użytkownika umożliwia budowanie różnych systemów reputacji i
tożsamości.

Dzięki Hyperledger Iroha każdy certyfikat edukacyjny lub
medyczny można zweryfikować pod kątem tego, czy został wydany
przez określoną instytucję certyfikującą. Niezmienność i jasne
reguły walidacji zapewniają przejrzystość w opiece zdrowotnej
i edukacji, znacząco zmniejszając użycie fałszywych
certyfikatów.

Przykład
^^^^^^^^

Wyobraź sobie instytucję medyczną zarejestrowaną jako domena
``hospital`` w Hyperledger Iroha. Ta domena ma certyfikowanych
i zarejestrowanych pracowników, każdy z pewną rolą, np.
``physician``, ``therapist``, ``nurse``. Każdy pacjent szpitala
ma konto z pełną historią medyczną. Każda historia choroby,
jak wyniki badań krwi, jest bezpiecznie i prywatnie
przechowywana na koncie pacjenta jako pary klucz/wartość JSON.
Reguły w domenie ``hospital`` są zdefiniowane tak, że tylko
certyfikowani pracownicy medyczni i użytkownik mają dostęp do
informacji osobowych. Dane medyczne zwracane przez zapytanie
są weryfikowane pod kątem pochodzenia z zaufanego źródła.

Szpital jest powiązany z konkretną lokalizacją, zgodnie z
przepisami prawnymi tej lokalizacji, np. przechowywaniem danych
osobowych obywateli tylko w określonych regionach
(``privacy rules``_). Podejście wielodomenowe w Hyperledger
Iroha pozwala udostępniać informacje między wieloma krajami bez
łamania przepisów. Na przykład, jeśli użytkownik
``makoto@hospital`` zdecyduje się udostępnić swoją historię
przypadków medycznych instytucji w innym kraju, może użyć
polecenia ``grant`` z uprawnieniem ``can_get_my_acc_detail``.

Podobnie jak instytucja medyczna, zarejestrowany uniwersytet w
Hyperledger Iroha ma uprawnienia do przesyłania informacji do
absolwentów. Dyplom lub certyfikat jest w zasadzie
Proof-of-Graduation z podpisem uznawanego uniwersytetu. To
podejście ułatwia proces rekrutacji — pracodawca wykonuje
zapytanie do Hyperledger Iroha, aby uzyskać umiejętności i
kompetencje potencjalnego pracownika.

.. _`privacy rules`: https://privacypolicies.com/blog/privacy-law-by-country/

Transgraniczne transfery aktywów
--------------------------------

Hyperledger Iroha zapewnia szybkie i jasne reguły handlu
i rozliczeń przy użyciu kont wielopodpisowych i wymiany
atomowej. Zarządzanie aktywami jest proste jak w systemach
scentralizowanych, zapewniając jednocześnie niezbędne
gwarancje bezpieczeństwa. Upraszczając reguły i polecenia
wymagane do tworzenia i transferowania aktywów, obniżamy
barierę wejścia, zachowując jednocześnie wysokie gwarancje
bezpieczeństwa.

Przykład
^^^^^^^^

Na przykład [#f1]_, użytkownik może chcieć przenieść własność
samochodu. Użytkownik ``haruto`` zarejestrował relację
właściciel-aktywo z samochodem marki ``sora`` z parametrami:
``{"id": "34322069732074686520616E73776572", "color": "red",
"size": "small"}``. Ta własność jest zapisana w bazie danych
systemu z kopиями u każdego walidującego peera. Aby wykonać
transfer, użytkownik ``haruto`` tworzy ofertę, czyli
wielopodpisową transakcję z dwoma poleceniami: ``transfer``
identyfikatora samochodu do użytkownika ``haru`` i ``transfer``
pewnej kwoty tokenów ``usd`` od ``haru`` do ``haruto``.
Otrzymując ofertę, ``haru`` akceptuje ją podpisując transakcję
wielopodpisową — w tym przypadku transakcja jest atomowo
zatwierdzana w systemie.

Hyperledger Iroha nie ma wbudowanego tokenu, ale obsługuje
różne aktywa od różnych twórców. To podejście pozwala budować
zdecentralizowany rynek wymiany. Na przykład system może mieć
banki centralne z różnych krajów emitujące aktywa.

.. [#f1] Obecnie niezaimplementowane

Aplikacje finansowe
-------------------

Hyperledger Iroha może być bardzo przydatna w procesie
audytu. Każda informacja jest walidowana przez reguły
biznesowe i stale utrzymywana przez odrębnych uczestników
sieci. Reguły kontroli dostępu wraz z pewnym szyfrowaniem
zapewniają pożądany poziom prywatności. Reguły kontroli
dostępu można definiować na różnych poziomach: użytkownika,
domeny lub systemu. Na poziomie użytkownika definiowane są
reguły prywatności dla konkretnej osoby. Jeśli reguły dostępu
są określone na poziomie domeny lub systemu, dotyczą wszystkich
użytkowników w domenie. W Hyperledger Iroha zapewniamy wygodne
reguły kontroli dostępu oparte na rolach, gdzie każda rola ma
określone uprawnienia.

Transakcje można śledzić w lokalnej bazie danych. Używając
Iroha-API, audytor może wykonywać zapytania i analitykę danych,
uruchamiając konkretne oprogramowanie audytowe.
Hyperledger Iroha obsługuje różne scenariusze wdrażania
oprogramowania analitycznego: na komputerze lokalnym lub
wykonując kod na konkretnym middleware. To podejście pozwala
analizować aplikacje Big Data z Hadoop, Apache i innymi.
Hyperledger Iroha służy jako gwarant integralności i prywatności
danych (dzięki ograniczeniu uprawnień zapytań).

Przykład
^^^^^^^^

Na przykład audyt może być pomocny w aplikacjach finansowych.
Konto audytora ma rolę ``auditor`` z uprawnieniami dostępu do
informacji użytkowników w domenie bez uciążliwości dla
użytkownika. Aby zmniejszyć prawdopodobieństwo przejęcia
konta i zapobiec wysyłaniu złośliwych zapytań przez audytora,
audytor jest zwykle definiowany jako konto wielopodpisowe,
co oznacza, że audytor może wykonywać zapytania tylko mając
podpisy od wielu odrębnych tożsamości. Audytor może
wykonywać zapytania nie tylko o pobieranie danych konta
i salda, ale także o wszystkie transakcje użytkownika, np.
wszystkie transfery użytkownika ``haruto`` w domenie
``konoha``. Aby efektywnie analizować dane miliona użytkowników,
każdy węzeł Iroha może współpracować z oprogramowaniem
analitycznym.

Transakcje wielopodpisowe są potężnym narzędziem Hyperledger
Iroha, które może zrewolucjonizować system podatkowy. Każda
transakcja w danej domenie może być transakcją
wielopodpisową, gdzie jeden podpis pochodzi od użytkownika
(np. transfer aktywa), a drugi od specjalnych węzłów
podatkowych. Węzły podatkowe będą miały specjalne reguły
walidacji napisane za pomocą Iroha-API, np. każdy zakup w
certyfikowanych sklepach musi wiązać się z zapłatą podatku.
Innymi słowy, prawidłowa transakcja zakupu w Iroha musi
zawierać dwa polecenia: transfer pieniędzy (zakup) do sklepu
i transfer pieniędzy (płatność podatku) do rządu.


Zarządzanie tożsamościami
-------------------------

Hyperledger Iroha ma wbudowane wsparcie zarządzania
tożsamościami. Każdy użytkownik w systemie ma jednoznacznie
zidentyfikowane konto z informacjami osobowymi, a każda
transakcja jest podpisana i powiązana z określonym
użytkownikiem. To sprawia, że Hyperledger Iroha jest idealna
do różnych aplikacji z funkcjami KYC (Know Your Customer).

Przykład
^^^^^^^^

Na przykład firmy ubezpieczeniowe mogą korzystać z
odpytywania informacji o transakcjach użytkownika bez
martwienia się o prawdziwość informacji. Użytkownicy również
mogą korzystać z przechowywania informacji osobowych na
blockchainie, ponieważ uwierzytelnione informacje skrócą czas
przetwarzania roszczeń.
Wyobraź sobie sytuację, w której użytkownik chce wziąć kredyt
hipoteczny. Obecnie wstępna kwalifikacja to żmudny proces
zbierania informacji o dochodach, długach i weryfikacji
informacji. Każdy użytkownik w Hyperledger Iroha ma konto
z zweryfikowanymi informacjami osobowymi, takimi jak posiadane
aktywa, stanowiska pracy i długi. Dochody i długi użytkownika
można prześledzić zapytaniem ``GetAccountTransactions``,
posiadane aktywa zapytaniem ``GetAccountAssets``, a stanowiska
pracy zapytaniem ``GetAccountDetail``. Każde zapytanie zwraca
zweryfikowany wynik, więc przetwarzanie kredytu hipotecznego
zajmie tylko kilka sekund.
Aby zachęcić użytkowników do udostępniania informacji
osobowych, różne firmy mogą opracować procesy biznesowe. Na
przykład firmy ubezpieczeniowe mogą tworzyć bonusowe zniżki
dla użytkowników uprawiających aktywność fizyczną.
Aplikacje fitness mogą przesyłać prywatne Proof-of-Activity
do systemu, a użytkownik może później zdecydować się
udostępnić informacje firmom ubezpieczeniowym za pomocą
``GrantPermission`` z uprawnieniem ``can_get_my_acc_detail``.


Łańcuch dostaw
--------------

Zarządzanie zdecentralizowanym systemem i reprezentowanie
reguł prawnych jako kodu systemu to istotne połączenie każdego
systemu łańcucha dostaw. System certyfikacji w Hyperledger
Iroha pozwala na tokenizację przedmiotów fizycznych i
osadzanie ich w systemie. Każdy przedmiot zawiera informacje
o "czym, kiedy, gdzie i dlaczego".

Systemy uprawnień i ograniczony zestaw bezpiecznych poleceń
rdzenia zawęża wektor ataku i zapewnia bez wysiłku podstawowy
poziom prywatności. Każda transakcja jest możliwa do
wyśledzenia w systemie za pomocą wartości hash, danych
uwierzytelniających lub certyfikatów twórcy.

Przykład
^^^^^^^^

Łańcuch dostaw żywności to wspólny system z wieloma różnymi
aktorami, takimi jak rolnicy, magazyny, sklepy spożywcze
i klienci. Celem jest dostarczenie żywności od pola rolnika
do stołu klienta. Produkt przechodzi przez wiele etapów,
z których każdy jest rejestrowany we wspólnej przestrzeni.
Klient skanuje kod produktu przez urządzenie mobilne, w którym
zakodowane jest zapytanie Iroha. Zapytanie Iroha zapewnia
pełną historię ze wszystkimi etapami, informacjami o produkcie
i rolniku.

Na przykład ``gangreen`` to zarejestrowany rolnik, twórca
zasobu ``tomato``; działa jako gwarant tokenizujący przedmioty
fizyczne, czyli wiążący każdy pomidor z elementem Iroha
``tomato``. Procesy tworzenia i dystrybucji zasobów są
całkowicie przejrzyste dla uczestników sieci. Pomidor Iroha
wyrusza w podróż przez mnóstwo sprzedawców, aby ostatecznie
trafić do użytkownika ``chad``.

Uprościliśmy tworzenie zasobów do pojedynczego polecenia
``CreateAsset`` bez potrzeby tworzenia złożonych smart
kontraktów. Jedną z głównych zalet Hyperledger Iroha jest
jej prostota, która pozwala deweloperom skupić się na
dostarczanej wartości swoich aplikacji.

Zarządzanie funduszami
----------------------

Dzięki wsparciu transakcji wielopodpisowych możliwe jest
utrzymanie funduszu przez wielu menedżerów. W tym schemacie
inwestycja może zostać dokonana dopiero po potwierdzeniu
przez kworum uczestników.

Przykład
^^^^^^^^

Aktywa funduszu powinny być przechowywane na jednym koncie.
Jego sygnatariuszami powinni być menedżerowie funduszu,
którzy zajmują się inwestycjami i dystrybucją portfeli.
Można to dodać za pomocą polecenia ``AddSignatory``.
Wszystkie aktywa powinny być przechowywane na jednym koncie,
którego sygnatariusze reprezentują menedżerów funduszu.
W ten sposób konkretne wymiany można wykonywać transakcją
wielopodpisową, tak że wszyscy będą decydować o konkretnej
decyzji finansowej.
Jedna strona może potwierdzić transakcję, wysyłając oryginalną
transakcję i podpis jednego z menedżerów.
Iroha będzie utrzymywać wysyłanie transakcji, tak że transakcja
nie zostanie zakończona, dopóki nie otrzyma wymaganej liczby
potwierdzeń, która jest parametryzowana parametrem kworum
transakcji.


Powiązane badania
-----------------

(Pomysł polegał na pokazaniu obecnych pionierów aplikacji
blockchainowych i ich prac.)
