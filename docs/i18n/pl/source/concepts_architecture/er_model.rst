Model encji i relacji
======================

Każdy peer Hyperledger Iroha ma stan zwany "World State View",
reprezentowany przez zbiór encji i relacji między nimi.
Aby wyjaśnić, jakie encje istnieją w systemie i jakie są między
nimi relacje, ta sekcja zawiera diagram ER i wyjaśnienie jego
komponentów.

Diagram ER
----------

.. image:: ./../../image_assets/er-model.png

Peer
----

 - address — adres sieciowy i port wewnętrzny, używany do
   synchronizacji, konsensusu i komunikacji z usługą ordering
 - public_key — klucz używany do podpisywania bloków podczas
   procesu konsensusu

Asset
-----

 - asset_id — identyfikator assetu, sformatowany jako
   asset_name#domain_id
 - domain_id — identyfikator domeny, w której utworzono asset,
   referencja do istniejącej domeny
 - precision — rozmiar części ułamkowej

Signatory
---------

 - public_key — klucz publiczny

Domain
------

 - domain_id — identyfikator domeny
 - default_role — domyślna rola na użytkownika utworzonego w
   domenie, referencja do istniejącej roli

Role
----

 - role_id — identyfikator roli

RoleHasPermissions
^^^^^^^^^^^^^^^^^^

 - role_id — identyfikator roli, referencja do istniejącej roli
 - permission_id — identyfikator uprawnienia

Account
-------

 - account_id — identyfikator konta, sformatowany jako
   account_name@domain_id
 - domain_id — identyfikator domeny, w której utworzono konto,
   referencja do istniejącej domeny
 - quorum — liczba sygnatariuszy wymagana do utworzenia ważnej
   transakcji z tego konta
 - transaction_count – licznik transakcji utworzonych przez to
   konto
 - data — magazyn klucz-wartość dla dowolnych informacji
   związanych z kontem. Rozmiar ograniczony do 268435455 bajtów
   (0x0FFFFFFF) (pole PostgreSQL JSONB).

AccountHasSignatory
^^^^^^^^^^^^^^^^^^^

 - account_id — identyfikator konta, referencja do istniejącego
   konta
 - public_key — klucz publiczny (zwany też sygnatariuszem),
   referencja do istniejącego sygnatariusza

AccountHasAsset
^^^^^^^^^^^^^^^

 - account_id — identyfikator konta, referencja do istniejącego
   konta
 - asset_id — identyfikator assetu, referencja do istniejącego
   assetu
 - amount — ilość assetu należąca do konta

AccountHasRoles
^^^^^^^^^^^^^^^

 - account_id — identyfikator konta, referencja do istniejącego
   konta
 - role_id — identyfikator roli, referencja do istniejącej roli

AccountHasGrantablePermissions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

 - account_id — identyfikator konta, referencja do istniejącego
   konta. To konto nadaje uprawnienie do przekazywania
   (grantable) wykonywania operacji na sobie posiadaczowi
   pozwolenia.
 - permittee_account_id — identyfikator konta, referencja do
   istniejącego konta. To konto otrzymuje uprawnienie do
   wykonywania operacji na account_id.
 - permission_id — identyfikator grantable_permission
