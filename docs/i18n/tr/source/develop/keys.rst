===============
Anahtar çifti biçimi
====================

Iroha, işlemleri imzalamak için anahtar çiftleri (.pub ve .priv
anahtarları) kullanır – her `hesabın
<../concepts_architecture/glossary.html#account>`__ en az 1 çifti
vardır. Bazı hesapların (`quorum
<../concepts_architecture/glossary.html#quorum>`__ 1'den büyükse)
işlemleri imzalayan daha fazla `imzalayanı
<../concepts_architecture/glossary.html#signatory>`__ olabilir –
ve her imzalayanın bir anahtar çifti vardır. Kriptografik
algoritmalar bu anahtarları kullanır – ve Iroha'da hangi
algoritmaların kullanılacağını seçme imkânı sunuyoruz.

.. note:: Python kütüphanesi kullanarak anahtar çiftlerinin nasıl
   oluşturulacağını `burada
   <../getting_started/python-guide.html#creating-your-own-key-pairs-with-python-library>`__
   görebilirsiniz.

Desteklenen kripto algoritmaları
================================

Yerel olarak HL Iroha özel bir algoritma kullanır – SHA-3 ile
Ed25519. Bu anahtarlar eskiler dahil tüm Iroha sürümleri
tarafından desteklenir. Ancak hepimizin bildiği gibi daha evrensel
seçeneklere de ihtiyacımız var – bu yüzden Iroha'nın `HL Ursa
entegrasyonu
<../integrations/index.html#hyperledger-ursa>`__ var – bu,
farklı kripto algoritmalarına sahip bir kütüphanedir ve daha
ana akım anahtarlar kullanarak Iroha ile çalışmanıza olanak
tanır. Ursa, Iroha'ya SHA-2 algoritmasıyla standart Ed25519
desteği sağlar.

Açık anahtarlar
---------------

Geriye dönük uyumluluğu "bozmadan" farklı algoritmalar
kullanmanıza olanak tanıyan kolay bir çözüm sağlamak için
Iroha'da açık anahtarlar için **multihash** biçimini
tanıttık. Multihash hakkında daha fazlasını `buradan
<https://github.com/multiformats/multihash>`__ öğrenebilirsiniz.

Genel olarak, yerel SHA-3 ed25519 anahtarlarından farklı anahtarlar
kullanmak için bunları bu biçime dönüştürmeniz gerekir:

.. code-block:: shell

	<varint key type code><varint key size in bytes><actual key bytes>


.. note:: Multihash'te varint'ler, En Anlamlı Bit işaretsiz
   varint'leridir (base-128 varint olarak da adlandırılır).


Iroha 32 baytlık standart bir açık anahtar alırsa, bunu yerel
Iroha anahtarı olarak kabul eder. Multihash açık anahtarı
alırsa, aşağıdaki tabloya göre işler.


Şu anda Iroha yalnızca bir multihash anahtar biçimini "anlar":

+-------------+--------+------+----------------------+
| Ad          | Etiket | Kod  | Açıklama             |
+=============+========+======+======================+
| ed25519-pub | key    | 0xed | Ed25519 açık anahtar |
+-------------+--------+------+----------------------+

Iroha'daki açık anahtarlara örnekler:

+----------------+--------+----------+-------------------------+----------------------+
| tür            | kod    | uzunluk  | veri                    | Iroha'nın tanıdığı   |
+================+========+==========+=========================+======================+
| multihash anahtar| ED01 | 20       | 62646464c35383430b...   | ed25519/sha2         |
+----------------+--------+----------+-------------------------+----------------------+
| ham 32 bayt    | --     | --       | 716fe505f69f18511a...   | ed25519/sha3         |
| anahtar        |        |          |                         |                      |
+----------------+--------+----------+-------------------------+----------------------+

`0xED` kodunun multihash biçiminin kurallarıyla `ED01` olarak
kodlandığını unutmayın.

Özel anahtarlar
---------------

Ursa'daki **özel anahtarlar**, bir özel anahtar ve bir açık
anahtarın birleştirilmesiyle temsil edilir – multihash
önekleri olmadan.
