.. _iroha-cli:

======================
``iroha-cli``
======================

``iroha-cli`` は Iroha のコマンドラインクライアントです。コードを
書かずにピアへトランザクションとクエリを送信できる対話型ツール
です。``irohad`` と一緒にすべてのディストリビューションに
同梱されています — すでに持っているものを選んでください。

``iroha-cli`` の入手方法
------------------------

Docker
^^^^^^

公式イメージからノードを実行している場合、``iroha-cli`` は既に
コンテナ内にあります:

.. code-block:: shell

  docker exec -it iroha iroha-cli -account_name admin@test

``-account_name`` 引数は、トランザクションに署名する鍵ペアを選択
します — この例では設定ファイルの ``admin@test`` （鍵は
``example/`` の他のファイルと一緒にあります）。

AppImage
^^^^^^^^

`GitHub Releases
<https://github.com/baziorek/iroha1-cpp/releases>`__ ページから
``iroha-cli-x86_64.AppImage`` を ``irohad`` と一緒にダウンロード
し、実行可能にして起動します:

.. code-block:: shell

  chmod +x iroha-cli-x86_64.AppImage
  ./iroha-cli-x86_64.AppImage -account_name admin@test

.. note:: AppImage の ``iroha-cli`` はリリース ``1.6.1`` から
  公開されます — :ref:`node-appimage` を参照。

ソースからのビルド
^^^^^^^^^^^^^^^^^^

:ref:` ソースから Iroha をビルド <node-build>` すると、
``iroha-cli`` は ``irohad`` の隣に生成されます:

.. code-block:: shell

  ./build/bin/iroha-cli -account_name admin@test

初回実行
--------

ディストリビューションに関わらず、初回実行ではピアのアドレスと
ポートを尋ねられます — ローカルノードでは ``localhost`` と
``50051`` （ノード設定の ``torii_port`` ）です。その後、対話メニュー
でトランザクション（``tx`` ）とクエリ（``qry`` ）の作成、ピアへの
送信、トランザクションステータスの確認ができます。

.. note:: ``iroha-cli`` は最も一般的なコマンドのみをカバーする
  補助ツールであり、完全な API ではありません。最初のトランザ
  クション送信の詳細なウォークスルーは :doc:`cli-guide` に、
  プログラムによるアクセスは :doc:`python-guide` と
  `API リファレンス
  <../develop/api/commands.html>`_ にあります。
