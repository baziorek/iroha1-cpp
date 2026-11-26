iOS Swift Kütüphanesi (bakımsız)
---------------------------------

.. note::
   Bu sayfa upstream ``iroha-ios`` kütüphanesini açıklar; bu
   kütüphane **2020'den beri bakımsızdır** (son sürüm v4.0.1) —
   Iroha 1 düğümleriyle çalışmaya devam eder ancak güncelleme
   almaz. Yeni projeler için bu fork'un aktif olarak
   bakımı yapılan :doc:`cpp` veya :doc:`python` kütüphanelerini
   tercih edin.

Kütüphane, işlem/sorgu gönderme, işlem durumları ve blok
commit'lerinin akışını içeren, iOS uygulamalarının Iroha
blockchain'iyle iletişim kurması için kullanışlı bir arayüz
sağlamak amacıyla oluşturulmuştur.

Nereden edinilir
^^^^^^^^^^^^^^^^

Iroha iOS kütüphanesi CocoaPods üzerinden mevcuttur. Yüklemek için
Podfile'ınıza şu satırı eklemeniz yeterlidir:

.. code-block:: swift

    pod 'IrohaCommunication'

Kütüphanenin kaynak kodunu `deposundan
<https://github.com/hyperledger/iroha-ios>`__ da
download edebilirsiniz


Nasıl kullanılır
^^^^^^^^^^^^^^^^

Yeni Iroha kullanıcıları için `iOS örnek projesini
<https://github.com/hyperledger/iroha-ios/tree/master/Example>`__
incelemenizi öneririz. Bilgisayarınızda yerel olarak da çalışması
gereken bir Iroha eşiyle bağlantı kurmaya çalışır; yeni bir hesap
oluşturmak ve ona biraz varlık miktarı göndermek içindir.
Projeyi çalıştırmak için aşağıdaki adımları izleyin:

- Docker konteynerinde Iroha eşini kurmak ve çalıştırmak için
  Iroha dokümantasyonundaki talimatları izleyin.

- `iroha-ios deposunu
  <https://github.com/hyperledger/iroha-ios>`__ klonlayın.

- Example dizinine gidin ve pod install çalıştırın.

- XCode'da IrohaCommunication.xcworkspace dosyasını açın.

- IrohaExample hedefini derleyin ve çalıştırın.

- Senaryonun başarıyla tamamlanıp tamamlanmadığını görmek için
  günlükleri kontrol edin.

Örnek projeyle denemeler yapmaktan çekinmeyin ve `Discord
<https://discord.com/channels/905194001349627914/905205848547155968>`__
üzerinden her türlü soruyu sormaktan çekinmeyin.
