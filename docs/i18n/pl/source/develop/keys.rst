===============
Format pary kluczy
===============

Iroha używa par kluczy (.pub i .priv) do podpisywania
transakcji – każde `konto
<../concepts_architecture/glossary.html#account>`_ ma co
najmniej 1 parę.
Niektóre konta (jeśli `quorum
<../concepts_architecture/glossary.html#quorum>`_ jest większe
niż 1) mogą mieć więcej `sygnatariuszy
<../concepts_architecture/glossary.html#signatory>`_
podpisujących transakcje – i każdy sygnatariusz ma parę kluczy.
Algorytmy kryptograficzne używają tych kluczy – a w Iroha
oferujemy Ci wybór – których algorytmów używać.

.. note:: Zobacz, jak tworzyć pary kluczy za pomocą biblioteki
Python `tutaj
<../getting_started/python-guide.html#creating-your-own-key-pairs-with-python-library>`__.

Obsługiwane algorytmy kryptograficzne
=====================================

Natywnie HL Iroha używa niestandardowego algorytmu – Ed25519 z
SHA-3. Te klucze są obsługiwane przez wszystkie wersje Irohy,
w tym stare.
Ale jak wszyscy wiemy, potrzebujemy też bardziej uniwersalnych
opcji – dlatego Iroha ma `integrację HL Ursa
<../integrations/index.html#hyperledger-ursa>`_ – to
biblioteka z różnymi algorytmami kryptograficznymi, która
pozwala pracować z Irohą używając bardziej głównonurtowych
kluczy. Ursa zapewnia Iroha wsparcie standardowego Ed25519 z
algorytmem SHA-2.

Klucze publiczne
----------------

Aby zapewnić łatwe rozwiązanie, które pozwoliłoby używać
różnych algorytmów bez "łamania" zgodności wstecznej,
wprowadziliśmy format **multihash** dla kluczy publicznych w
Iroha. Możesz dowiedzieć się więcej o multihash `tutaj
<https://github.com/multiformats/multihash>`__.

Ogólnie, aby używać kluczy innych niż natywne klucze ed25519
SHA-3, będziesz musiał doprowadzić je do tego formatu:

.. code-block:: shell

	<varint key type code><varint key size in bytes><actual key bytes>


.. note:: W multihash varinty to bezsignowe varinty
najbardziej znaczącego bitu (zwane też varintami base-128).


Jeśli Iroha otrzyma standardowy klucz publiczny 32-bajtowy,
potraktuje go jako natywny klucz Iroha.
Jeśli otrzyma klucz publiczny multihash, potraktuje go
zgodnie z tabelą poniżej.


Obecnie Iroha "rozumie" tylko jeden format klucza multihash:

+------------+-----------+----------+------------------+
|Nazwa       |Tag        |Kod       |Opis              |
+============+===========+==========+==================+
|ed25519-pub |key        |0xed	    |Klucz publiczny Ed25519|
+------------+-----------+----------+------------------+

Przykłady kluczy publicznych w Iroha:

+----------------+--------+----------+-------------------------+----------------------+
| typ            | kod    | długość  | dane                    | co rozpoznaje Iroha  |
+================+========+==========+=========================+======================+
| klucz multihash| ED01   | 20       | 62646464c35383430b...   | ed25519/sha2         |
+----------------+--------+----------+-------------------------+----------------------+
| surowy klucz   | --     | --       | 716fe505f69f18511a...   | ed25519/sha3         |
| 32-bajtowy     |        |          |                         |                      |
+----------------+--------+----------+-------------------------+----------------------+

Zauważ, że kod `0xED` jest kodowany jako `ED01` zgodnie z
regułami formatu multihash.

Klucze prywatne
----------------

**Klucze prywatne** w Ursa są reprezentowane przez konkatenację
klucza prywatnego i klucza publicznego – bez prefiksów
multihash.
