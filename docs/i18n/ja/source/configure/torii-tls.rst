クライアント-ピア通信の TLS 設定（torii）
==========================================

デフォルトでは、クライアント-ピア通信は暗号化されていません。
有効にするには、次の手順が必要です:

1. 各ピアの鍵/証明書ペアを`生成 <#generating-keys>`__ する
2. 証明書をすべてのクライアントに配布する
3. irohad がこれらの鍵を使用するように`設定
   <#configuring-irohad>`__ する
4. irohad を[再]起動する


鍵の生成
~~~~~~~~

鍵は PEM 形式である必要があります。生成には ``openssl`` を使用でき
ます:

.. code-block:: sh

    $ openssl genpkey -algorithm rsa -out server.key
    $ openssl req -new -key server.key -x509 -out server.crt

``rsa`` の代わりに任意のアルゴリズムを使用できますが、``openssl``
がそれをサポートしている必要があります。
サポートされているものを確認するには、次を使用します:

.. code-block:: sh

    $ openssl list-public-key-algorithms

ノードへの接続にプレーンな IP アドレスを使用する必要がある場合、
サーバー証明書に ``subjectAltName`` を指定する必要があります。その
ためには、証明書を生成する前に openssl 設定の ``v3_ca`` セクション
に ``subjectAltName`` ディレクティブを追加します。
例えば、デフォルトインストールの場合 ``/etc/ssl/openssl.cnf``:

.. code-block:: text

    [ v3_ca ]
    subjectAltName=IP:12.34.56.78

証明書内のフィールドは Common Name（CN）を除いて実際には重要では
ありません。CN はクライアントのホスト名と照合され、一致しない場合
（例: example.com:50051 に接続する場合、example.com 上の irohad
は証明書の common name に example.com を持つ必要がある）TLS
ハンドシェイクは失敗します。

irohad の設定
~~~~~~~~~~~~~~

iroha があなたの鍵を使用するよう設定するには、``torii_tls_params``
設定パラメータを変更する必要があります。

次のブロックのようにする必要があります:

.. code-block:: javascript

    "torii_tls_params": {
        "port": 55552,
        "key_pair_path": "/path/to/server"
    }

``port`` - 任意のポートに設定できます（ただし通常は 55552 を使いま
す）

``key_pair_path`` - 鍵/証明書ペアへのフルパスを設定します。
``/path/to/server.key`` に鍵、``/path/to/server.crt`` に証明書があ
る場合、``torii_tls_keypair=/path/to/server`` と指定する必要がありま
す

.. note:: `examples ディレクトリ
  <https://github.com/baziorek/iroha1-cpp/tree/iroha1-main/example/torii_tls>`__
  にはサンプル証明書がありますが、TLS を有効にするにはサーバー用の
  新しい証明書が必要です（サンプルは動作しません）。
