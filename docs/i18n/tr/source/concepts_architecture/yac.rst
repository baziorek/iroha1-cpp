.. _yac:

=========================
YAC konsensüs algoritması
=========================

Bu bölümde, Iroha'nın düğümler arasında ledger durumunda nasıl
mutabık kaldığını — YAC (Yet Another Consensus) mekanizmasının bir
parçasını — açıklıyoruz.

Konsensüs nedir?
----------------

Iroha, eşit düğümlerden oluşan bir ağ olarak çalışır. Bir kullanıcı
işlem gönderdiğinde, bu işlemin ortak sıralı bir listeye (blok/
öneri) dahil edilmesi ve tüm düğümler tarafından uygulanması
gerekir. "Konsensüs", düğümlerin bu listenin içeriği ve sırası
hakkında mutabık kalma sürecidir.

Iroha, YAC'ın bir varyantını kullanır — tipik durum için hızlı
yol optimizasyonlarıyla BFT tarzı, tur tabanlı bir protokolü
birleştiren bir algoritma.

Temel kavramlar
---------------

**Ordering peers ve syncing peers**
    Genesis block'ta veya AddPeer komutuyla eklenen tüm peer'lar
    ya **ordering peer** (konsensüse katılır) ya da **syncing
    peer** (salt okunur) olur. Syncing peer oy vermez ve quorum'a
    girmaz — doğrulanmış blokları doğrudan alır ve öneri oluşturmaz.
    ``Peer.syncing_peer`` alanı peer'ın sınıfını belirler; glossary
    kaydına bakın.

**Rounds**
    Konsensüs "turlar" halinde ilerler. Her turda ordering peer'lar
    bir lider (proposer) seçer; lider işlemleri bir öneride
    toplar ve diğerlerine oylama için gönderir. Bir öneri quorum
    elde edemezse yeni bir tur başlar.

**Vote / Commit**
    Her peer, öneri için "commit" veya "reject" oyu verir.
    Commit imzalarının quorum'u toplandığında öneri kesinleşir —
    blok ledger'a yazılır. Bu finalitedir: kesinleşen bloktaki
    işlemler geri alınmaz.

**Byzantine-fault tolerance**
    YAC, Byzantine arızalarına — keyfi veya düşmanca davranan
    düğümlere — dayanacak şekilde tasarlanmıştır. Teorik olarak N ≥
    3f + 1 iken f düğüme kadar dayanır. Ancak standart Iroha derlemesi
    YAC'ı **CFT** (crash-fault tolerant, basit çoğunluk) modunda
    çalıştırır — çünkü konsensüs modeli parametresi derleme zamanı
    sabitidir. Ayrıntılar aşağıda.

CFT vs BFT modu
---------------

YAC kodu her iki modeli de destekler:

- ``YACMode::Cft`` — crash-fault tolerant: dürüst düğümlerin
  çoğunluğunun oyları beklenir
- ``YACMode::Bft`` — byzantine-fault tolerant: 2f + 1 quorum
  gerekir

Ancak değer ``kCft`` derleme zamanı sabitiyle belirlenir —
varsayılan olarak Iroha derlemesi CFT kullanır. BFT'ye geçmek için
``kCft = false`` ile yeniden derleme gerekir. Bu, tipik bir Iroha
1 dağıtımının kötü niyetli düğümlere değil çökmelere karşı
korunduğu anlamına gelir.

Performans ve işlem akışı
-------------------------

- Ordering peer'lar gRPC ``internal_port`` (varsayılan ``10001``)
  üzerinden koordine olur.
- İşlemler önce ``torii_port`` (istemci arayüzü) üzerinden geçer,
  kuyruğa girer ve ardından önerilere dahil edilir.
- Yapılandırmadaki ``vote_delay`` ve ``proposal_creation_timeout``
  parametreleri tur zamanlamalarını kontrol eder; bkz.
  :doc:`../configure/index`.

Senkron düğümler ve konsensüs
-----------------------------

Senkron peer (bkz. :doc:`../deploy/syncing-node`) turlara katılmaz
— ordering peer'lardan aldığı zaten kesinleşmiş blokları uygular
ve WSV'sini günceller. Bu, konsensüs gecikmesini artırmadan okuma
repikaları tutmayı sağlar.

Ayrıca bakınız
--------------

- :doc:`architecture` — genel mimari (ordering, WSV, Torii)
- :doc:`../deploy/flags` — konsensüsü etkileyen bayraklar
  (``mst_enable``, ``proposal_creation_timeout`` vb.)
