.. _iroha-cli:

======================
``iroha-cli``
======================

``iroha-cli``, Iroha'nın komut satırı istemcisidir: kod yazmadan bir
peer'a işlem ve sorgu göndermek için etkileşimli bir araçtır.
``irohad`` ile birlikte her dağıtımda gelir — zaten sahip
olduğunuz olanı seçin.

``iroha-cli`` edinme
--------------------

Docker
^^^^^^

Düğümü resmi imajdan çalıştırıyorsanız, ``iroha-cli`` zaten
konteynerin içinde:

.. code-block:: shell

  docker exec -it iroha iroha-cli -account_name admin@test

``-account_name`` argümanı, işlemlerin hangi anahtar çiftiyle
imzalanacağını seçer — bu örnekte yapılandırmadaki ``admin@test``
(anahtar, ``example/`` içindeki diğer dosyalarla birlikte bulunur).

AppImage
^^^^^^^^

`GitHub Releases
<https://github.com/baziorek/iroha1-cpp/releases>`__ sayfasından
``iroha-cli-x86_64.AppImage``'ı ``irohad`` ile birlikte indirin,
çalıştırılabilir yapın ve başlatın:

.. code-block:: shell

  chmod +x iroha-cli-x86_64.AppImage
  ./iroha-cli-x86_64.AppImage -account_name admin@test

.. note:: AppImage ``iroha-cli`` ``1.6.1`` sürümüyle birlikte
  yayınlanır — bkz. :ref:`node-appimage`.

Kaynak koddan derleme
^^^^^^^^^^^^^^^^^^^^^

:ref:`Iroha'yı kaynaktan derleme <node-build>` sırasında
``iroha-cli``, ``irohad``'ın yanında üretilir:

.. code-block:: shell

  ./build/bin/iroha-cli -account_name admin@test

İlk çalıştırma
--------------

Dağıtımdan bağımsız olarak, ilk çalıştırmada peer'ın adresini ve
portunu sorar — yerel düğüm için bu ``localhost`` ve ``50051``'dir
(düğüm yapılandırmasındaki ``torii_port``). Ardından etkileşimli
menü işlem (``tx``) ve sorgu (``qry``) oluşturmanıza, peer'a
göndermenize ve işlem durumlarını kontrol etmenize olanak tanır.

.. note:: ``iroha-cli`` yalnızca en yaygın komutları kapsayan bir
  yardımcı araçtır, tam bir API değildir. İlk işlemi gönderme
  konusunda ayrıntılı kılavuz :doc:`cli-guide`'dadır; programatik
  erişim :doc:`python-guide` ve `API referansında
  <../develop/api/commands.html>`_ açıklanmıştır.
