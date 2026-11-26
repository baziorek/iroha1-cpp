.. _configuration:

=========
設定
=========

.. toctree::
      :maxdepth: 1

      torii-tls.rst
      db.rst

このセクションでは、Iroha の設定方法を理解します。
一部の設定パラメーターはすべてのノードで同じでなければならず（アスタリスク \* で示します）、一部は異なっていても構いません。
``example/config.sample`` を見てみましょう。

.. note:: v1.2 以降、``irohad`` は設定ファイルだけでなく環境変数でも設定できるようになりました。

まず設定ファイルを見て、その後 `環境パラメーター <環境変数_>`_ による Iroha の設定方法を見ていきます。

.. literalinclude:: ../../../example/config.sample
   :language: json


ご覧のように、設定ファイルは有効な ``json`` 構造です。
行ごとに見ていき、設定ファイル形式における各パラメーターの意味を理解しましょう。

デプロイ固有のパラメーター
==============================

- ``block_store_path`` （任意） ブロックが格納されるフォルダーへのパスを設定します。このパラメーターが指定されない場合、ブロックはデータベースに格納されます。
- ``torii_port`` は外部通信用のポートを設定します。クエリとトランザクションはここに送信されます。
- ``internal_port`` は内部通信用のポートを設定します。オーダリングサービス、コンセンサス、ブロックローダーが使用します。
- ``database`` （任意） はデータベースの設定に使用されます（後述）。
- ``pg_opt`` （任意） は PostgreSQL の認証情報を設定する **非推奨の** 方法です。ホスト名、ポート、ユーザー名、パスワード、データベース名を指定します。データベース名以外のすべてのデータが必須です。データベース名が指定されない場合はデフォルトの ``iroha_default`` が使用されます。
- ``log`` はログ出力の詳細度と形式を制御する任意のパラメーターです（後述）。
- ``utility_service`` （任意） はメンテナンスタスク用のエンドポイントです。存在する場合、バインドする ``ip`` アドレスと ``port`` を含める必要があります。メンテナンスエンドポイントの使用例については `shepherd ドキュメント <../maintenance/shepherd.html>`_ を参照してください。
- ``metrics`` （任意） は Iroha のメトリクスを監視するエンドポイントです。Prometheus HTTP サーバーはこのエンドポイントでリッスンします。存在する場合、"[addr]:<port>" 形式に対応している必要があり、例えば "127.0.0.1:8080"、"9090"、":1234" などが指定できます。誤った値を指定すると Prometheus メトリクスサーバーが暗黙的に無効になります。また、このパラメーターを上書きするコマンドラインオプション ```--metrics_port`` と ``--metrics_addr`` もあります。
- ``healthcheck_port`` （任意） は Iroha ヘルスチェック用のエンドポイントです。このエンドポイントに ``http://<host>:<healthcheck_port>/healthcheck`` 形式でリクエストを送信すると、ノードの状態に関する情報が返されます。現在のメモリ消費量（``memory_consumption``）、現在のブロック数（``last_block_round``）、現在の拒否ラウンド数（``last_reject_round``）、ノードがリモートノードと同期中かどうか（``is_syncing``）、ノードが現在稼働しているかどうか（``status``）などです。

また、設定に含めることでクライアント通信の TLS サポートを有効にできる任意の ``torii_tls_params`` パラメーターもあります。

ここで、``port`` は TLS サーバーがバインドする TCP ポートであり、``key_pair_path`` はキーペアへのパスです。このパスに ``.crt`` を付加すると PEM エンコードされた証明書へのパスになり、``.key`` を付加するとこの証明書の PEM エンコードされた秘密鍵へのパスになります（例えば ``key_pair_path`` が ``"/path/to/the/keypair"`` の場合、iroha は ``"/path/to/the/keypair.crt"`` にある証明書と ``"/path/to/the/keypair.key"`` にある鍵を探します）。

.. warning::
   設定フィールド ``pg_opt`` は非推奨です。``database`` セクションを使用してください。

   設定で両方が指定された場合、``database`` セクションは ``pg_opt`` を上書きします。

   ``pg_opt`` と ``database`` フィールドはどちらも任意ですが、少なくとも一方を指定する必要があります。

``database`` セクションのフィールド:

- ``host`` — PostgreSQL 接続に使用するホスト
- ``port`` — PostgreSQL 接続に使用するポート
- ``user`` — PostgreSQL 接続に使用するユーザー
- ``password`` — PostgreSQL 接続に使用するパスワード
- ``working database`` — ワールドステートビューと任意でブロックを格納するために使用されるデータベース名
- ``maintenance database`` — ワールドデータベースをメンテナンスするために使用されるデータベース名。例えば、iroha がワークデータベースを作成または削除する必要がある場合、PostgreSQL に接続するために別のデータベースを使用しなければなりません。

環境固有のパラメーター
===============================

- ``max_proposal_size`` \* は 1 つの提案に含められるトランザクションの最大数であり、結果として単一のブロックにも同じ数が含まれます。この値を変更することで潜在的なブロックのサイズを定義できます。最初は ``10`` のままで構いません。ただし、1 秒あたりのトランザクション数が多い場合は、この数を増やすことをお勧めします。

    **このパラメーターはパフォーマンスに影響します。** ネットワークで大量のトランザクションが流れている場合はこのパラメーターを増やしてください。スループット向上のために ``max_proposal_size`` を増やす場合は、独立して増やすことができます。このパラメーターを増やすことでパフォーマンスを向上させられますが、ある時点を超えると値を増やすことがパフォーマンスの低下につながる可能性があることに注意してください。

- ``vote_delay`` \* は次のピアに投票を送信する前の待機時間（ミリ秒）です。最適な値はネットワーク内の Iroha ピアの数に大きく依存します（ノード数が多いほど長い ``vote_delay`` が必要です）。** 少なくとも 1 秒に設定することを強くお勧めします。そうしないと、一部のピアに容易に到達できない場合にブロックチェーンの成長が非常に遅くなるか、成長が止まってしまうことがあります。**

    **このパラメーターはコンセンサスメカニズムにのみ影響します。** ネットワークが高速なら、このパラメーターはネットワークにほとんど影響しません。しかし、ネットワークが遅い場合は、ピアが応答する時間を確保するために増やしてください。

- ``mst_enable`` は Iroha のマルチシグネチャトランザクションのネットワーク転送を有効または無効にします。フラグが ``false`` に設定されていても、MST エンジンは常にすべてのピアで動作することに注意してください。このフラグはピア間で MST トランザクションに関する情報を共有できるようにするだけです。

- ``mst_expiration_time`` は、完全に署名されていないトランザクション（またはバッチ）が期限切れと見なされるまでの時間（分）を指定する任意のパラメーターです。デフォルト値は 1440 です。

- ``proposal_creation_timeout`` （以前は ``max_rounds_delay`` と呼ばれていました）\* は、オーダリングサービスが提案リクエストに応答する前に提案（つまり ``max_proposal_size`` までトランザクションを収集すること）を組み立てる間待機する最大時間（ミリ秒）を指定する任意のパラメーターです。デフォルト値は 3000 です。2 番目の派生タイムアウト — ピア間の ``RequestProposal`` RPC のネットワークデッドライン — は自動的に ``2 × proposal_creation_timeout`` として計算され、個別に設定することはできません。

    **このパラメーターはリソース消費に影響します。** Iroha が長期間アイドル状態になることが予想され、リソースを節約したい場合はこの値を増やしてください。新しいトランザクションをより稀にチェックするようになります。注意: アイドル期間後の最初のトランザクションはその影響で少し遅延する可能性があります。2 番目以降のブロックはより速く処理されます。

- ``stale_stream_max_rounds`` は、ステータス更新が報告されない間、開いたステータスストリームを保持する最大ラウンド数を指定する任意のパラメーターです。デフォルト値は 2 です。この値を増やすと、何らかの理由で新しいラウンドで更新されない場合にクライアントがトランザクションを追跡するために再接続しなければならない回数が減ります。ただし、大きな値は各ラウンド中の接続クライアントの平均数を増加させます。

    ノードがストリームで過負荷にならないよう、このパラメーターを制限することが推奨されます。

- ``initial_peers`` は、ノードが起動後にジェネシスブロックのピアの代わりに使用するピアのリストを指定する任意のパラメーターです。初期ピアのほとんどが悪意を持つ可能性があるネットワークに新しいノードを追加する場合に便利です。ピアリストは JSON 配列として指定する必要があります:

.. code-block:: javascript

  "initial_peers": [
    {
      "address": "127.0.0.1:10001",
      "public_key": "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
    }
  ]

- ``max_past_created_hours``: トランザクションの ``created_time`` を現在時刻（ピアで計測）から過去何時間まで設定できるかを指定する任意のパラメーター。デフォルト値は ``"24"`` 時間です。この値はすべてのピアで同じでなければなりません。そうでないと、ネットワークがブロックの生成を静かに停止させる原因になる可能性があります。

ベストプラクティスの例
----------------------

均等な分散でかなり良い結果が得られました — 5 分間に 300k トランザクションを送信し、コミットには 2 秒から 2 分かかりました。
**結果は常にネットワーク内のピア数、その速度、ピアが稼働するホストのパラメーターに依存することに注意してください。**

使用した設定は次のとおりです:

.. code-block:: javascript

  "max_proposal_size" : 10000,
  "vote_delay" : 1000,
  "mst_enable" : true,
  "mst_expiration_time": 1440,
  "proposal_creation_timeout": 500,
  "stale_stream_max_rounds": 100000


環境変数
=========

Iroha を設定する別の方法は環境変数を使用することです。
設定ファイルと環境変数は組み合わせることができます。
**設定ファイルで指定されたパラメーターが存在する場合、環境変数で設定されたものを上書きします。**

パラメーターがどのように見えるかの例をいくつか示します。

Unix
----

.. code-block:: bash

  export IROHA_BLOCK_STORE_PATH=/tmp/block_store/
  export IROHA_TORII_PORT=50051
  export IROHA_INTERNAL_PORT=10001
  export IROHA_PG_OPT="host=172.19.0.2 port=5432 user=iroha password=helloworld"
  export IROHA_MAX_PROPOSAL_SIZE=10
  export IROHA_VOTE_DELAY=5000
  export IROHA_MST_ENABLE=false
  export IROHA_MST_EXPIRATION_TIME=1440
  export IROHA_PROPOSAL_CREATION_TIMEOUT=3000
  export IROHA_CRYPTO_PROVIDERS_0_KEY=p1
  export IROHA_CRYPTO_PROVIDERS_0_CRYPTO_TYPE=ed25519_sha3_256
  export IROHA_CRYPTO_PROVIDERS_0_PRIVATE_KEY=cc5013e43918bd0e5c4d800416c88bed77892ff077929162bb03ead40a745e88
  export IROHA_CRYPTO_PROVIDERS_0_TYPE=default
  export IROHA_CRYPTO_SIGNER=p1

Windows
-------

.. code-block:: bat

  setx IROHA_BLOCK_STORE_PATH C:\block_store
  setx IROHA_TORII_PORT 50051
  setx IROHA_INTERNAL_PORT 10001

PowerShell
----------

.. code-block:: bash

  $Env:IROHA_BLOCK_STORE_PATH="C:\block_store"
  $Env:IROHA_TORII_PORT="50051"
  $Env:IROHA_INTERNAL_PORT="10001"

パラメーター名
---------------

ご覧のように、パラメーター名は設定ファイルのものとは異なります。
設定構造から形成され、先頭に固定ラベル IROHA が追加され、すべて大文字に変換されて _ で連結されます。
構造をもう少し詳しく見てみましょう。

**単純な文字列値の場合**

設定ファイルでは:

.. code-block:: javascript

  "block_store_path": "/tmp/block_store/"

環境変数では:

.. code-block:: bash

  IROHA_BLOCK_STORE_PATH=/tmp/block_store/


**配列の場合**

配列は 0 から始まるインデックスを持ち、番号を飛ばさずに直接の順序でなければなりません:

設定ファイルでは:

.. code-block:: javascript

  "initial_peers": [
    {
      "address": "127.0.0.1:10001",
      "public_key": "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
    },
    {
      "address": "127.0.0.1:10002",
      "public_key": "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308920"
    }
  ]

環境変数では:

.. code-block:: bash

  IROHA_INITIAL_PEERS_0_ADDRESS=127.0.0.1:10001
  IROHA_INITIAL_PEERS_0_PUBLIC_KEY=bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929
  IROHA_INITIAL_PEERS_1_ADDRESS=127.0.0.1:10002
  IROHA_INITIAL_PEERS_1_PUBLIC_KEY=bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308920

**ユーザー定義キーを持つ辞書の場合**

ユーザーが提供する辞書キーは少し複雑です。キーと値は別々の変数で設定されます。
暗号プロバイダーの設定例で説明できます。

暗号プロバイダー
================

現在、HL Iroha は ``default`` と呼ばれる 1 種類のプロバイダーをサポートしています。これは HL Iroha 暗号ライブラリ（暗号タイプ ``ed25519_sha3_256``）や HL Ursa ライブラリ（Iroha は現在暗号タイプ ``ed25519_sha2_256`` をサポート）などの組み込み暗号プロバイダーを含みます。

これらはどちらも ``crypto_type`` と ``private_key`` のみをパラメーターとして取ります。

.. note::  他のタイプ、ハードウェアセキュリティモジュール用の ``hsm`` を含む追加のタイプの追加に取り組んでいます。これらは異なるパラメーターセットを持ち、リリース後にここに追加されます。

暗号プロバイダーの設定
----------------------------

現在利用可能な暗号プロバイダーを設定するには、ピアで使用される可能性のあるプロバイダーを定義し（``p1`` と ``p2`` を参照）、このノードでメッセージの署名に使用される ``signer`` を選択する必要があります:

設定ファイルでは:

.. code-block:: javascript

  "crypto": {
    "providers": {
      "p1": {
        "crypto_type": "ed25519_sha3_256",
        "private_key": "cc5013e43918bd0e5c4d800416c88bed77892ff077929162bb03ead40a745e88",
        "type": "default"
      },
      "p2": {
        "crypto_type": "ed25519_sha2_256",
        "private_key": "7bab70e95cb585ea052c3aeb27de0afa9897ba5746276aa1c25310383216ceb860eb82baacbc940e710a40f21f962a3651013b90c23ece31606752f298c38d90",
        "type": "default"
      }
    },
    "signer": "p1"
  }

環境変数では:

.. code-block:: bash

  IROHA_CRYPTO_PROVIDERS_0_KEY=p1
  IROHA_CRYPTO_PROVIDERS_0_CRYPTO_TYPE=ed25519_sha3_256
  IROHA_CRYPTO_PROVIDERS_0_PRIVATE_KEY=cc5013e43918bd0e5c4d800416c88bed77892ff077929162bb03ead40a745e88
  IROHA_CRYPTO_PROVIDERS_0_TYPE=default
  IROHA_CRYPTO_PROVIDERS_1_KEY=p2
  IROHA_CRYPTO_PROVIDERS_1_CRYPTO_TYPE=ed25519_sha2_256
  IROHA_CRYPTO_PROVIDERS_1_PRIVATE_KEY=7bab70e95cb585ea052c3aeb27de0afa9897ba5746276aa1c25310383216ceb860eb82baacbc940e710a40f21f962a3651013b90c23ece31606752f298c38d90
  IROHA_CRYPTO_PROVIDERS_1_TYPE=default
  IROHA_CRYPTO_SIGNER=p1


ロギング
========

Iroha では、必要なだけ細かくログを調整できます。
各コンポーネントは独自のロギング設定を持ち、プロパティは親から継承され、設定ファイルを通じて上書きできます。
これは、すべてのコンポーネントロガーが単一のルートを持つツリーに編成されていることを意味します。
設定ファイルの関連セクションには上書き値が含まれます:

設定ファイルでは:

.. code-block:: javascript

  "log": {
    "level": "trace",
    "patterns": {
      "debug": "don't panic, it's %v.",
      "error": "MAMA MIA! %v!"
    },
    "children": {
      "KeysManager": {
        "level": "trace"
      },
      "Irohad": {
        "children": {
          "Storage": {
            "level": "trace",
            "patterns": {
              "debug": "thread %t: %v."
            }
          }
        }
      }
    }
  }

環境変数では:

.. code-block:: bash

  IROHA_LOG_LEVEL=trace
  IROHA_LOG_PATTERNS_0_KEY=debug
  IROHA_LOG_PATTERNS_0="don't panic, it's %v."
  IROHA_LOG_PATTERNS_1_KEY=error
  IROHA_LOG_PATTERNS_1="MAMA MIA! %v!"
  IROHA_LOG_CHILDREN_0_KEY=KeysManager
  IROHA_LOG_CHILDREN_0_LEVEL=trace
  IROHA_LOG_CHILDREN_1_KEY=Irohad
  IROHA_LOG_CHILDREN_1_CHILDREN_0_KEY=Storage
  IROHA_LOG_CHILDREN_1_CHILDREN_0_LEVEL=trace
  IROHA_LOG_CHILDREN_1_CHILDREN_0_PATTERNS_0_KEY=debug
  IROHA_LOG_CHILDREN_1_CHILDREN_0_PATTERNS_0="thread %t: %v."

この設定セクションのすべての部分は任意です。

- ``level`` は詳細度を設定します。利用可能な値は（詳細度の高い順に）:

  - ``trace`` - すべてを出力
  - ``debug``
  - ``info``
  - ``warning``
  - ``error``
  - ``critical`` - 重大なメッセージのみを出力

- ``patterns`` は異なる詳細度レベルの各ログ文字列のフォーマットを制御します。各値は詳細度の低いレベルも上書きします。つまり、上の例では "don't panic" パターンは info および warning レベルにも適用され、trace レベルのパターンのみが設定で初期化されていません（デフォルトのハードコードされた値が設定されます）。

.. note::  単一のコンポーネントに複数のパターンが指定されていても、このコンポーネントは 1 つのパターン — 選択されたロギングレベルに対応するもの — のみを使用します。ただし、パターンは継承され、子ロガーで使用できます。

- ``children`` は子ノードの上書きを記述します。キーはコンポーネント名で、値はルートログ設定と同じ構文と意味を持ちます。
