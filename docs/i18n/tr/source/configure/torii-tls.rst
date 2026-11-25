İstemci-eş iletişimi için TLS yapılandırması (torii)
====================================================

Varsayılan olarak istemci-eş iletişimi şifrelenmez.
Etkinleştirmek için şunları yapmanız gerekir:

1. Her eş için bir anahtar/sertifika çifti `oluşturun
   <#generating-keys>`_
2. Sertifikayı tüm istemcilere dağıtın
3. irohad'ın bu anahtarları kullanacak şekilde `yapılandırın
   <#configuring-irohad>`_
4. irohad'ı [yeniden] başlatın


Anahtar üretme
~~~~~~~~~~~~~~

Anahtarlar PEM formatında olmalıdır. Üretmek için ``openssl``
kullanabilirsiniz:

.. code-block:: sh

    $ openssl genpkey -algorithm rsa -out server.key
    $ openssl req -new -key server.key -x509 -out server.crt

``openssl``'unuz desteklediği sürece ``rsa`` yerine herhangi bir
algoritma kullanabilirsiniz.
Hangilerinin desteklendiğini öğrenmek için şunu kullanabilirsiniz:

.. code-block:: sh

    $ openssl list-public-key-algorithms

Düğüme bağlanmak için düz IP adresleri kullanmanız gerekiyorsa,
sunucu sertifikanızda ``subjectAltName`` belirtmeniz gerekir; bunun
için sertifikayı oluşturmadan önce openssl yapılandırmanızın
``v3_ca`` bölümüne bir ``subjectAltName`` direktifi eklemeniz
gerekir.
Örneğin varsayılan kurulum için ``/etc/ssl/openssl.cnf``:

.. code-block:: text

    [ v3_ca ]
    subjectAltName=IP:12.34.56.78

Sertifikadaki alanlar Common Name (CN) dışında pek önemli
değildir — CN istemcinin ana makine adıyla kontrol edilir ve
eşleşmezlerse TLS el sıkışması başarısız olur (ör. example.com:50051
adresine bağlanıyorsanız, example.com'daki irohad'ın sertifikanın
common name'inde example.com olması gerekir).

irohad'ı yapılandırma
~~~~~~~~~~~~~~~~~~~~~

iroha'nın anahtarlarınızı kullanması için ``torii_tls_params``
yapılandırma parametresini değiştirmeniz gerekir.

Şu bloğa benzemelidir:

.. code-block:: javascript

    "torii_tls_params": {
        "port": 55552,
        "key_pair_path": "/path/to/server"
    }

``port`` - istediğiniz herhangi bir porta ayarlayın (ancak
genellikle 55552 istersiniz)

``key_pair_path`` - anahtar/sertifika çiftinin tam yolunu ayarlayın;
``/path/to/server.key`` konumunda bir anahtarınız ve
``/path/to/server.crt`` konumunda bir sertifikanız varsa
``torii_tls_keypair=/path/to/server`` belirtmeniz gerekir

.. note:: `Örnekler dizininde
  <https://github.com/baziorek/iroha1-cpp/tree/iroha1-main/example/torii_tls>`__
  örnek sertifikalar vardır, ancak TLS'i etkinleştirmek için
  sunucunuz için yeni bir sertifika gereklidir (örnek çalışmaz).
