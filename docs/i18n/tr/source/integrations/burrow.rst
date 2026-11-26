HL Burrow Entegrasyonu
======================

Iroha bakımcıları olarak, kullanıcılarımızdan özel
akıllı sözleşme desteği için birçok soru ve istek
aldık. İş ihtiyaçlarını gerçekleştirmede onlara daha
fazla özgürlük sağlamak için Hyperledger
serasinden başka harika bir proje olan HL Burrow
EVM'yi Iroha ile entegre ettik.

.. note:: Iroha bağlamında HL Burrow, Solidity
   akıllı sözleşmelerini çalıştırabilen bir Ethereum
   Sanal Makinesi sağlar. Size mümkün olan en iyi
   kullanıcı deneyimini sunmak için elimizden geleni
   yaptık – ve Iroha ile kullanmak için yalnızca
   `Iroha derlemesi sırasında bir CMake bayrağı
   eklemeniz <../build/index.html#cmake-parameters>`_
   yeterli, hemen çalışmaya başlar.

   `Hyperledger Burrow <https://hyperledger.github.io/burrow/#/>`_
   Entegrasyonu ile derlenmiş resmi docker imajları
   vardır. Bunları kullanmak için
   ``hyperledger/iroha:latest`` yerine
   ``hyperledger/iroha-burrow:latest`` kullanın.

Bu dilde yeniseniz Solidity akıllı sözleşme dilini
`tutarda <https://solidity.readthedocs.io/>`_
okuyabilirsiniz.

Nasıl çalışır
-------------

Bu entegrasyon için Iroha'da özel bir `Call Engine
komutu
<../develop/api/commands.html#call-engine>`_ ve
komutun sonuçlarını almak için özel bir `Engine
Receipts sorgusu
<../develop/api/queries.html#engine-receipts>`_
olşturduk.

Komut
^^^^^

Komutta şunları yapabilirsiniz:

**EVM'de yeni bir sözleşme hesabı oluşturma**

`CallEngine <../develop/api/commands.html#call-engine>`_
içinde *callee* belirtilmezse ve *input* parametresi
bir bytecode içeriyorsa, yeni bir sözleşme hesabı
oluşturulur.

**Daha önce dağıtılmış bir sözleşmenin metodunu
çağırma**

*callee* belirtilirse, input, callee sözleşmesinin
bir metodunun ABI ile kodlanmış seçicisi ve ardından
argümanlar olarak işlenir.

.. hint:: `eth_sendTransaction
   <https://github.com/ethereum/wiki/wiki/JSON-RPC#eth_sendtransaction>`_
   mesaj çağrısının `data` alanının içeriğine bağlı
   olarak Ethereum'da bir sözleşme dağıtmaya veya bir
   sözleşme fonksiyonunu çağırmaya çok benzer.
   Ayrıntılar için `ABI spesifikasyonuna
   <https://solidity.readthedocs.io/en/v0.6.5/abi-spec.html>`_
   bakın.

Sorgu
^^^^^

Bir `CallEngine
<../develop/api/commands.html#call-engine>`_
komutunun sonucunu sorgulamak için `Engine
Receipts sorgusu
<../develop/api/queries.html#engine-receipts>`_
kullanılmalıdır. EVM içindeki hesaplamaların çıktısı,
deftere yazılana (yani ilgili Iroha işlemini içeren
bloğun taahhüt edilmesine) kadar çağırana
kullanılamaz. Diğer `verilerin
<../develop/api/queries.html#response-structure>`_
ardından, *EngineReceipts* sorgusu *CallEngine*
yürütmesi sırasında EVM'de oluşturulan günlük girdilerinin
dizisini döndürür.

.. hint:: dApp geliştiricileri için, sözleşme
   yürütmesinin sonucunu ilgili tarafların görmesini
   sağlamanın yaygın bir yolu, bir sözleşme
   fonksiyonundan çıkmadan önce bazı verilerle bir
   olay yayınlamaktır, böylece bu veriler *Event
   Log*'a yazılır. `Ethereum Sarı
   Makalesi <https://ethereum.github.io/yellowpaper/paper.pdf>`_
   günlük girdisini, yayıncının adresini, 32 baytlık
   konular dizisini ve bazı verilerin bayt dizisini
   içeren 3'lü olarak tanımlar.

Ordering ve değerlerin listesi olan argümanların
aktarımı, parametrelerle JSON dizesi gönderilerek
yapılır. Daha fazla örnek için örneklere bakın.

EVM'de Yerel Iroha Komutlarını Çalıştırma
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

HL Burrow entegrasyonu ile Iroha durumunu
değiştirmek için yerel komutları da kullanabilirsiniz.

Burrow EVM'nin entegrasyon mekanizması, Iroha
uygulama geliştiricilerini akıllı sözleşme kodundan
doğrudan Iroha durumuna etki edebilen bir araçla
güçlendirir ve bu da yerleşik Iroha komutları
sisteminin programlanabilir iş mantığı uzantıları
için temel sağlar. Koşullu varlık transferleri,
işlem ücretleri, değiştirilemez varlıklar vb.
böyle uzantılara yalnızca birkaç örnektir. Buradaki
zor kısım, Iroha veri modelinin Ethereum'un
kine kıyasla oldukça farklı olmasıdır. Örneğin
Ethereum'da yalnızca bir tür yerleşik varlık
(``Eth``) vardır, bu nedenle EVM bağlamında bir
hesap bakiyesini almak basitçe hesabın bakiye
özelliğini döndürmek anlamına gelir. Buna karşılık
Iroha'da bir hesap birden fazla varlığa sahip
olabilir veya hiç varlığı olmayabilir, bu nedenle bir
hesap bakiyesi döndüren herhangi bir fonksiyon en
az bir ek argüman — varlık kimliği — kabul
etmelidir. Aynı mantık hesaptan hesaba varlık
transfer etme/gönderme için de geçerlidir.

Bu veri modeli uyumsuzluğu sorununa bir çözüm olarak
Burrow'da "Iroha veri modelinin farkında" olan ve
Iroha durumuyla etkileşim için API sunan sözde
Service Contracts'ı sunuyoruz (bakiyeleri sorgulama,
varlıkları transfer etme vb.).

.. note:: Native'ler ve harici dispatcher'lar hakkında
   daha fazla bilgi için `Burrow
   belgelerini
   <https://wiki.hyperledger.org/display/burrow/Burrow+-+The+Boring+Blockchain>`_
   kontrol edebilirsiniz.

Şematik olarak sistemin farklı bölümleri arasındaki
etkileşim şöyle görünür:

.. image:: ../../image_assets/burrow/natives.svg

.. attention::
   Burrow EVM perspektifinden böyle bir Service
   Contract yerel harici bir VM'de barındırılır ve
   kendisinin EVM'deki özel bir adreste
   dağıtılmışmış gibi aynı arabirimler aracılığıyla
   çağrılabilir. Bu metotlar özellikle Iroha
   entegrasyonu için kullanılır, bu nedenle Service
   Contract'ın adresi yalnızca Iroha üzerinden
   çalışırken bulunabilir.

Iroha EVM sarmalayıcısının mevcut sürümü, Iroha
komutlarını ve sorgularını açığa çıkaran tek bir
hizmet sözleşmesi içerir; bu sözleşme
``A6ABC17819738299B3B2C1CE46D55C74F04E290C``
adresinde dağıtılmıştır (*ServiceContract* dizesinin
*keccak256* hash'inin son 20 baytı) ve aşağıdaki
Iroha komutlarını ve sorgularını açığa çıkarır:

.. list-table:: Entegre Edilen Komutlar/Sorgular
   :widths: 3 35 35
   :header-rows: 1

   * - S No.
     - Açığa çıkarılan API metodlarının imzası
     - Argümanlar
   * - 1
     - `addAssetQuantity(string,string) <../develop/api/commands.html#add-asset-quantity>`_
     - asset_id, amount
   * - 2
     - `addPeer(string,string) <../develop/api/commands.html#add-peer>`_
     - address, peer_key
   * - 3
     - `addSignatory(string,string) <../develop/api/commands.html#add-signatory>`_
     - account_id, public_key
   * - 4
     - `appendRole(string,string) <../develop/api/commands.html#append-role>`_
     - account_id, role_name
   * - 5
     - `createAccount(string,string,string) <../develop/api/commands.html#create-account>`_
     - account_name, domain_id, public_key
   * - 6
     - `createAsset(string,string,string) <../develop/api/commands.html#create-asset>`_
     - asset_name, domain_id, precision
   * - 7
     - `createDomain(string,string) <../develop/api/commands.html#create-domain>`_
     - domain_id, default_role
   * - 8
     - `detachRole(string,string) <../develop/api/commands.html#detach-role>`_
     - account_id, role_name
   * - 9
     - `removePeer(string) <../develop/api/commands.html#remove-peer>`_
     - public_key
   * - 10
     - `removeSignatory(string,string) <../develop/api/commands.html#remove-signatory>`_
     - account_id, public_key
   * - 11
     - `setAccountDetail(string,string,string) <../develop/api/commands.html#set-account-detail>`_
     - account_id, key, value
   * - 12
     - `setAccountQuorum(string,string) <../develop/api/commands.html#set-account-quorum>`_
     - account_id, quorum
   * - 13
     - `subtractAssetQuantity(string,string) <../develop/api/commands.html#subtract-asset-quantity>`_
     - asset_id, amount
   * - 14
     - `transferAsset(string,string,string,string,string) <../develop/api/commands.html#transfer-asset>`_
     - src_account_id, dest_account_id, asset_id, description, amount
   * - 15
     - `getAccount(string) <../develop/api/queries.html#get-account>`_
     - account_id
   * - 16
     - `getBlock(string) <../develop/api/queries.html#get-block>`_
     - height
   * - 17
     - `getSignatories(string) <../develop/api/queries.html#get-signatories>`_
     - account_id
   * - 18
     - `getAssetBalance(string,string) <../develop/api/queries.html#get-account-assets>`_
     - account_id, asset_id
   * - 19
     - `getAccountDetail() <../develop/api/queries.html#get-account-detail>`_
     -
   * - 20
     - `getAssetInfo(string) <../develop/api/queries.html#get-asset-info>`_
     - asset_id
   * - 21
     - `getRoles() <../develop/api/queries.html#get-roles>`_
     -
   * - 22
     - `getRolePermissions(string) <../develop/api/queries.html#get-role-permissions>`_
     - role_id
   * - 23
     - `getPeers() <../develop/api/queries.html#get-peers>`_
     -


.. hint:: Bir geliştiricinin perspektifinden yerel bir
  sözleşmenin fonksiyonunu çağırmak, latter'ın adresi
  bilindiği sürece herhangi bir diğer akıllı
  sözleşmenin metodunu çağırmaktan farklı değildir:

	bytes memory payload = abi.encodeWithSignature("getAssetBalance(string,string)", "myacc@test", "coin#test");

	(bool success, bytes memory ret) = address(0xA6ABC17819738299B3B2C1CE46D55C74F04E290C).delegatecall(payload);

Burada özel bir EVM mesaj çağrısı türü kullanılır —
**delegatecall**, bir sözleşmenin çalışma zamanında
farklı bir adresteki kodu kendi yürütme bağlamında
dinamik olarak yüklemesine ve çalıştırmasına izin
verir.

.. seealso:: Şimdi `kullanım
  örneklerine <burrow_example.html>`_ geçelim.
