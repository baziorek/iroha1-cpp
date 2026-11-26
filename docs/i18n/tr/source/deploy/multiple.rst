=========================================
Çoklu örnek çalıştırma (eş ağı)
=========================================

Bir eş ağı kurmak için bu bölümde açıklanan prosedürleri izlemek
gerekir. Bu sürümde manuel dağıtım ve Ansible Playbook ile
otomatik dağıtım desteklenir. Güvenlik kriterlerinizi ve diğer
ihtiyaçlarınızı karşılayan seçeneği seçin.

Manuel
------

Manuel dağıtım ile otomasyon olmadan Iroha eş ağını kurmayı
kastediyoruz. Tek yerel örnek çalıştırma işlemine benzer, tek
farkı genesis block'un birden fazla eş içermesidir. Birden fazla
eş içeren bir blok oluşturmak veya ihtiyaçlarınıza göre özelleştirmek
gerekirse :ref:`deploy_troubles` bölümüne bakın.

Otomatik
--------

`iroha-swarm <https://github.com/kuvaldini/iroha-swarm>`_ tek
bir komutla çok sayıda Iroha düğümü çalıştırmak için
docker-compose yapılandırma dosyaları üretir. Veritabanı
türünü — RocksDB veya Postgres — seçebilir, eş anahtarlarını
sağlayabilir veya önceden üretilmiş olanları kullanabilirsiniz.
Temel/ilk yapılandırma dosyasını oluşturmak ve sık değişen
ortamlar (farklı düğüm kümeleri) için çok kullanışlıdır.

.. note:: ``iroha-swarm`` bir topluluk aracıdır ve ~2019'dan
  beri bakımsızdır — kullanmadan önce dikkatle inceleyin; artık
  var olmayan görüntü etiketlerine (ör. ``:latest``)
  atıfta bulunabilir.

Ayrıca bir `kılavuz
<https://github.com/hyperledger/iroha-deploy/blob/master/ansible/roles/iroha-docker/README.md>`__
da var (arşivlenmiş depo); güncel olmayabilir ama faydalı
bilgiler sağlayabilir.
