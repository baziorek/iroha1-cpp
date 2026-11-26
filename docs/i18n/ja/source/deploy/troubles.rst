.. _deploy_troubles:

======================
トラブルシューティング
======================

—「助けてください、なぜなら…」

Iroha デーモンのバイナリがない
-------------------------------

ソースから Iroha デーモンのバイナリをビルドできます。バイナリ
は `こちら
<https://github.com/baziorek/iroha1-cpp/releases>`__ から入手
できます

設定ファイルがない
------------------

この `リンク
<../configure/index.html>`__ に従って設定ファイルの作成方法を
確認してください

genesis block がない
--------------------

`iroha-cli` で生成するか、`例
<https://github.com/baziorek/iroha1-cpp/blob/iroha1-main/example/genesis.block>`__
を使用して手動で genesis block を作成し、`パーミッション
<../develop/api/permissions.html>`__ を確認してください

ピアの鍵ペアがない
------------------

アカウントまたはピアの鍵ペアを作成するには、ピアの名前を
`--new_account` オプションで渡して iroha-cli バイナリを使用し
ます。例:

.. code-block:: shell

    ./iroha-cli --account_name newuser@test --new_account
