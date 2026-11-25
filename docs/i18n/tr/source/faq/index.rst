===
FAQ
===

Yenisiyim. Nereden başlamalıyım?
--------------------------------

Merhaba yeni başlayan! Hoş geldiniz :)
Iroha ile başlamak için 2 yol var:

 1. `Hızlı başlangıç kılavuzumuzu
    <../getting_started/index.html>`_ takip ederek basit bir örnek
    ağ kurarak Iroha'nın ne olduğunu ve nasıl çalıştığını
    görebilirsiniz
 2. `Iroha'nın temel kavramlarıyla
    <../concepts_architecture/index.html>`_ tanışabilir ve kendi
    `Iroha ağınızı <../deploy/index.html>`_ kurmaya
    başlayabilirsiniz

Artık sizin bir Iroha blokzinciriniz var! Tebrikler!
Hakkında sorularınız varsa, topluluğumuza buradan ulaşmaktan
çekinmeyin: https://chat.hyperledger.org/channel/iroha

Hangi tür veriler aktarılabilir?
--------------------------------

Hyperledger Iroha yalnızca varlıkları değil (böyle varlıkları
sunmak için oldukça gelişmiş komut ve sorgu seti nedeniyle böyle
bir izlenim edinebilirsiniz), aynı zamanda zincire kaydedilecek
herhangi bir veriyi göndermenize olanak tanır.

Mevcut uygulama bu olanağı en azından `SetAccountDetail
<../develop/api/commands.html#set-account-detail>`_ komutu ve
`GetAccountDetail
<../develop/api/queries.html#get-account-detail>`_ sorgusu
aracılığıyla sağlar.

Mobil cihaz düğüm olabilir mi?
------------------------------

"Mobil cihaz" ile neyi kastettiğinize bağlı olarak iki seçenek
vardır.

ARM tabanlı donanım üzerinde bir Linux (Raspberry PI gibi) veya
root'lanmış bir Android cihaz kastediliyorsa, o cihazda Iroha'yı
düğüm (ağdaki peer) olarak çalıştırmak mümkündür. Bu durumda
Iroha, platforma özgü bir ikili dosya olarak çalışır.

Fabrika çıkışı arayüzü (GUI) el değmemiş standart bir iOS veya
Android cihaz kastediliyorsa, genel olarak mümkün değildir ve bu
konuda talimat sağlamıyoruz. Yine de Iroha kullanan mobil
uygulamalar oluşturabilirsiniz — bunlar Iroha istemcileri olur ve
peer olarak hizmet vermezler.

Iroha'yı ARM cihazında çalıştırmak için hedef platformda derlemeniz
gerekir. Iroha'nın derlenmesi kayda değer miktarda RAM gerektirir —
32-bit ARM ana bilgisayar için 8 GB RAM gerekir. Derleme bir Docker
konteyneri içinde yapılabilir. Konteyneri hazırlamak için:

1. Iroha git reposunu klonlayın:
   https://github.com/baziorek/iroha1-cpp
2. `iroha1-cpp/docker/develop` içinde `docker build -t iroha-build-env .`
   çalıştırın
3. Yeni derlenen konteyneri başlatın ve içinde Iroha'nın kendisini
   derleyin

Iroha git deposu klasörünü konteynere mount etmeyi unutmayın.

Verim (TPS) nedir? Performans testi sonuçları var mı?
-----------------------------------------------------

Iroha ağınızın verimliliği yapılandırmaya, donanıma ve düğüm
sayısına bağlıdır. `test/load` dizinindeki yük testini denemenizi
ve sonuçları bildirmenizi memnuniyetle karşılarız.

Iroha ile Iroha 2 arasındaki fark nedir?
-----------------------------------------

Iroha 2, konsensüs mekanizmasında birkaç büyük değişiklikle Iroha'nın
Rust ile yeniden yazımıdır. `Iroha 2 belgeleri
<https://hyperledger.github.io/iroha-2-docs/guide/iroha-2.html>`_
iki proje arasındaki farkların bir özetini sunar.
