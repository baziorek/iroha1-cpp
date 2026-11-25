C++ Kitaplığı (iroha-lib)
-------------------------

``iroha-lib``, Iroha 1 için yerel bir C++ istemci kitaplığıdır.
İşlemleri ve sorguları oluşturmanızı, imzalamanızı, protobuf
biçimine serileştirmenizi ve bunları gRPC üzerinden bir Iroha
düğümüne göndermenizi sağlar — düğümün kendisini derlemeden veya
dağıtmadan.

Kitaplık iki şekilde derlenebilir:

* tam bir Iroha derlemesinin parçası olarak (``iroha_lib``
  hedefi), veya
* **bağımsız olarak** — yalnızca istemci kitaplığı ve
  dependencies derlenir, düğüm kodu gerekmez (block storage yok,
  Torii hizmeti yok, senkronizasyon yok).

Bağımsız derleme
^^^^^^^^^^^^^^^^

Bağımsız derleme ``iroha-lib/`` dizininde bulunur ve bağımsız bir
CMake projesidir. Depodan ``shared_model``, ``libs`` ve model
dönüştürücülerini yeniden kullanır, bu yüzden yine de tam kaynak
ağacı gerekir — ancak yalnızca kitaplık hedefleri derlenir.

Kitaplığın küçük bir bağımlılık kümesi vardır — ``protobuf``,
``grpc``, ``boost``, ``spdlog``, ``rapidjson``, ``gflags`` ve
``iroha-ed25519`` (bkz. ``vcpkg/VCPKG_DEPS_LIST``). vcpkg
kullanılırken bunlar düğüm için kullanılan aynı betikle bir kez
yüklenir — tam talimatlar için bkz. :ref:`build-guide`:

.. code:: sh

    # from the repository root; builds vcpkg into ./vcpkg-build
    ./vcpkg/build_iroha_deps.sh $PWD/vcpkg-build

Ardından yapılandırın ve derleyin:

.. code:: sh

    cmake -S iroha-lib -B build-iroha-lib \
        -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg-build/scripts/buildsystems/vcpkg.cmake \
        -DVCPKG_TARGET_TRIPLET=x64-linux \
        -DCMAKE_BUILD_TYPE=Release

    cmake --build build-iroha-lib --target iroha_lib -j$(nproc)

İliştirilmiş örnekleri de derlemek için (``TxExample``,
``BatchExample``, ``QueryExample``, ``TxOrderingExample``,
``BlocksQueryExample``, ``MstExample``,
``InfiniteBlocksStreamExample``, ``DomainAssetCreation``,
``TlsExample``):

.. code:: sh

    cmake --build build-iroha-lib -j$(nproc)

Düğüm projesi içinde derleme
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tüm proje yapılandırıldığında kitaplık ``iroha_lib`` hedefi
olarak kullanılabilir (``grpc_client`` ile birlikte):

.. code:: sh

    cmake --build build --target iroha_lib -j$(nproc)

Kitaplığı kullanma
^^^^^^^^^^^^^^^^^^

Ana giriş noktaları:

* ``iroha_lib::Tx`` / ``iroha_lib::TxBatch`` — işlemleri
  oluşturma, imzalama ve gönderme (bkz.
  ``model/Tx.hpp``, ``model/TxBatch.hpp``).
* ``iroha_lib::Query`` — sorguları oluşturma ve gönderme
  (bkz. ``model/Query.hpp``).
* ``iroha_lib::BlocksQuery`` — ``FetchCommits`` akışı için
  imzalı blok sorguları oluşturma (bkz.
  ``model/BlocksQuery.hpp``).
* ``iroha_lib::GrpcClient`` — Torii ``CommandService`` ve
  ``QueryService`` uç noktaları için düşük seviyeli gRPC
  istemcisi; ``sendStatusStream`` / ``sendBlocksStreamQuery``
  server-streaming API'leri ve ``iroha_lib::TlsOptions``
  aracılığıyla isteğe bağlı TLS taşıması dahil (bkz.
  ``grpc_client/GrpcClient.hpp``).
* ``iroha_lib::createKeyFiles`` / ``iroha_lib::loadKeyFiles``
  (``model/utils/key_files.hpp``) — ``KeysManagerImpl``
  biçiminde anahtar dosyaları oluşturma ve yükleme
  (``<name>.pub`` / ``<name>.priv``, isteğe bağlı parola
  şifrelemesi).
* ``iroha_lib::generateGenesisBlock`` / ``readPeersAddresses``
  (``model/utils/genesis.hpp``) — örnek genesis bloğu
  oluşturma, ``iroha-cli --genesis_block`` eşdeğeri.

Anahtar çiftleri hex biçimindeki özel anahtardan
``Utils::generateKeypairFromPrivateKey``
(``model/utils/Utils.h``) ile veya rastgele
``Utils::generateRandomKeypair`` ile oluşturulabilir; imzalar
``shared_model`` içindeki ed25519/SHA3 uygulamasını kullanır.

Tüm örnekler bağlantı ayarlarını ortam değişkenlerinden okur;
``iroha-python`` geliştirme yapılandırmasıyla eşleşen varsayılan
değerlerle (``IROHA_HOST_ADDR``, ``IROHA_PORT``,
``ADMIN_ACCOUNT_ID``, ``ADMIN_PRIVATE_KEY`` — bkz.
``examples/common.hpp``). ``Scenario`` veya ``makeClient``
üzerinden geçen örnekler ayrıca ``IROHA_TLS_ROOT_CERT``
değerini dikkate alır — ayarlandığında istemci TLS üzerinden
bağlanır.

Hızlı başlangıç — alan ve asset oluşturan bir işlemi oluşturma,
imzalama ve gönderme (``examples/scenario.hpp`` içinden):

.. literalinclude:: ../../../../iroha-lib/examples/scenario.hpp
   :language: c++
   :lines: 45-56

Sorgu gönderme ve yanıtı okuma (``examples/scenario.hpp``
içinden):

.. literalinclude:: ../../../../iroha-lib/examples/scenario.hpp
   :language: c++
   :lines: 179-201

Tam çalışan örnek ``examples/TxExample.cpp``'dir — bir alan ve
asset oluşturur, bir hesap kaydeder, coin transfer eder ve canlı
bir düğümde sorgular çalıştırır:

.. code:: sh

    IROHA_HOST_ADDR=127.0.0.1 IROHA_PORT=50051 \
        ./build-iroha-lib/examples/irohalib_tx_example

TLS (şifreli taşıma)
^^^^^^^^^^^^^^^^^^^^

Varsayılan olarak ``GrpcClient`` korumasız bir gRPC kanalı
kullanır. Bağlantıyı şifrelemek için yapıcıya
``iroha_lib::TlsOptions`` iletin:

.. code:: c++

    #include <fstream>
    #include <sstream>

    std::ifstream cert_file("/path/to/node.crt");
    std::ostringstream pem;
    pem << cert_file.rdbuf();

    iroha_lib::TlsOptions tls;
    tls.pem_root_certs = pem.str();   // PEM root certificate(s)
    // tls.pem_cert_chain / tls.pem_private_key - optional, for mutual TLS

    iroha_lib::GrpcClient client("127.0.0.1", 50051, tls);

Iroha Torii TLS uç noktası **tek yönlü TLS** kullanır (yalnızca
sunucu kimlik doğrulaması), bu nedenle pratikte yalnızca
``pem_root_certs`` gereklidir. Aynı kimlik bilgileri istemcinin
tüm işlemlerine uygulanır — işlemler, sorgular ve her iki
streaming API'si.

Düğüm tarafında TLS'yi etkinleştirmek için düğüm
yapılandırmasında ``torii_tls_params`` ayarlayın (``port`` ve
``key_path`` — sunucu ``<key_path>.crt`` ve ``<key_path>.key``
dosyalarını okur).

İliştirilmiş ``TlsExample``, bir TLS bağlantısını
gösterir: şifreli kanal üzerinden imzalı bir ``GetAccount``
sorgusu ve ``GetTxStatus`` çağrısı yapar. Düğümü doğrulamak için
kullanılan sertifikayı gösteren PEM dosyasını işaret eden
``IROHA_TLS_ROOT_CERT`` gerektirir:

.. code:: sh

    IROHA_HOST_ADDR=127.0.0.1 IROHA_PORT=<torii_tls_port> \
    IROHA_TLS_ROOT_CERT=/path/to/node.crt \
        ./build-iroha-lib/examples/irohalib_tls_example

Bu yolu kapsayan bir entegrasyon testi vardır:
``test/integration/iroha_lib_tls``, TLS uç noktası etkin bir
düğüm başlatır ve örneği üzerinde çalıştırır.
