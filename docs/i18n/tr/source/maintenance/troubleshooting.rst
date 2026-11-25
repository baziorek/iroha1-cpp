===============
Sorun Giderme
===============

Sert kapatma sonrası kurtarma
------------------------------

::

	[2020-11-27 10:40:01.764013860][th:1][warning] Irohad/Storage/FlatFileBlockStorage Error while block deserialization: Unexpected end of string. Expected a value.

	^

::

	[2020-11-25 10:36:19.993552669][C][Init]: Irohad startup failed: WSV state (height 4576773) is more recent than block storage (height 4576772).

Bu tür mesajlar, dosya tabanlı blok deposu kullanılırken düğüm
çökerse görünebilir. Eksik blokları başka bir düğümden kopyalayın
— bu durumda hızlı başlatma için ``--reuse_state`` bayrağını
kullanabilirsiniz — veya boş dosyadan başlayarak blokları silin
ve mevcut bloklardan WSV'yi yeniden oluşturun.

Uzun boşta kalma sürelerinden sonra istemci kesintileri veya
"Rendezvous" hataları
-------------------------------------------------------------------

İstemciler (özellikle uzun ömürlü durum akışları) uzun
etkisiz dönemlerden sonra Torii bağlantısını periyodik olarak
kaybediyorsa, neden genellikle ara bir hop'taki (NAT, yük
dengeleyici, proxy) boşta kalma zaman aşımıdır ve HTTP/2
bağlantısını sessizce keser.

Bu fork'un ``irohad``'i tüm sunucularında (Torii, internal
ordering, utility) **gRPC keepalive ping**'lerini etkinleştirir:
sunucu, boşta kalan bağlantıları 60 saniyede bir ping'ler ve 20
saniyelik zaman aşımından sonra ölü bağlantıları kapatır, böylece
istemciler yarı açık kanalda asılı kalmak yerine temiz şekilde
yeniden bağlanır.

Hâlâ kesintiler görüyorsanız:

- keepalive düzeltmesini içeren bir derleme kullandığınızdan emin
  olun (bu fork, ``iroha1-main`` dalı);
- istemci ile düğüm arasındaki proxy/LB'nin keepalive aralığından
  daha kısa bir boşta kalma zaman aşımı olup olmadığını kontrol
  edin ve artırın;
- durum akışları için geri çekilmeli yeniden bağlanmayı
  tercih edin — :ref:`configuration` içindeki
  ``stale_stream_max_rounds``'a bakın.
