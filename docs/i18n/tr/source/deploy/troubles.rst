.. _deploy_troubles:

=====================
Sorun Giderme
=====================

—"Lütfen yardım edin, çünkü ben…"

Iroha daemon binary'm yok
--------------------------

Iroha daemon binary'sini kaynaktan derleyebilirsiniz. Binary'leri
`buradan <https://github.com/baziorek/iroha1-cpp/releases>`__
indirebilirsiniz

Yapılandırma dosyam yok
-----------------------

Bu `bağlantıyı
<../configure/index.html>`__ takip ederek yapılandırma
dosyasının nasıl oluşturulacağını kontrol edin

genesis block'um yok
--------------------

`iroha-cli` ile oluşturarak veya `örneği
<https://github.com/baziorek/iroha1-cpp/blob/iroha1-main/example/genesis.block>`__
kullanarak manuel olarak genesis block oluşturun ve
`izinleri <../develop/api/permissions.html>`__ kontrol edin

Eş için anahtar çiftim yok
--------------------------

Hesap veya eş için anahtar çifti oluşturmak için, eş adını
`--new_account` seçeneğiyle ileterek iroha-cli binary'sini
kullanın. Örneğin:

.. code-block:: shell

    ./iroha-cli --account_name newuser@test --new_account
