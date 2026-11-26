Iroha kurulum güvenliği ipuçları
================================

Bu kılavuz Iroha kurulumunu güvence altına almeyi amaçlar. Bu
kılavuzdaki adımların çoğu apaçık görünebilir ancak gelecekteki
olası güvenlik sorunlarından kaçınmaya yardımcı olur.

Fiziksel güvenlik
^^^^^^^^^^^^^^^^^

Sunucular yerel olarak (fiziksel olarak erişilebilir)
konumlandırılmışsa, bir dizi güvenlik önlemi uygulanmalıdır.
Bulut barındırma kullanılıyorsa bu adımları atlayın.

Yalnızca yetkili personelin sunucu odasına erişebilmesi için
örgütsel bir politika ve/veya erişim kontrolü sistemi kurun.
Ardından BIOS/firmware parolasını ayarlayın ve yetkisiz
başlatmayı önlemek için önyükleme sırasını yapılandırın.
Böyle bir işlev varsa önyükleyicinin parola korumalı olduğundan
emin olun. Ayrıca CCTV izlemesinin olması da iyidir.

Dağıtım
^^^^^^^

Öncelikle, `kaynak kodu
<https://github.com/baziorek/iroha1-cpp>`__ (topluluk fork'u
`hyperledger-iroha/iroha
<https://github.com/hyperledger-iroha/iroha/tree/iroha1-main>`__)
ve `Docker imajlarını
<https://hub.docker.com/r/hyperledger/iroha>`__ indirmek için
resmi deponun kullanıldığını doğrulayın.
Kurulum sırasında kullanılan varsayılan parolaları — örneğin
postgres'e bağlanmak için kullanılan parolayı — değiştirin.
Iroha deposu özel ve genel anahtarların örneklerini içerir — bunları
asla üretimde kullanmayın.
Ayrıca, yeni anahtar çiftlerinin güvenli bir ortamda üretildiğini
ve yalnızca yöneticinin bu anahtar çiftlerine erişebildiğini
doğrulayın (veya en azından erişebilen kişi sayısını en aza
indirin).
Anahtarları Iroha eşlerine dağıttıktan sonra, dağıtımı gerçekleştirmek
için kullanılan host'tan özel anahtarları silin; yani özel
anahtarlar yalnızca Iroha eşlerinin içinde bulunmalıdır.
Özel anahtarları silmeden önce şifrelenmiş bir yedek oluşturun ve
erişimi sınırlayın.

Ağ yapılandırması
^^^^^^^^^^^^^^^^^

Iroha, 50051 ve 10001 portlarını dinler ve TLS etkinse isteğe
bağlı olarak 55552'yi de dinler.
Güvenlik duvarı ayarları bu portlara/üzerinden gelen/giden
bağlantılara izin vermelidir.
Mümkünse, dinleme portlarına sahip diğer ağ hizmetlerini
(FTP, DNS, LDAP, SMB, DHCP, NFS, SNMP vb.) devre dışı bırakın veya
kaldırın.
İdeal olarak Iroha, ağ açısından mümkün olduğunca izole
olmalıdır.

İstemci-eş iletişimini şifrelemek isterseniz torii portlarında
TLS'i etkinleştirebilirsiniz.

Trafik şifrelemesi kullanmıyorsanız, eşler arasındaki iletişimi
şifrelemeye olanak tanıyan herhangi bir mekanizma, yani Docker
overlay ağını kurmak için VPN veya Calico kullanmanızı şiddetle
öneririz.
Docker swarm varsayılan olarak iletişimleri şifreler, ancak güvenlik
duvarı yapılandırmasında gerekli portları açmayı unutmayın.
VPN kullanılıyorsa, VPN anahtarının diğer kullanıcılara
kullanılamaz olduğunu doğrulayın.

SSH kullanılıyorsa, root oturum açmayı devre dışı bırakın.
Bunun dışında, parola kimlik doğrulamasını devre dışı bırakın ve
yalnızca anahtarları kullanın.
SSH günlük düzeyini INFO olarak ayarlamak da yardımcı olabilir.

IPv6 kullanılmıyorsa, devre dışı bırakmak iyi bir fikir olabilir.

Güncellemeler
^^^^^^^^^^^^^

En son işletim sistemi güvenlik yamalarını yükleyin ve düzenli
olarak güncelleyin.
Iroha Docker konteynerlerinde çalışıyorsa Docker'ı düzenli olarak
güncelleyin.
İsteğe bağlı olsa da, güncellemeleri üretime kurmadan önce ayrı
bir sunucuda test etmek iyi bir pratiktir.

Günlük kaydı ve izleme
^^^^^^^^^^^^^^^^^^^^^^

- Bir aracı (ör. Filebeat) kullanarak günlükleri toplayın ve özel
  bir makineye gönderin.
- Tüm Iroha eşlerinden günlükleri merkezi bir noktada toplayın
  (ör. Logstash).
- Günlük kaydı ve izleme bilgilerini şifreli bir kanal üzerinden
  (ör. https) aktarın.
- Üçüncü tarafların günlüklere erişmesini önlemek için bir
  kimlik doğrulama mekanizması kurun.
- Üçüncü tarafların günlük göndermesini önlemek için bir kimlik
  doğrulama mekanizması kurun.
- Tüm yönetici erişimini kaydedin.

İS sertleştirme
^^^^^^^^^^^^^^^

Aşağıdaki adımlar Iroha'nın çalıştırılması için Docker
kullanıldığını varsayar.

- Docker Content Trust'u etkinleştirin ve yapılandırın.
- Yalnızca güvenilir kullanıcıların Docker daemon'unu
  kontrol etmesine izin verin.
- Docker konteyner kaynakları için bir limit belirleyin.
