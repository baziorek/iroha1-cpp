Konfiguracja TLS dla komunikacji klient-peer (torii)
====================================================

Domyślnie komunikacja klient-peer nie jest szyfrowana.
Aby to włączyć, musisz:

1. `Wygenerować <#generating-keys>`_ parę klucz/certyfikat dla
   każdego peera
2. Rozdystrybuować certyfikat wszystkim klientom
3. `Skonfigurować <#configuring-irohad>`_ irohad, aby używał
   tych kluczy
4. [Ponownie] uruchomić irohad


Generowanie kluczy
~~~~~~~~~~~~~~~~~~~

Klucze muszą być w formacie PEM. Aby je wygenerować, możesz użyć
``openssl``:

.. code-block:: sh

    $ openssl genpkey -algorithm rsa -out server.key
    $ openssl req -new -key server.key -x509 -out server.crt

Możesz użyć dowolnego algorytmu zamiast ``rsa``, o ile Twój
``openssl`` go obsługuje.
Aby dowiedzieć się, które są obsługiwane, użyj

.. code-block:: sh

    $ openssl list-public-key-algorithms

Jeśli musisz używać zwykłych adresów IP do łączenia się z węzłem,
musisz określić ``subjectAltName`` w certyfikacie serwera — w tym
celu dodaj dyrektywę ``subjectAltName`` do sekcji ``v3_ca``
konfiguracji openssl przed wygenerowaniem certyfikatu.
Na przykład dla domyślnej instalacji, ``/etc/ssl/openssl.cnf``:

.. code-block:: text

    [ v3_ca ]
    subjectAltName=IP:12.34.56.78

Pola w certyfikacie tak naprawdę nie mają znaczenia z wyjątkiem
Common Name (CN) — jest on sprawdzany względem nazwy hosta
klienta, a handshake TLS nie powiedzie się, jeśli się nie zgadzają
(np. jeśli łączysz się z example.com:50051, irohad na
example.com musi mieć example.com we wspólnej nazwie
certyfikatu).

Konfiguracja irohad
~~~~~~~~~~~~~~~~~~~~

Aby skonfigurować irohę do używania Twoich kluczy, musisz
zmodyfikować parametr konfiguracyjny ``torii_tls_params``.

Powinien wyglądać jak poniższy blok:

.. code-block:: javascript

    "torii_tls_params": {
        "port": 55552,
        "key_pair_path": "/path/to/server"
    }

``port`` - ustaw na dowolny port (ale zwykle chcesz 55552)

``key_pair_path`` - ustaw na pełną ścieżkę do pary
klucz/certyfikat, tak że jeśli masz klucz w ``/path/to/server.key``
i certyfikat w ``/path/to/server.crt``, musisz podać
``torii_tls_keypair=/path/to/server``

.. note:: W `katalogu przykładów
  <https://github.com/baziorek/iroha1-cpp/tree/iroha1-main/example/torii_tls>`_
  znajdują się przykładowe certyfikaty, ale aby włączyć TLS
  potrzebujesz nowego certyfikatu dla swojego serwera (przykład
  nie zadziała).
