============
Eş Ekleme
============

HL Iroha'da ağ çalışırken yeni eşler ekleyebilirsiniz. Bu, özel
bir komut olan `AddPeer
<../develop/api/commands.html#add-peer>`_ kullanılarak yapılır.

Gereksinimler
=============

**Ağda zaten bulunan eşlerdekiyle aynı Genesis Block'u (blockchain'in
başlangıç bloğu) ile çalışan bir eş olmalıdır:**

— çözümlenebilir bir adrese sahip olmalı;

— bir eş anahtar çiftine (SHA-2/SHA-3 ile Ed25519) sahip olmalı

.. important:: Eş ekleyen transaction'ı gönderen hesabın rolünde
   `Can Add Peer
   <../develop/api/permissions.html#can-add-peer>`_ izni olmalı; eş
   kaldırmak için `Can Remove Peer
   <../develop/api/permissions.html#can-remove-peer>`_ izni
   olmalıdır — bu genesis block'ta ayarlanmalıdır.

Kullanım
========

`API referansında <../develop/api/commands.html#add-peer>`__
açıklandığı gibi, komutu kullanmak için yalnızca şunlara
ihtiyacınız vardır:

— ağa eklemek istediğiniz eşin açık anahtarı;

— eşin çözümlenebilir IP adresi

Adımlar:

1. Kullanıcıya atanan genesis block'ta `Can Add Peer
   <../develop/api/permissions.html#can-add-peer>`_ ve `Can
   Remove Peer
   <../develop/api/permissions.html#can-remove-peer>`_
   izinleri ayarlanmış bir ağ oluşturun;
2. Aynı genesis block ve benzer yapılandırmayla HL Iroha
   çalıştıran başka bir eş oluşturun;
3. Gerekli izinlere sahip hesaptan ``add peer`` komutunu içeren
   bir transaction gönderin (aşağıdaki örneğe bakın)
4. Her şeyin doğru çalıştığını görmek için eşlerin günlüklerini
   kontrol edin.

Düğümlerdeki blok sayısının aynı olup olmadığını kontrol ederek de
her şeyin yolunda olduğundan emin olabilirsiniz.

.. note:: Yalnızca 1 mevcut eş çalışıyorsa, eklediğiniz eşleri,
   tüm eşleri (hem mevcut hem yeni olanları) `yapılandırmadaki
   <../configure/index.html#environment-specific-parameters>`__
   "initial_peers" parametresinde bulunduracak şekilde
   yapılandırmanız gerekecektir. Buna ihtiyaç duyulan diğer bir
   durum, ağın bir süredir çalıştığı ve genesis block'ta belirtilen
   eşlerin artık bulunmadığı durumdur (yeni eşler eklenirken Remove
   Peer komutu kullanılarak kaldırıldıkları için).

Örnek
=====

Python'da bir komutun nasıl görünebileceğini gösteriyoruz. Bu
örnekte `Can Add Peer
<../develop/api/permissions.html#can-add-peer>`_ ve `Can Remove
Peer <../develop/api/permissions.html#can-remove-peer>`_
dahil tüm izinlere sahip olan `Root iznini
<../develop/api/permissions.html#root>`__ kullandık:

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

Eş Kaldırma
===========

Eşi kaldırmak için ``CanRemovePeer`` iznine sahip hesaptan `Remove
Peer <../develop/api/commands.html#remove-peer>`_ komutunu
kullanmanız gerekecektir.
