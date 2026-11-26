Wskazówki bezpieczeństwa instalacji Irohy
=========================================

Ten przewodnik ma na celu zabezpieczenie instalacji Irohy.
Większość kroków z tego przewodnika może wydawać się oczywista,
ale pomaga uniknąć potencjalnych problemów bezpieczeństwa w
przyszłości.

Bezpieczeństwo fizyczne
^^^^^^^^^^^^^^^^^^^^^^^

W przypadku gdy serwery są zlokalizowane lokalnie (fizycznie
dostępne), należy zastosować szereg środków bezpieczeństwa.
Pomiń te kroki, jeśli używany jest hosting w chmurze.

Ustal politykę organizacyjną i/lub system kontroli dostępu tak,
aby tylko upoważniony personel miał dostęp do serwerowni.
Następnie ustaw hasło BIOS/firmware i skonfiguruj kolejność
startową, aby zapobiec nieautoryzowanemu uruchamianiu z innych
nośników.
Upewnij się, że bootloader jest chroniony hasłem, jeśli taka
funkcjonalność istnieje. Dobrze też mieć monitoring CCTV.

Wdrożenie
^^^^^^^^^

Po pierwsze, zweryfikuj, że używane jest oficjalne repozytorium do
pobierania `kodu źródłowego
<https://github.com/baziorek/iroha1-cpp>`__ (społecznościowy fork
`hyperledger-iroha/iroha
<https://github.com/hyperledger-iroha/iroha/tree/iroha1-main>`__)
i `obrazów Docker
<https://hub.docker.com/r/hyperledger/iroha>`__.
Zmień wszelkie domyślne hasła używane podczas instalacji, np.
hasło do połączenia z postgres.
Repozytorium Irohy zawiera przykłady kluczy prywatnych i
publicznych — nigdy nie używaj ich w produkcji.
Co więcej, zweryfikuj, że nowe pary kluczy są generowane w
bezpiecznym środowisku i tylko administrator ma dostęp do tych
par kluczy (lub przynajmniej zminimalizuj liczbę osób).
Po wdrożeniu kluczy do peerów Iroha usuń klucze prywatne z hosta
używanego do wdrożenia, tj. klucze prywatne powinny znajdować się
tylko wewnątrz peerów Iroha.
Utwórz zaszyfrowaną kopię zapasową kluczy prywatnych przed ich
usunięciem i ogranicz do niej dostęp.

Konfiguracja sieci
^^^^^^^^^^^^^^^^^^^

Iroha nasłuchuje na portach 50051 i 10001, a opcjonalnie 55552,
jeśli TLS jest włączony.
Ustawienia firewalla muszą zezwalać na połączenia przychodzące/
wychodzące do/z tych portów.
Jeśli to możliwe, wyłącz lub usuń wszelkie inne usługi sieciowe
z portami nasłuchującymi (FTP, DNS, LDAP, SMB, DHCP, NFS, SNMP
itd.).
W idealnym przypadku Iroha powinna być tak izolowana jak to
możliwe pod względem sieci.

Możesz włączyć TLS na portach torii, jeśli chcesz szyfrować
komunikację klient-peer.

Jeśli nie używasz szyfrowania ruchu, zdecydowanie zalecamy użycie
VPN lub Calico do konfiguracji nakładkowej sieci Docker, tj.
jakiegokolwiek mechanizmu umożliwiającego szyfrowanie komunikacji
między peerami.
Docker swarm domyślnie szyfruje komunikację, ale pamiętaj, aby
otworzyć niezbędne porty w konfiguracji firewalla.
W przypadku użycia VPN zweryfikuj, że klucz VPN jest
niedostępny dla innych użytkowników.

Jeśli używany jest SSH, wyłącz logowanie roota.
Poza tym wyłącz uwierzytelnianie hasłem i używaj tylko kluczy.
Pomocne może być również ustawienie poziomu logowania SSH na
INFO.

Jeśli IPv6 nie jest używane, dobrym pomysłem może być jego
wyłączenie.

Aktualizacje
^^^^^^^^^^^^

Zainstaluj najnowsze poprawki bezpieczeństwa systemu
operacyjnego i regularnie go aktualizuj.
Jeśli Iroha działa w kontenerach Docker, regularnie
aktualizuj Dockera.
Choć opcjonalne, dobrym praktykom jest testowanie aktualizacji
na osobnym serwerze przed instalacją w produkcji.

Rejestrowanie i monitorowanie
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Zbieraj i wysyłaj logi do dedykowanej maszyny za pomocą agenta
  (np. Filebeat).
- Zbieraj logi ze wszystkich peerów Iroha w centralnym punkcie
  (np. Logstash).
- Przesyłaj informacje o logowaniu i monitorowaniu szyfrowanym
  kanałem (np. https).
- Skonfiguruj mechanizm uwierzytelniania, aby zapobiec dostępowi
  osób trzecich do logów.
- Skonfiguruj mechanizm uwierzytelniania, aby zapobiec osobom
  trzecim przesyłaniu logów.
- Rejestruj cały dostęp administratora.

Wzmacnianie systemu operacyjnego
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Następujące kroki zakładają, że Docker jest używany do
uruchamiania Irohy.

- Włącz i skonfiguruj Docker Content Trust.
- Zezwól na kontrolowanie demona Dockera tylko zaufanym
  użytkownikom.
- Ustal limit dla zasobów kontenera Dockera.
