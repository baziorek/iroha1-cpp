Biblioteka iOS Swift (nieutrzymywana)
------------------------------------

.. note::
   Ta strona opisuje upstreamową bibliotekę ``iroha-ios``, która
   jest **nieutrzymywana od 2020** (ostatni release v4.0.1) —
   nadal działa z węzłami Iroha 1, ale nie otrzymuje aktualizacji.
   Dla nowych projektów preferuj aktywnie utrzymywane biblioteki
   :doc:`cpp` lub :doc:`python` tego forka.

Biblioteka została utworzona, aby zapewnić wygodny interfejs dla
aplikacji iOS do komunikacji z blockchainem Iroha, w tym
wysyłanie transakcji/zapytań, strumieniowanie statusów transakcji
i commitów bloków.

Gdzie zdobyć
^^^^^^^^^^^^

Biblioteka Iroha iOS jest dostępna przez CocoaPods. Aby ją
zainstalować, po prostu dodaj następującą linię do swojego
Podfile:

.. code-block:: swift

    pod 'IrohaCommunication'

Możesz też pobrać kod źródłowy biblioteki z `jej repozytorium
<https://github.com/hyperledger/iroha-ios>`__


Jak używać
^^^^^^^^^^

Dla nowych użytkowników Iroha zalecamy sprawdzenie `przykładowego
projektu iOS
<https://github.com/hyperledger/iroha-ios/tree/master/Example>`__.
Stara się on połączyć z peerem Iroha, który powinien być również
uruchomiony lokalnie na Twoim komputerze, aby utworzyć nowe konto
i wysłać na nie trochę ilości assetu.
Aby uruchomić projekt, przejdź przez poniższe kroki:

- Postępuj zgodnie z instrukcjami z dokumentacji Iroha, aby
  skonfigurować i uruchomić peer Iroha w kontenerze Docker.

- Sklonuj `repozytorium iroha-ios
  <https://github.com/hyperledger/iroha-ios>`__.

- Przejdź do katalogu Example i uruchom pod install.

- Otwórz IrohaCommunication.xcworkspace w XCode

- Zbuduj i uruchom target IrohaExample.

- Sprawdź logi, aby zobaczyć, czy scenariusz zakończył się
  pomyślnie.

Zachęcamy do eksperymentowania z przykładowym projektem i
zadawania wszelkich pytań na `Discord
<https://discord.com/channels/905194001349627914/905205848547155968>`_.
