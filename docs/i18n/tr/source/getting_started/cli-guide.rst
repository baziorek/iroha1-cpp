CLI kılavuzu: ilk işlemlerinizi ve sorgularınızı gönderme
=========================================================

.. note:: Bu sayfa ``iroha-cli``'nin zaten kullanılabilir olduğunu
  varsayar — düğüm dağıtımına dahildir. Docker, AppImage veya kaynak
  derlemesiyle nasıl elde edeceğiniz için :doc:`cli` sayfasına bakın.

Iroha ile çeşitli şekillerde etkileşebilirsiniz.
İstemci kütüphanelerimizi kullanarak Iroha ile iletişim kuran kodu
çeşitli programlama dillerinde (ör. Java, Python, Javascript, Swift)
yazabilirsiniz.
Alternatif olarak, Iroha ile etkileşim için komut satırı aracımız
``iroha-cli``'yi kullanabilirsiniz.
Bu kılavuzun bir parçası olarak ``iroha-cli`` ile tanışalım.

.. Attention:: ``iroha-cli`` tartışmasız Iroha ile çalışmaya
  başlamak için en basit yol olsa da, yalnızca olası komutların/
  sorguların bir kısmını kapsar, bu nedenle kullanıcı deneyimi en
  iyi olmayabilir. Daha iyi bir CLI sürümü oluşturmamıza yardım
  etmek isterseniz bize bildirin!

.. raw:: html

  <script src="https://asciinema.org/a/6dFA3CWHQOgaYbKfQXtzApDob.js" id="asciicast-6dFA3CWHQOgaYbKfQXtzApDob" async></script>

Yeni bir terminal açın (Iroha konteynerinin ve ``irohad``'in
çalışır durumda olması gerektiğini unutmayın) ve ``iroha`` docker
konteynerine bağlanın:

.. code-block:: shell

  docker exec -it iroha /bin/bash

Yine Iroha konteynerinin etkileşimli kabuğundasınız.
``iroha-cli``'yi başlatmamız ve istenen kullanıcının hesap adını
durmamız gerekiyor. Örneğimizde ``admin`` hesabı zaten ``test``
alanında oluşturulmuş. Bu hesabı Iroha ile çalışmak için
kullanalım.

.. code-block:: shell

  iroha-cli -account_name admin@test

.. note:: Tam hesap adında ad ile alan adı arasında ``@`` sembolü
  vardır. Anahtar çiftinin de aynı ada sahip olduğuna dikkat edin.

İlk İşlemi Oluşturma
^^^^^^^^^^^^^^^^^^^^^^

``iroha-cli`` arayüzünü şimdi görebilirsiniz.
Yeni bir varlık oluşturalım, admin hesabına biraz varlık ekleyelim
ve başka bir hesaba transfer edelim. Bunu başarmak için konsola
``tx`` veya ``1`` yazarak ``1. New transaction (tx)`` seçeneğini
seçin.

Şimdi mevcut komutların listesini görebilirsiniz.
Yeni bir varlık oluşturmaya çalışalım.
``14. Create Asset (crt_ast)`` seçin.
Şimdi varlığınız için bir ad girin, örneğin ``coolcoin``.
Ardından bir Domain ID girin. Örneğimizde zaten ``test`` adında
bir alanımız var, onu kullanalım.
Sonra bir varlık hassasiyeti girmemiz gerekiyor
– kesirli kısımdaki basamak sayısı.
Hassasiyeti ``2`` olarak ayarlayalım.

Tebrikler, ilk komutunuzu oluşturdunuz ve bir işleme eklediniz!
Ya Iroha'ya gönderebilir ya da daha fazla komut ekleyebilirsiniz
``1. Add one more command to the transaction (add)``.
Daha fazla komut ekleyelim, her şeyi tek seferde yapabilelim.
``add`` yazın.

Şimdi hesabımıza biraz ``coolcoin`` eklemeyi deneyelim.
``16. Add Asset Quantity (add_ast_qty)`` seçin, varlık ID'si —
``coolcoin#test`` — tam kısım ve hassasiyeti girin.
Örneğin 200.50 eklemek için tam kısmı ``20050`` ve hassasiyeti
``2`` olarak girin, böylece ``200.50`` olur.

.. note:: Tam varlık adında ad ile alan adı arasında ``#`` sembolü
  vardır.

``admin@test``'den ``test@test``'e 100.50 ``coolcoin`` transfer
edelim. Bir komut daha ekleyip ``5. Transfer Assets (tran_ast)``
seçin. Source Account ve Destination Account — bu durumda
``admin@test`` ve ``test@test`` — Asset ID (``coolcoin#test``),
tam kısım ve hassasiyet (sırasıyla ``10050`` ve ``2``) girin.

Şimdi işlemimizi Iroha eşine göndermemiz gerekiyor
(``2. Send to Iroha peer (send)``). Eş adresini (bu durumda
``localhost``) ve portu (``50051``) girin.
İşleminiz şimdi gönderildi ve işlem hash'inizi görebilirsiniz.
Bunu işlemin durumunu kontrol etmek için kullanabilirsiniz.

``irohad``'in çalıştığı terminale dönün.
İşleminizin günlüklerini görebilirsiniz.

Yaşasın! İlk işleminizi Iroha'ya gönderdiniz.

İlk Sorguyu Oluşturma
^^^^^^^^^^^^^^^^^^^^^^

Şimdi ``coolcoin``'lerin ``admin@test``'den ``test@test``'e başarıyl
a transfer edilip edilmediğini kontrol edelim.
``2. New query (qry)`` seçin.
``8. Get Account's Assets (get_acc_ast)`` ``test@test``'in artık
``coolcoin``'e sahip olup olmadığını kontrol etmenize yardımcı
olabilir. Komutlarda yaptığınız gibi benzer şekilde bir sorgu
oluşturun ve ``1. Send to Iroha peer (send)`` kullanın.
Artık ``test@test``'in kaç ``coolcoin``'e sahip olduğunu
görebilirsiniz. Şuna benzer görünecektir:

.. code-block:: shell

  [2018-03-21 12:33:23.179275525][th:36][info] QueryResponseHandler [Account Assets]
  [2018-03-21 12:33:23.179329199][th:36][info] QueryResponseHandler -Account Id:- test@test
  [2018-03-21 12:33:23.179338394][th:36][info] QueryResponseHandler -Asset Id- coolcoin#test
  [2018-03-21 12:33:23.179387969][th:36][info] QueryResponseHandler -Balance- 100.50

Harika değil mi?
İlk sorgunuzu Iroha'ya gönderdiniz ve yanıt aldınız!

.. hint:: Mevcut tüm komutlar ve sorgular hakkında bilgi almak için
  API bölümümüze bakın.

Kötü Olmak
^^^^^^^^^^

Kötü olmayı deneyelim ve Iroha'yı kandıralım. Örneğin
``admin@test``'nin sahip olduğundan daha fazla ``coolcoin``
transfer etmeye çalışalım.
``admin@test``'den ``test@test``'e 100000.00 ``coolcoin``
transfer etmeyi deneyin. Yine ``1. New transaction (tx)``,
``5. Transfer Assets (tran_ast)`` ile devam edin, Source Account
ve Destination Account — bu durumda ``admin@test`` ve
``test@test`` — Asset ID (``coolcoin#test``), tam kısım ve
hassasiyet (sırasıyla ``10000000`` ve ``2``) girin.
Önceki gibi işlemi Iroha eşine gönderin.
Şöyle diyor:

.. code::

  [2018-03-21 12:58:40.791297963][th:520][info] TransactionResponseHandler Transaction successfully sent
  Congratulation, your transaction was accepted for processing.
  Its hash is fc1c23f2de1b6fccbfe1166805e31697118b57d7bb5b1f583f2d96e78f60c241

`İşleminiz işleme alınmak üzere kabul edildi`.
Bu Iroha'yı başarıyla kandırdığımız anlamına mı geliyor?
İşlemin durumunu görmeyi deneyelim.
``3. New transaction status request (st)`` seçin ve önceki
komuttan sonra konsolda alabileceğiniz işlem hash'ini girin.
Iroha'ya gönderin.
Şöyle yanıt veriyor:

.. code::

  Transaction has not passed stateful validation.

Görünüşe göre hayır.
İşlemimiz stateful validation'ı geçemediği için kabul edilmedi ve
``coolcoin``'ler transfer edilmedi.
Emin olmak için sorgularla ``admin@test`` ve ``test@test``'in
durumunu kontrol edebilirsiniz (daha önce yaptığımız gibi).

``iroha-cli`` seçenekleri
-------------------------

Yukarıda gösterilen etkileşimli moda ek olarak, ``iroha-cli``
etkileşimsiz ve betikli kullanım için komut satırı bayrakları
kabul eder:

+==========================+==============================================================+
| Bayrak                   | Açıklama                                                     |
+==========================+==============================================================+
| ``-account_name``        | üzerinde çalışılacak hesap ID'si (örn. ``admin@test``)       |
+==========================+==============================================================+
| ``-new_account``         | ilk çalıştırmada hesap anahtar çiftini oluştur               |
+==========================+==============================================================+
| ``-key_path``            | ``<name>.pub``/``<name>.priv`` anahtarlarını içeren dizin    |
+==========================+==============================================================+
| ``-pass_phrase``         | şifrelenmiş özel anahtarlar için parola                      |
+==========================+==============================================================+
| ``-peer_ip``             | Iroha düğüm adresi (varsayılan ``127.0.0.1``)                |
+==========================+==============================================================+
| ``-torii_port``          | düğümün Torii portu (varsayılan ``50051``)                   |
+==========================+==============================================================+
| ``-tls_root_cert``       | PEM kök sertifikasına yol; TLS transport'u etkinleştirir     |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
| ``-json_transaction``    | etkileşimli yerine JSON olarak tanımlanan bir işlem gönderir  |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
| ``-json_query``          | etkileşimli yerine JSON olarak tanımlanan bir sorgu gönderir  |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
| ``-genesis_block``       | genesis block dosyası üretir ve çıkar                        |
+==========================+==============================================================+
| ``-genesis_transaction`` | genesis block üretilirken kullanılan işlem                   |
+==========================+==============================================================+
| ``-peers_address``       | üretilen genesis block'ta kullanılan eş listesi              |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
| ``-interactive``         | etkileşimli kabuğu çalıştırır (varsayılan ``true``; tek       |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
|                          | seferlik JSON modu için ``false`` ayarlayın)                 |
+--------------------------+--------------------------------------------------------------+

Örneğin, etkileşimli kabuk olmadan önceden oluşturulmuş bir JSON
işlemi göndermek için:

.. code-block:: shell

  iroha-cli -account_name admin@test \
            -peer_ip 127.0.0.1 -torii_port 50051 \
            -json_transaction tx.json -interactive=false

Düğümün Torii uç noktası TLS etkinleştirilmişse (bkz.
:doc:`../configure/torii-tls`), ``-tls_root_cert``'i düğümün PEM
sertifikasına yönlendirin — aynı bayrak etkileşimli modda da
geçerlidir.
