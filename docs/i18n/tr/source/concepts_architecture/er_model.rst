Varlık-ilişki modeli
====================

Her Hyperledger Iroha eşi, "World State View" olarak adlandırılan
bir duruma sahiptir; bu, varlıkların ve aralarındaki ilişkilerin
bir kümesiyle temsil edilir. Sistemde hangi varlıkların bulunduğunu
ve aralarında hangi ilişkilerin olduğunu açıklamak için bu bölüm ER
diyagramını ve bileşenlerinin açıklamasını içerir.

ER diyagramı
-----------

.. image:: ./../../image_assets/er-model.png

Peer
----

 - address — ağ adresi ve dahili port; senkronizasyon, konsensüs
   ve ordering servisiyle iletişim için kullanılır
 - public_key — konsensüs süreci sırasında blokları imzalamak için
   kullanılacak anahtar

Asset
-----

 - asset_id — varlığın tanımlayıcısı, asset_name#domain_id
   biçiminde
 - domain_id — varlığın oluşturulduğu alanın tanımlayıcısı,
   mevcut bir alana referans
 - precision — kesirli kısmın boyutu

Signatory
---------

 - public_key — açık anahtar

Domain
------

 - domain_id — alanın tanımlayıcısı
 - default_role — alanda oluşturulan kullanıcı başına varsayılan
   rol; mevcut bir role referans

Role
----

 - role_id — rolün tanımlayıcısı

RoleHasPermissions
^^^^^^^^^^^^^^^^^^

 - role_id — rolün tanımlayıcısı; mevcut bir role referans
 - permission_id — iznin tanımlayıcısı

Account
-------

 - account_id — hesabın tanımlayıcısı, account_name@domain_id
   biçiminde
 - domain_id — hesabın oluşturulduğu alanın tanımlayıcısı,
   mevcut bir alana referans
 - quorum — bu hesaptan geçerli bir transaction oluşturmak için
   gereken imzalayan sayısı
 - transaction_count – bu hesap tarafından oluşturulan
   transaction'ların sayacı
 - data — hesapla ilgili herhangi bir bilgi için anahtar-değer
   deposu. Boyut 268435455 bayt (0x0FFFFFFF) ile sınırlıdır
   (PostgreSQL JSONB alanı).

AccountHasSignatory
^^^^^^^^^^^^^^^^^^^

 - account_id — hesabın tanımlayıcısı; mevcut bir hesaba referans
 - public_key — açık anahtar (imzalayan olarak da adlandırılır);
   mevcut bir imzalayana referans

AccountHasAsset
^^^^^^^^^^^^^^^

 - account_id — hesabın tanımlayıcısı; mevcut bir hesaba referans
 - asset_id — varlığın tanımlayıcısı; mevcut bir varlığa referans
 - amount — hesaba ait varlık miktarı

AccountHasRoles
^^^^^^^^^^^^^^^

 - account_id — hesabın tanımlayıcısı; mevcut bir hesaba referans
 - role_id — rolün tanımlayıcısı; mevcut bir role referans

AccountHasGrantablePermissions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

 - account_id — hesabın tanımlayıcısı; mevcut bir hesaba referans.
   Bu hesap, izin verilen kişiye kendisi üzerinde işlem yapma
   yetkisi veren bir izin verir.
 - permittee_account_id — hesabın tanımlayıcısı; mevcut bir hesaba
   referans. Bu hesaba account_id üzerinde işlem yapma izni
   verilir.
 - permission_id — grantable_permission tanımlayıcısı
