=======================
Rozwiązywanie problemów
=======================

Odzyskiwanie po awaryjnym wyłączeniu
------------------------------------

::

	[2020-11-27 10:40:01.764013860][th:1][warning] Irohad/Storage/FlatFileBlockStorage Error while block deserialization: Unexpected end of string. Expected a value.

	^

::

	[2020-11-25 10:36:19.993552669][C][Init]: Irohad startup failed: WSV state (height 4576773) is more recent than block storage (height 4576772).

Takie komunikaty mogą pojawić się, jeśli węzeł uległ awarii podczas
używania magazynu bloków opartego na plikach. Skopiuj brakujące
bloki z innego węzła — w takim przypadku możesz użyć flagi
``--reuse_state`` dla szybkiego startu — lub usuń bloki zaczynając
od pustego pliku i odtwórz WSV z istniejących bloków.

Rozłączanie klientów lub błędy "Rendezvous" po długich okresach
bezczynności
-------------------------------------------------------------------

Jeśli klienci (szczególnie długo żyjące strumienie statusów)
regularnie tracą połączenie z Torii po wielu godzinach
bezczynności, przyczyną jest zazwyczaj timeout bezczynności na
pośrednim hopie (NAT, load balancer, proxy), który po cichu
zrywa połączenie HTTP/2.

Ten fork ``irohad`` włącza **gRPC keepalive pingi** na wszystkich
swoich serwerach (Torii, internal ordering, utility): serwer
ping-uje bezczynne połączenia co 60 sekund i zamyka martwe po
20-sekundowym timeoutie, dzięki czemu klienci łączą się ponownie
czysto zamiast wisieć na półotwartym kanale.

Jeśli nadal widzisz rozłączenia:

- upewnij się, że używasz builda zawierającego fix keepalive
  (ten fork, branch ``iroha1-main``);
- sprawdź, czy proxy/LB między klientem a węzłem ma własny timeout
  bezczynności krótszy niż interwał keepalive i zwiększ go;
- dla strumieni statusów preferuj ponowne łączenie z backoff —
  zobacz ``stale_stream_max_rounds`` w :ref:`configuration`.
