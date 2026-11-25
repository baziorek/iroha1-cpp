.. _node-deb:

=====================================
リリースパッケージからの Iroha ノード
=====================================

Docker イメージ、AppImage、ソースビルドに加えて、Iroha 1 のリリース
パイプラインはコミュニティフォークの `GitHub Releases
<https://github.com/baziorek/iroha1-cpp/releases>`__ にネイティブ
パッケージを添付します:

- ``iroha_*.deb`` — CPack でビルドされた Debian/Ubuntu パッケージ。
  ``irohad`` とメンテナンスツール（migration tool、WSV checker）
  を含む
- ``iroha-*.rpm`` — RPM ディストリビューション向けの同等品（公開
  されている場合）
- ``irohad-x86_64.AppImage`` / ``iroha-cli-x86_64.AppImage`` —
  :doc:`node-appimage` 参照

.. note:: パッケージはタグ ``1.6.1`` 以降の Releases ページに
  現れます。それまでは :ref:`Docker <node-docker>` の代わりに
  :ref:`Docker <node-docker>` または
  :ref:` ソースビルド <node-build>` を使用してください。

.deb パッケージのインストール
-----------------------------

Releases ページから ``.deb`` ファイルをダウンロードし、依存関係も
解決してくれる ``apt`` でインストールします:

.. code-block:: shell

  sudo apt install ./iroha_*.deb

または ``dpkg`` を直接使用:

.. code-block:: shell

  sudo dpkg -i iroha_*.deb
  # apt が依存関係の不足を報告した場合:
  sudo apt -f install

パッケージはバイナリをシステムにインストールします（CPack の
デフォルトプレフィックスは ``/usr`` ）。確認:

.. code-block:: shell

  which irohad
  irohad --help

.rpm パッケージのインストール
-----------------------------

RPM ディストリビューション（Fedora、RHEL、openSUSE）では:

.. code-block:: shell

  sudo dnf install ./iroha-*.rpm
  # または: sudo rpm -i iroha-*.rpm && sudo dnf -f install

ノードの起動
------------

パッケージ化された ``irohad`` は他のビルドと同じ引数を受け付けます
— 設定ファイル、genesis block、鍵ペア名:

.. code-block:: shell

  sudo irohad --config /etc/iroha/config.json \
              --genesis_block /etc/iroha/genesis.block \
              --keypair_name /etc/iroha/node0

パッケージには設定が含まれていません — リポジトリの ``example/``
ディレクトリから例をコピーして環境に合わせて調整してください
（:doc:`configuration` 参照）。起動前にアクセス可能である必要が
ある ``database`` セクション（PostgreSQL または RocksDB）を含みます。

.. note:: パッケージ化されたノードにも PostgreSQL（または RocksDB）
  が必要で、プロダクションデプロイでは Torii の前に TLS 終端が
  必要です — :doc:`../configure/torii-tls` を参照。
