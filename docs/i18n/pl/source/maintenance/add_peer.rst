================
Dodawanie peerów
================

W HL Iroha możesz dodawać nowych peerów do sieci, gdy już działa.
Robi się to za pomocą specjalnej komendy `AddPeer
<../develop/api/commands.html#add-peer>`_.

Wymagania
=========

**Powinien istnieć peer, który:**

— działa z blokiem Genesis (początkowym blokiem blockchaina)
identycznym z tym na peerach już w sieci;

— ma rozwiązywalny adres;

— ma parę kluczy peera (Ed25519 z SHA-2/SHA-3)

.. important:: Konto wysyłające transakcję dodającą peera musi
   mieć uprawnienie `Can Add Peer
   <../develop/api/permissions.html#can-add-peer>`_, a do usuwania
   peera — uprawnienie `Can Remove Peer
   <../develop/api/permissions.html#can-remove-peer>`_ w swojej
   roli — musi to być ustawione w bloku genesis.

Użycie
======

Jak opisano w `referencji API
<../develop/api/commands.html#add-peer>`_, aby użyć komendy,
będziesz potrzebować tylko:

— klucza publicznego peera, który chcesz dodać do sieci;

— rozwiązywalnego adresu IP peera

Kroki:

1. Utwórz sieć z uprawnieniami `Can Add Peer
   <../develop/api/permissions.html#can-add-peer>`_ i `Can Remove
   Peer <../develop/api/permissions.html#can-remove-peer>`_
   ustawionymi w bloku genesis przypisanym do użytkownika;
2. Utwórz kolejnego peera uruchamiającego HL Irohę z tym samym
   blokiem genesis i podobną konfiguracją;
3. Wyślij transakcję z konta z niezbędnymi uprawnieniami,
   zawierającą komendę ``add peer`` (zobacz przykład poniżej)
4. Sprawdź logi peerów, aby zobaczyć, czy wszystko działa
   poprawnie.

Możesz też upewnić się, że wszystko jest w porządku, wysyłając
transakcję i sprawdzając, czy liczba bloków jest taka sama na
węzłach.

.. note:: Jeśli działa tylko 1 istniejący peer, będziesz musiał
   skonfigurować dodawanych peerów tak, aby mieli wszystkich peerów
   (zarówno istniejących, jak i nowych) w parametrze
   "initial_peers" w `konfiguracji
   <../configure/index.html#environment-specific-parameters>`_.
   Innym przypadkiem, gdy jest to potrzebne, jest sytuacja, gdy sieć
   działa od jakiegoś czasu, a peerzy wskazani w bloku genesis już
   nie istnieją (bo zostali usunięci komendą Remove Peer, gdy
   dodawano nowych peerów).

Przykład
========

Oto jak komenda może wyglądać w Pythonie.
W tym przykładzie użyliśmy `uprawnienia Root
<../develop/api/permissions.html#root>`_, które ma wszystkie
uprawnienia, w tym `Can Add Peer
<../develop/api/permissions.html#can-add-peer>`_ i `Can Remove
Peer <../develop/api/permissions.html#can-remove-peer>`_:

.. code-block:: python

    def add_peer():
        peer1 = primitive_pb2.Peer()
        peer1.address = '192.168.1.1:50541'
        peer1.peer_key = '716fe505f69f18511a1b083915aa9ff73ef36e6688199f3959750db38b8f4bfc'
        tx = iroha.transaction([
            iroha.command('AddPeer', peer=peer1)
        ], creator_account=ADMIN_ACCOUNT_ID, quorum=1)

        IrohaCrypto.sign_transaction(tx, ADMIN_PRIVATE_KEY)

    add_peer()

Usuwanie peera
==============

Aby usunąć peera, będziesz potrzebować komendy `Remove Peer
<../develop/api/commands.html#remove-peer>`_ z konta, które ma
uprawnienie ``CanRemovePeer``.
