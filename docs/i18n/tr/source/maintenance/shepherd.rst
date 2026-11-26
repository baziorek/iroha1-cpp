========
Shepherd
========

Shepherd, çalışan irohad daemon'unda bakım görevlerini
gerçekleştirmeye yardımcı olan bir komut satırı yardımcı
programıdır.

Ön koşullar
===========

irohad daemon'ına erişmek için onda ``utility service``
yapılandırılmış olmalıdır. `Yapılandırma ayrıntılarına
<../configure/index.html#deployment-specific-parameters>`__ bakın.

Ardından, ``shepherd``'ı çağırırken, irohad utility servisinin
adresi ve portunu içeren ``--irohad`` komut satırı argümanını
geçirin:

.. code-block:: shell

   ./shepherd --irohad 127.0.0.1:11001 <...>

Desteklenen eylemler
====================

Ek komut satırı argümanları belirterek ``shepherd`` ile
yapabilecekleriniz şunlardır.

Kontrollü kapatma
^^^^^^^^^^^^^^^^^
Iroha'yı daha önce nasıl durdururdunuz?
Ne, gerçekten gerçekten kill mi ediyordunuz?
Ah, lütfen bir daha asla yapmayın — bu kibar ve nazik değil!

.. code-block:: shell

   ./shepherd <...> --shutdown

``--shutdown`` argümanı ile shepherd, Iroha'dan nazikçe durmasını
ister.

Çalışmasını izleme
^^^^^^^^^^^^^^^^^^
Başkalarının çalışmasını izlemek en büyük zevklerden biri olarak
geniş çapta kabul edilir. shepherd ile Iroha'nın çalışmasını
gözlemleyebilirsiniz!

.. code-block:: shell

   ./shepherd <...> --status

Bu, çalışma döngüsü durumu güncellemelerine abone olur. Daemon
başladığında, çalışırken, sonlanırken veya az önce durduğunda
belirsiz olmayan mesajlar alacaksınız.

Diğer parametreler
==================

Günlük kaydı düzeyini de ayarlayabilirsiniz:

.. code-block:: shell

   ./shepherd <...> --verbosity debug <...>

Desteklenen değerler ``trace``, ``debug``, ``info``, ``warning``,
``error`` ve ``critical``'dir.
