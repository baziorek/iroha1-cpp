========
Shepherd
========

Shepherd は、実行中の irohad デーモンでメンテナンスタスクを実行す
るのに役立つコマンドラインユーティリティです。

前提条件
========

irohad デーモンにアクセスするには、``utility service`` が設定されて
いる必要があります。`設定の詳細
<../configure/index.html#deployment-specific-parameters>`__ を参照
してください。

次に、``shepherd`` を呼び出す際に、irohad utility サービスのアドレ
スとポートを含む ``--irohad`` コマンドライン引数を渡します:

.. code-block:: shell

   ./shepherd --irohad 127.0.0.1:11001 <...>

サポートされるアクション
========================

追加のコマンドライン引数を指定することで、``shepherd`` でできるこ
とは次のとおりです。

正常なシャットダウン
^^^^^^^^^^^^^^^^^^^^
これまで Iroha をどうやって停止していましたか?
えっ、本当に kill していたんですか?
おい、もう二度とそれをしないでください。失礼で親切ではありませ
ん!

.. code-block:: shell

   ./shepherd <...> --shutdown

``--shutdown`` 引数を指定すると、shepherd は Iroha に停止を丁寧に
依頼します。

動作の監視
^^^^^^^^^^^
他人の仕事を見ていることは、最も偉大な喜びの一つと広く考えられて
います。shepherd で Iroha が働く様子を見られます!

.. code-block:: shell

   ./shepherd <...> --status

これはワークサイクルステータスの更新を購読します。デーモンが起動
、動作、終了、または停止したばかりのときに明確なメッセージが届き
ます。

その他のパラメータ
==================

ログレベルも設定できます:

.. code-block:: shell

   ./shepherd <...> --verbosity debug <...>

サポートされる値は ``trace``、``debug``、``info``、``warning``、
``error``、``critical`` です。
