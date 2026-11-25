=========
Metrikler
=========

Ağınızın performansını kullanışlı ve kapsamlı bir şekilde
izlemek için artık metrikleri kullanabilirsiniz. Bunlar Iroha
ağınız hakkında zaman içinde toplanan sayısal verilerdir.
Daha sonra verileri analiz ederek projenizi daha da verimli
hale getirebilirsiniz!

Metrikler nasıl kullanılır?
===========================

Metrikleri kullanmak için bunları `Iroha yapılandırmanıza
<../configure/index.html#deployment-specific-parameters>`__
eklemeniz ve 1.2.1'den daha yeni bir Iroha sürümü kullanmanız
gerekir.

.. note:: Iroha'yı Docker'da çalıştırıyorsanız, konteynerin
   dışından metriklere erişmek için: 1) `yapılandırmada
   <../configure/index.html>`__ ``"metrics":0.0.0.0:PORT``
   ayarlayın; 2) ``run ... -pPORT:PORT ...`` çalıştırırken Docker'da
   ilgili portu açın

Ardından, çalışan Iroha örneğinden verilere erişmek için basitçe
IP adresini kullanabilirsiniz.

İşte bir örnek:

.. code-block:: shell

  > curl http://127.0.0.1:8080/metrics

şu gibi sonuçlar verir:

.. code-block:: shell

  # HELP blocks_height Total number of blocks in chain
  # TYPE blocks_height gauge
  blocks_height 135543
  # HELP peers_number Total number peers to send transactions and request proposals
  # TYPE peers_number gauge
  peers_number 7
  # HELP number_of_domains Total number of domains in WSV
  # TYPE number_of_domains gauge
  number_of_domains 14
  # HELP total_number_of_transactions Total number of transactions in blockchain
  # TYPE total_number_of_transactions gauge
  total_number_of_transactions 216499
  # HELP number_of_signatures_in_last_block Number of signatures in last block
  # TYPE number_of_signatures_in_last_block gauge
  number_of_signatures_in_last_block 5

Multisignature transaction (MST) metrikleri
===========================================

Multisignature transaction'lar kullanıldığında, uç nokta ayrıca
imza bekleyen MST'ler için sayaçlar sağlar:

.. code-block:: shell

  # HELP number_of_pending_mst_batches Number of pending MST batches
  # TYPE number_of_pending_mst_batches gauge
  number_of_pending_mst_batches 0
  # HELP number_of_pending_mst_transactions Number of pending MST transactions
  # TYPE number_of_pending_mst_transactions gauge
  number_of_pending_mst_transactions 0

Bu ölçerler yerel bekleyen-MST önbelleğini yansıtır — yapılandırmada
``mst_enable`` ``true`` olarak ayarlanmış eşlerde (bkz.
:ref:`configuration`) en kullanışlıdır; orada MST durumu eşler
arasında paylaşılır.
