.. raw:: html

    <style> .red {color:#aa0060; font-weight:bold; font-size:16px} </style>

.. role:: red

Mevcut WSV ile Iroha düğümünü yeniden başlatma
=============================================

Önceden, bir düğümü güncellemeniz gerektiğinde veya herhangi bir
nedenden kapandığında, tutarlı bir `world state view (WSV olarak
da bilinir)
<../concepts_architecture/architecture.html#world-state-view>`__
'ü yeniden oluşturmak için tüm blokları yeniden okumak tek seçenekti.
Bir düğümü daha hızlı başlatmak için, artık hızlı bir kontrolden
sonra mevcut WSV veritabanını yeniden kullanmak mümkündür. Bunun
için üst bloğun ``hash``'i ve blockstorage'ın ``height``'i WSV'ye
dahil edilmiştir.

.. warning::
	WSV'nin manuel olarak düzenlenmediğinden emin olmak düğüm
yöneticilerinin sorumluluğundadır – yalnızca Iroha veya `göç
betiği <#changing-iroha-version-migration>`__ tarafından
düzenlenmelidir. Manuel düzenleme veya güvenilir yönergelere
uymayan göç betiği düzenlemesi tutarsız bir ağa yol açabilir.
Bunu kendi sorumluluğunuzda yapın (uyardık).

Bazı durumlar için harika bir fikir olabilir, ancak blockstorage'dan
WSV'yi geri yüklemeye kıyasla WSV'yi yeniden kullanmanın belirli
özellikleri olduğunu lütfen göz önünde bulundurun:

| :red:`Güven noktası`
| **WSV'yi yeniden kullanma:** hem blockstorage'a hem de WSV'ye
  güvenmemiz gerekir.
| **block storage'dan WSV'yi geri yükleme:** yalnızca genesis
  block'a güveniriz.


| :red:`Bütünlük`
| **WSV'yi yeniden kullanma:** blockstorage ve WSV birbiriyle
  eşleşmelidir! Iroha bunu kontrol etmez.
| **block storage'dan WSV'yi geri yükleme:** Iroha, WSV'yi geri
  yüklerken her bloğu kontrol eder.
	blockstorage'daki herhangi bir hata bulunacaktır (tabii ki
	genesis block hariç).
	WSV'nin blockstorage ile eşleşmesi garantidir.

| :red:`Zaman`
| **WSV'yi yeniden kullanma:** Iroha neredeyse hemen ağda
  çalışmaya hazırdır.
| **block storage'dan WSV'yi geri yükleme:** blockstorage ne
  kadar büyükse, geri yüklemek ve çalışmaya başlamak o kadar
  uzun sürer.

.. note:: Kapanan yerel defter olması gerekenden daha fazla
bloğa sahipse ve doğru WSV aralarındaysa — sorun yok, Iroha doğru
bloğun WSV'sini alacaktır.
	Bloklar olması gerekenden azsa – WSV'yi yeniden kullanma
	seçeneği size işe yaramayacaktır.
	Lütfen bloklardan geri yükleyin.


WSV'yi düşürme
^^^^^^^^^^^^^^^

Varsayılan olarak Iroha başlangıçta WSV durumunu yeniden kullanır,
bu nedenle artık ``--reuse_state`` bayrağına gerek yoktur. Ancak
geriye dönük uyumluluk için bırakılmıştır. WSV durumunu düşürmek
isterseniz Iroha'yı ``--drop_state`` bayrağıyla başlatın. Bu bayrak
verildiğinde Iroha durum veritabanını sıfırlar ve üzerine yazar.

Durum veritabanı şema sürümü
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Mevcut WSV yeniden kullanılırken Iroha bir şema sürümü uyumluluk
kontrolü gerçekleştirir. Şeması kullanılan Iroha ile uyumlu değilse
veritabanını başlatmaz veya herhangi bir şekilde değiştirmez.

Şemanız v1.1.1 veya daha düşük bir sürümün Iroha'sı tarafından
oluşturulduysa, büyük olasılıkla sürüm bilgisi içermez. Bu durumda
bunu manuel olarak eklemeniz gerekir. Bu amaçla betiğimizi
kullanmanızı öneririz; `burada
<https://github.com/hyperledger-iroha-state-migration-tool/blob/master/state_migration.py>`__
bulunur. Şema sürümünüzü zorla (yani herhangi bir `göç işlemi
<#changing-iroha-version-migration>`__ olmadan) ayarlamak için,
betiği `--force_schema_version` bayrağıyla başlatın ve şemanızı
oluşturmak için kullanılan Iroha binary'sinin sürümünü geçirin.

.. warning::
  Şema sürüm numaralarını zorla yazmadan önce, şemayı oluşturan
  irohad sürümünü iki kez kontrol edin.
  Şema numaralarını zorladığınızda hiçbir kontrol yapılmaz; bu
  nedenle gelecekte (bir sonraki göç sırasında) durum
  veritabanını bozmak kolaydır.

Iroha sürümünü değiştirme. Göç.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
WSV'yi koruyarak Iroha sürümünü değiştirmek istiyorsanız göç
yapmanızı öneririz. Şema uyumsuzsa Iroha başlatmayı reddedeceği
için gereksiz olabilir, ancak genel kural olarak her sürümle
şemayı iyileştiriyoruz ve göç daha iyi performans için iyi bir
fikir olabilir. Göçten önce standart `PostgreSQL yönergelerini
<https://www.postgresql.org/docs/current/backup.html>`__
kullanarak veritabanı yedeği almanızı öneririz.

Göçü gerçekleştirmek için `betiğimizi
<https://github.com/hyperledger-iroha-state-migration-tool/blob/master/state_migration.py>`__
kullanın.

Veritabanından şema bilgisini yükleyecek ve göç adımlarıyla
eşleştirecektir (varsayılan olarak göç senaryoları betikle aynı
klasördeki ``migration_data`` dizininde tanımlanır). Ardından
veritabanınızı istenen sürüme taşıyacak tüm göç yollarını bulacak
ve birini seçmenizi isteyecektir.

.. seealso::
	`Burada
	<https://github.com/hyperledger-iroha-state-migration-tool/blob/master/README.md>`__
	farklı göç durumları hakkında ayrıntılar ve göçü gerçekleştirmek
için inceleyebileceğiniz örnekler var

WSV senkronizasyon modu.
^^^^^^^^^^^^^^^^^^^^^^^

WSV senkronizasyon modu için ``--wait_for_new_blocks``
seçeneklerini belirtin. Iroha, WSV'yi blockstore'dan geri yükler
ve dışarıdan yeni blokların eklenmesini bekler. Bu modda Iroha
ağ işlemleri gerçekleştirmez.
