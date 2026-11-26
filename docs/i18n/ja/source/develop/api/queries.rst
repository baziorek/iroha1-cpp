クエリ
=======

クエリとは、World State View —— ブロックチェーンの最新状態 —— の特定の部分に関連するリクエストである。
クエリはチェーンの内容を変更することはできず、受信したピアがクエリを処理した直後に、レスポンスがクライアントに返される。

バリデーション
^^^^^^^^^^^^^^

すべてのクエリのバリデーションには以下が含まれる:

- タイムスタンプ —— 過去のもの（ `Iroha 設定 <../../configure/index.html#environment-specific-parameters>`_ で設定可能）や未来のもの（ピア時刻に 5 分の範囲が加えられる）であってはならない
- クエリ作成者の署名 —— クエリ作成者のアイデンティティを確認するために使用される
- クエリカウンター —— クエリ作成者からの後続のすべてのクエリでインクリメントされていることがチェックされる
- ロール —— クエリ作成者のロールに応じて: クエリ可能な状態の範囲は、同一アカウント、ドメイン内のアカウント、チェーン全体、または一切許可されない、のいずれかに関連し得る

結果のページネーション
^^^^^^^^^^^^^^^^^^^^^^^^^^

一部のクエリは `TxPaginationMeta` をサポートしており、開発で利用できるさまざまな方法でクエリ結果をカスタマイズ・ソートすることができる。
ページネーションは、 `SQL 言語の ORDER BY <https://www.postgresql.org/docs/12/sql-select.html#SQL-ORDERBY>`_ に似た順序付けパラメータと連携して動作する —— 「結果行は指定された式（Iroha では Field）に従ってソートされる。最左の式で 2 つの行が等しい場合、次の式で比較され、以下同様に繰り返される」。

「式」は次のように指定される:

.. code-block:: proto

    enum Field {
        kCreatedTime = 0;
        kPosition = 1;
    }

順序付けの基準は 2 つある —— 作成時刻によるものと、ブロック番号によるものである。

各 Field には昇順と降順の方向がある:

.. code-block:: proto

    enum Direction {
        kAscending = 0;
        kDescending = 1;
    }

次に、順序付けそのもの:

.. code-block:: proto

    message Ordering {
        message FieldOrdering {
          Field field = 1;
          Direction direction = 2;
        }
        repeated FieldOrdering sequence = 1;
    }

順序付けが指定されたら、ページネーションを実行できる:

.. literalinclude:: ../../../../shared_model/schema/queries.proto
    :language: proto
    :start-at: message TxPaginationMeta {
    :end-before: message AssetPaginationMeta {


ページネーションの場合にリクエスト構造に追加されるもの
----------------------------------------------------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "ページサイズ", "クエリによって返されるページのサイズ。レスポンスがページサイズより少ないトランザクションしか含まない場合、レスポンス内の次の tx ハッシュは空になる", "page_size > 0", "5"
    "最初の tx ハッシュ", "ページ内の最初のトランザクションのハッシュ。このフィールドが設定されていない場合は、最初のトランザクションが返される", "16 進形式のハッシュ", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
    "ordering", "結果がどのようにソートされるべきか（ページネーション適用前）", "下記フィールドを参照", "下記フィールドを参照"
    "ordering.sequence", "SQL の ORDER BY のような順序指定", "フィールドと方向のシーケンス", "[{kCreatedTime, kAscending}, {kPosition, kDescending}]"
    "最初の tx 時刻", "クエリ結果内の最初のトランザクションの時刻。このフィールドが設定されていない場合は、最初からのトランザクションが返される", "Google Protocol Buffer Timestamp 型", "0001-01-01T00:00:00Z <= first tx time <= 9999-12-31T23:59:59.999999999Z"
    "最後の tx 時刻", "クエリ結果内の最後のトランザクションの時刻。このフィールドが設定されていない場合は、最後までのトランザクションが返される", "Google Protocol Buffer Timestamp 型", "0001-01-01T00:00:00Z <= last tx time <= 9999-12-31T23:59:59.999999999Z"
    "最初の tx 高さ", "クエリ結果内の最初のトランザクションのブロック高さ。このフィールドが設定されていない場合は、高さ 1 からのトランザクションが返される", "first tx height > 0", "4"
    "最後の tx 高さ", "クエリ結果内の最後のトランザクションのブロック高さ。このフィールドが設定されていない場合は、最後のトランザクションまでが返される", "last tx height > 0", "6"
    
エンジンレシート
^^^^^^^^^^^^^^^^^^^

目的
----

CallEngine コマンドのレシートを取得する。
Ethereum JSON RPC API の eth.GetTransactionReceipt API 呼び出しに類似している。
EVM 内部の計算中に作成されたイベントログにアクセスすることができる。

リクエストスキーマ
-------------------

.. code-block:: proto

   message GetEngineReceipts {
       string tx_hash = 1;     // hex string
   }

リクエスト構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "トランザクションハッシュ", "CallEngine コマンドを含むトランザクションのハッシュ", "16 進形式のハッシュ", "5241f70cf3adbc180199c1d2d02db82334137aede5f5ed35d649bbbc75ab2634"

レスポンススキーマ
-------------------

.. code-block:: proto

    message EngineReceiptsResponse {
        repeated EngineReceipt engine_receipt = 1;
    }
    message EngineReceipt {
        int32 command_index = 1;
        string caller = 2;
        oneof opt_to_contract_address {
            CallResult call_result = 3;
            string contract_address = 4;
        }
        repeated EngineLog logs = 5;
    }
    message CallResult {
        string callee = 1;
        string result_data = 2;
    }
    message EngineLog {
        string address = 1;         // hex string
        string data = 2;            // hex string
        repeated string topics = 3; // hex string
    }

レスポンス構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "command_index", "トランザクション内での CallEngine コマンドのインデックス", "非負整数", "0"
    "caller", "スマートコントラクトの呼び出し元アカウント", "<account_name>@<domain_id>", "admin@test"
    "call_result.callee", "呼び出されたコントラクトのアドレス", "16 進表現の 20 バイト文字列", "0000000000000000000000000000000000000000"
    "call_result.result_data", "コントラクトが返した値", "16 進表現の文字列", "00"
    "contract_address", "新たにデプロイされたコントラクトの EVM アドレス", "16 進表現の 20 バイト文字列", "7C370993FD90AF204FD582004E2E54E6A94F2651"
    "logs", "スマートコントラクト実行中に作成された EVM イベントログの配列。", "下記を参照", "下記を参照"
    "logs.[].address", "コントラクト呼び出し元の EVM アドレス", "16 進表現の 20 バイト文字列", "577266A3CE7DD267A4C14039416B725786605FF4"
    "logs.[].data", "ログに記録されたデータ", "16 進文字列", "0000000000000000000000007203DF5D7B4F198848477D7F9EE080B207E544DD000000000000000000000000000000000000000000000000000000000000006D"
    "logs.[].topics", "Ethereum と同様のログトピック", "32 バイト文字列", "[3990DB2D31862302A685E8086B5755072A6E2B5B780AF1EE81ECE35EE3CD3345, 000000000000000000000000969453762B0C739DD285B31635EFA00E24C25628]"


考えられるステートフルバリデーションエラー
-------------------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "2", "No such permissions", "クエリの作成者にエンジンレシートを取得するためのいずれのパーミッションもない", "必要なパーミッションを付与する"
    "3", "Invalid signatures", "このクエリの署名がバリデーションを通過しなかった", "署名を追加し、クエリの署名がアカウントの署名者のサブセットであることを確認する"

アカウントの取得
^^^^^^^^^^^^^^^^^

目的
----

アカウント取得クエリの目的は、アカウントの状態を取得することである。

リクエストスキーマ
-------------------

.. code-block::proto

    message GetAccount {
        string account_id = 1;
    }

リクエスト構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "アカウント ID", "状態をリクエストするアカウント ID", "<account_name>@<domain_id>", "alex@morgan"

レスポンススキーマ
-------------------

.. code-block:: proto

    message AccountResponse {
        Account account = 1;
        repeated string account_roles = 2;
    }

    message Account {
        string account_id = 1;
        string domain_id = 2;
        uint32 quorum = 3;
        string json_data = 4;
    }


レスポンス構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "アカウント ID", "アカウント ID", "<account_name>@<domain_id>", "alex@morgan"
    "ドメイン ID", "アカウントが作成されたドメイン", "RFC1035 [#f1]_, RFC1123 [#f2]_ ", "morgan"
    "クォーラム", "トランザクションを有効にするために署名が必要な署名者の数", "0 < quorum ≤ 128", "5"
    "JSON データ", "キー・バリュー形式のアカウント情報", "JSON", "{ genesis: {name: alex} }"

考えられるステートフルバリデーションエラー
-------------------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "Could not get account", "内部エラーが発生した", "再試行するか、開発者に連絡する"
    "2", "No such permissions", "クエリの作成者にアカウントを取得するためのいずれのパーミッションもない", "必要なパーミッションを付与する: 個別、グローバル、またはドメインのもの"
    "3", "Invalid signatures", "このクエリの署名がバリデーションを通過しなかった", "署名を追加し、クエリの署名がアカウントの署名者のサブセットであることを確認する"

ブロックの取得
^^^^^^^^^^^^^^^

目的
----

ブロック取得クエリの目的は、その高さを識別子として特定のブロックを取得することである。

リクエストスキーマ
-------------------

.. code-block:: proto

    message GetBlock {
        uint64 height = 1;
    }


リクエスト構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "高さ", "取得するブロックの高さ", "0 < height < 2^64", "42"

レスポンススキーマ
-------------------

.. code-block:: proto

    message BlockResponse {
        Block block = 1;
    }

レスポンス構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "ブロック", "取得されたブロック", "ブロック構造", "block"

考えられるステートフルバリデーションエラー
-------------------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "Could not get block", "内部エラーが発生した", "再試行するか、開発者に連絡する"
    "2", "No such permissions", "クエリの作成者にブロックを取得するパーミッションがない", "`can_get_block <permissions.html#can-get-blocks>`__ パーミッションを付与する"
    "3", "Invalid signatures", "このクエリの署名がバリデーションを通過しなかった", "署名を追加し、クエリの署名がアカウントの署名者のサブセットであることを確認する"
    "3", "Invalid height", "指定された高さが uint_64 でないか、台帳の高さより大きい", "高さを確認して再試行する"

.. note::
    このクエリではエラーコード 3 は曖昧である。
    無効な署名者または無効な高さのいずれかを示す。
    無効な署名者をチェックするには、`height = 1` （最初のブロックは常に存在する）でこのメソッドを使用する。

署名者の取得
^^^^^^^^^^^^^

目的
----

署名者取得クエリの目的は、アカウントのアイデンティティとして機能する署名者を取得することである。

リクエストスキーマ
-------------------

.. code-block:: proto

    message GetSignatories {
        string account_id = 1;
    }

リクエスト構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "アカウント ID", "署名者をリクエストするアカウント ID", "<account_name>@<domain_id>", "alex@morgan"

レスポンススキーマ
-------------------

.. code-block:: proto

    message SignatoriesResponse {
        repeated bytes keys = 1;
    }

レスポンス構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "Keys", "公開鍵の配列", "`ed25519 <https://ed25519.cr.yp.to>`_", "292a8714694095edce6be799398ed5d6244cd7be37eb813106b217d850d261f2"

考えられるステートフルバリデーションエラー
-------------------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "Could not get signatories", "内部エラーが発生した", "再試行するか、開発者に連絡する"
    "2", "No such permissions", "クエリの作成者に署名者を取得するためのいずれのパーミッションもない", "必要なパーミッションを付与する: 個別、グローバル、またはドメインのもの"
    "3", "Invalid signatures", "このクエリの署名がバリデーションを通過しなかった", "署名を追加し、クエリの署名がアカウントの署名者のサブセットであることを確認する"

トランザクションの取得
^^^^^^^^^^^^^^^^^^^^^^^^

目的
----

GetTransactions は、ハッシュに基づいてトランザクションに関する情報を取得するために使用される。

.. note:: このクエリは、リクエストされたすべてのハッシュが正しい場合にのみ有効である: 対応するトランザクションが存在し、かつユーザーがそれらを取得するパーミッションを持っていること。

リクエストスキーマ
-------------------

.. code-block:: proto

    message GetTransactions {
        repeated bytes tx_hashes = 1;
    }

リクエスト構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "トランザクションハッシュ", "ハッシュの配列", "32 バイトハッシュの配列", "{hash1, hash2…}"

レスポンススキーマ
-------------------

.. code-block:: proto

    message TransactionsResponse {
        repeated Transaction transactions = 1;
    }

レスポンス構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "トランザクション", "トランザクションの配列", "コミットされたトランザクション", "{tx1, tx2…}"

考えられるステートフルバリデーションエラー
-------------------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "Could not get transactions", "内部エラーが発生した", "再試行するか、開発者に連絡する"
    "2", "No such permissions", "クエリの作成者にトランザクションを取得するためのいずれのパーミッションもない", "必要なパーミッションを付与する: 個別、グローバル、またはドメインのもの"
    "3", "Invalid signatures", "このクエリの署名がバリデーションを通過しなかった", "署名を追加し、クエリの署名がアカウントの署名者のサブセットであることを確認する"
    "4", "Invalid hash", "指定されたハッシュの少なくとも 1 つがユーザーのトランザクションリストに存在しないか、クエリ作成者にそれを見るパーミッションがない", "指定されたハッシュを確認して再試行する"

保留中トランザクションの取得
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

目的
----

GetPendingTransactions は、クエリ作成者のアカウントによって発行された、保留中の（完全に署名されていない） `マルチシグネチャトランザクション <../../concepts_architecture/glossary.html#multisignature-transactions>`_
または `トランザクションバッチ <../../concepts_architecture/glossary.html#batch-of-transactions>`__ のリストを取得するために使用される。

.. note:: このクエリは、より迅速で便利なクエリレスポンスのために `ページネーション <#result-pagination>`_ を使用する。クエリリクエストを送信する前に、これと `リクエスト構造 <#what-is-added-to-the-request-structure-in-case-of-pagination>`_ について読んでページネーションを指定すること。

リクエストスキーマ
-------------------

.. code-block:: proto

    message GetPendingTransactions {
        TxPaginationMeta pagination_meta = 1;
    }

ユーザーの半署名済みマルチシグネチャ（保留中）トランザクションをすべてクエリできる。
レスポンスに含まれるトランザクションの最大数は **page_size** フィールドで制限できる。
すべての保留中トランザクションは、十分な署名が集まるか期限切れになるまで保存される。
保留中トランザクションまたはトランザクションバッチの相互の順序は、ユーザーに対して保持される。
これにより、ユーザーはすべてのトランザクションを順次 —— ページごとに —— クエリできる。
各レスポンスには、クエリできる次のバッチまたはトランザクションへの参照が含まれる場合がある。
ページサイズは、後続バッチ（トランザクション数で）のサイズより大きい場合がある。
その場合、複数のバッチまたはトランザクションが返される。
ページ間を移動する間に、後続バッチがクエリされる前に不足している署名を収集する可能性がある。
これは、バッチのハッシュが欠けているため、ステートフルな失敗クエリレスポンスをもたらす。

例
----

それぞれ 3 つのトランザクションを含む 2 つの保留中バッチがあり、ユーザーがページサイズ 5 で保留中トランザクションをクエリした場合、
最初のバッチのトランザクションがレスポンスに含まれ、2 番目のバッチへの参照
（最初のトランザクションハッシュとバッチサイズ。実際には単一トランザクションの場合でも）も指定される。
2 番目のバッチのトランザクションは最初のレスポンスに含まれない。バッチはいくつかの部分に分割できず、
完全なバッチのみがレスポンスに含められるためである。

レスポンススキーマ
-------------------

.. code-block:: proto

    message PendingTransactionsPageResponse {
        message BatchInfo {
            string first_tx_hash = 1;
            uint32 batch_size = 2;
        }
        repeated Transaction transactions = 1;
        uint32 all_transactions_size = 2;
        BatchInfo next_batch_info = 3;
    }

レスポンス構造
---------------

レスポンスには、 `保留中トランザクション <../../concepts_architecture/glossary.html#pending-transactions>`_ のリスト、
ユーザーのすべての保存済み保留中トランザクションの数、
および後続ページをクエリするために必要な情報（存在する場合）が含まれる。

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

        "トランザクション", "保留中トランザクションの配列", "保留中トランザクション", "{tx1, tx2…}"
        "全トランザクション数", "保存されているトランザクションの数", "all_transactions_size >= 0", "0"
        "次のバッチ情報", "次のページへの参照 —— レスポンスにメッセージが設定されない場合がある", "", ""
        "最初の tx ハッシュ", "次のバッチ内の最初のトランザクションのハッシュ",  "16 進形式のハッシュ", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"
        "バッチサイズ", "次のバッチを取得するために必要な最小ページサイズ", "batch_size > 0", "3"

保留中トランザクションの取得（非推奨）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. warning::
  パラメータなしのクエリは現在非推奨であり、次のメジャーな Iroha リリース（2.0）で削除される予定である。
  代わりに新しいクエリバージョンを使用すること: `保留中トランザクションの取得 <#get-pending-transactions>`__。

目的
----

GetPendingTransactions は、クエリ作成者のアカウントによって発行された、保留中の（完全に署名されていない） `マルチシグネチャトランザクション <../../concepts_architecture/glossary.html#multisignature-transactions>`_
または `トランザクションバッチ <../../concepts_architecture/glossary.html#batch-of-transactions>`__ のリストを取得するために使用される。

リクエストスキーマ
-------------------

.. code-block:: proto

    message GetPendingTransactions {
    }

レスポンススキーマ
-------------------

.. code-block:: proto

    message TransactionsResponse {
        repeated Transaction transactions = 1;
    }

レスポンス構造
---------------

レスポンスには、 `保留中トランザクション <../../concepts_architecture/glossary.html#pending-transactions>`_ のリストが含まれる。

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

        "トランザクション", "保留中トランザクションの配列", "保留中トランザクション", "{tx1, tx2…}"

考えられるステートフルバリデーションエラー
-------------------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "Could not get pending transactions", "内部エラーが発生した", "再試行するか、開発者に連絡する"
    "2", "No such permissions", "クエリの作成者に保留中トランザクションを取得するためのいずれのパーミッションもない", "必要なパーミッションを付与する: 個別、グローバル、またはドメインのもの"
    "3", "Invalid signatures", "このクエリの署名がバリデーションを通過しなかった", "署名を追加し、クエリの署名がアカウントの署名者のサブセットであることを確認する"

アカウントトランザクションの取得
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

目的
----

アカウントごとのトランザクションリストが必要な場合、 `GetAccountTransactions` クエリを構成できる。

.. note:: このクエリは、より迅速で便利なクエリレスポンスのために `ページネーション <#result-pagination>`_ を使用する。クエリリクエストを送信する前に、これと `リクエスト構造 <#what-is-added-to-the-request-structure-in-case-of-pagination>`_ について読んでページネーションを指定すること。

リクエストスキーマ
-------------------

.. code-block:: proto

    message GetAccountTransactions {
        string account_id = 1;
        TxPaginationMeta pagination_meta = 2;
    }

リクエスト構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "アカウント ID", "トランザクションをリクエストするアカウント ID", "<account_name>@<domain_id>", "makoto@soramitsu"

レスポンススキーマ
-------------------

.. code-block:: proto

    message TransactionsPageResponse {
        repeated Transaction transactions = 1;
        uint32 all_transactions_size = 2;
        oneof next_page_tag {
            string next_tx_hash = 3;
        }
    }

考えられるステートフルバリデーションエラー
-------------------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "Could not get account transactions", "内部エラーが発生した", "再試行するか、開発者に連絡する"
    "2", "No such permissions", "クエリの作成者にアカウントトランザクションを取得するためのいずれのパーミッションもない", "必要なパーミッションを付与する: 個別、グローバル、またはドメインのもの"
    "3", "Invalid signatures", "このクエリの署名がバリデーションを通過しなかった", "署名を追加し、クエリの署名がアカウントの署名者のサブセットであることを確認する"
    "4", "Invalid pagination hash", "指定されたハッシュがユーザーのトランザクションのいずれにも現れない", "ハッシュが正しいことを確認して再試行する"
    "5", "Invalid account id", "そのようなアカウント ID を持つユーザーが存在しない", "アカウント ID が正しいことを確認する"

レスポンス構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "トランザクション", "指定されたアカウントのトランザクションの配列", "コミットされたトランザクション", "{tx1, tx2…}"
    "全トランザクション数", "指定されたアカウントによって作成されたトランザクションの総数", "", "100"
    "次のトランザクションハッシュ", "ページ内の最後のトランザクションの次のトランザクションを指すハッシュ。指定されたアカウントの最後のトランザクションがページに含まれる場合は空", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"

アカウントアセットトランザクションの取得
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

目的
----

`GetAccountAssetTransactions` クエリは、指定されたアカウントとアセットに関連付けられたすべてのトランザクションを返す。

.. note:: このクエリは、より迅速で便利なクエリレスポンスのために `ページネーション <#result-pagination>`_ を使用する。クエリリクエストを送信する前に、これと `リクエスト構造 <#what-is-added-to-the-request-structure-in-case-of-pagination>`_ について読んでページネーションを指定すること。

リクエストスキーマ
-------------------

.. code-block:: proto

    message GetAccountAssetTransactions {
        string account_id = 1;
        string asset_id = 2;
        TxPaginationMeta pagination_meta = 3;
    }

リクエスト構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "アカウント ID", "トランザクションをリクエストするアカウント ID", "<account_name>@<domain_id>", "makoto@soramitsu"
    "アセット ID", "このアセットを含むトランザクションでフィルタリングするためのアセット ID", "<asset_name>#<domain_id>", "jpy#japan"

レスポンススキーマ
-------------------

.. code-block:: proto

    message TransactionsPageResponse {
        repeated Transaction transactions = 1;
        uint32 all_transactions_size = 2;
        oneof next_page_tag {
            string next_tx_hash = 3;
        }
    }

レスポンス構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "トランザクション", "指定されたアカウントとアセットのトランザクションの配列", "コミットされたトランザクション", "{tx1, tx2…}"
    "全トランザクション数", "指定されたアカウントとアセットのトランザクションの総数", "", "100"
    "次のトランザクションハッシュ", "ページ内の最後のトランザクションの次のトランザクションを指すハッシュ。指定されたアカウントとアセットの最後のトランザクションがページに含まれる場合は空", "bddd58404d1315e0eb27902c5d7c8eb0602c16238f005773df406bc191308929"

考えられるステートフルバリデーションエラー
-------------------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "Could not get account asset transactions", "内部エラーが発生した", "再試行するか、開発者に連絡する"
    "2", "No such permissions", "クエリの作成者にアカウントアセットトランザクションを取得するためのいずれのパーミッションもない", "必要なパーミッションを付与する: 個別、グローバル、またはドメインのもの"
    "3", "Invalid signatures", "このクエリの署名がバリデーションを通過しなかった", "署名を追加し、クエリの署名がアカウントの署名者のサブセットであることを確認する"
    "4", "Invalid pagination hash", "指定されたハッシュがユーザーのトランザクションのいずれにも現れない", "ハッシュが正しいことを確認して再試行する"
    "5", "Invalid account id", "そのようなアカウント ID を持つユーザーが存在しない", "アカウント ID が正しいことを確認する"
    "6", "Invalid asset id", "そのようなアセット ID を持つアセットが存在しない", "アセット ID が正しいことを確認する"

アカウントアセットの取得
^^^^^^^^^^^^^^^^^^^^^^^^^^

目的
----

アカウント内のすべてのアセットの状態（残高）を取得するには、 `GetAccountAssets` クエリを使用できる。

リクエストスキーマ
-------------------

.. code-block:: proto

    message AssetPaginationMeta {
        uint32 page_size = 1;
        oneof opt_first_asset_id {
            string first_asset_id = 2;
        }
    }

    message GetAccountAssets {
        string account_id = 1;
        AssetPaginationMeta pagination_meta = 2;
    }

リクエスト構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "アカウント ID", "残高をリクエストするアカウント ID", "<account_name>@<domain_id>", "makoto@soramitsu"
    AssetPaginationMeta.page_size, "リクエストされたページサイズ。レスポンス内のアセット数はこの値を超えない。レスポンスが切り詰められた場合、返されたアセットの直後のアセット ID が next_asset_id で提供される。", 0 < page_size < 32 bit unsigned int max (4294967296), 100
    AssetPaginationMeta.first_asset_id, "リクエストされたページの開始位置。このフィールドが設定されていない場合は、最初のページが返される。", name#domain, my_asset#my_domain

レスポンススキーマ
-------------------
.. code-block:: proto

    message AccountAssetResponse {
        repeated AccountAsset account_assets = 1;
        uint32 total_number = 2;
        oneof opt_next_asset_id {
            string next_asset_id = 3;
        }
    }

    message AccountAsset {
        string asset_id = 1;
        string account_id = 2;
        string balance = 3;
    }

レスポンス構造
---------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "アセット ID", "残高の確認に使用されるアセットの識別子", "<asset_name>#<domain_id>", "jpy#japan"
    "アカウント ID", "この残高を持つアカウント", "<account_name>@<domain_id>", "makoto@soramitsu"
    "残高", "アセットの残高", "0 以上", "200.20"
    total_number, ページ制限なしでクエリに一致するアセット数, 0 < total_number < 32 bit unsigned int max (4294967296), 100500
    next_asset_id, 現在のページの直後のアセットの ID, name#domain, my_asset#my_domain

.. note::
   ページサイズが他のリクエスト条件に一致するアセット数以上の場合、レスポンス内の次のアセット ID は未設定になる。
   それ以外の場合、クライアントが次のページを取得したい場合に最初のアセット ID として使用すべき値が含まれる。


考えられるステートフルバリデーションエラー
-------------------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "Could not get account assets", "内部エラーが発生した", "再試行するか、開発者に連絡する"
    "2", "No such permissions", "クエリの作成者にアカウントアセットを取得するためのいずれのパーミッションもない", "必要なパーミッションを付与する: 個別、グローバル、またはドメインのもの"
    "3", "Invalid signatures", "このクエリの署名がバリデーションを通過しなかった", "署名を追加し、クエリの署名がアカウントの署名者のサブセットであることを確認する"
    "4", "Invalid pagination metadata", "不正なページサイズまたは存在しない最初のアセット", "有効なページサイズを設定し、アセット ID が有効であることを確認するか、最初のアセット ID を指定しない"

アカウント詳細の取得
^^^^^^^^^^^^^^^^^^^^

目的
-------

アカウントの詳細を取得するには `GetAccountDetail` クエリを使用できる。アカウント詳細はキーと値のペアであり、ライターごとのカテゴリに分割される。ライターとは、対応するアカウント詳細を追加したアカウントである。このような構造の例は次の通りである。

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18,
            "hobbies": "crypto"
        },
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

ここでは、4つのアカウント詳細 - "age"、"hobbies"、"sports" - が2人のライター - "account@a_domain" と "account@b_domain" - によって追加されていることがわかる。これらの詳細はすべて明らかに同じアカウントに関するものである。

リクエストスキーマ
------------------

.. code-block:: proto

    message AccountDetailRecordId {
        string writer = 1;
        string key = 2;
    }

    message AccountDetailPaginationMeta {
        uint32 page_size = 1;
        AccountDetailRecordId first_record_id = 2;
    }

    message GetAccountDetail {
        oneof opt_account_id {
          string account_id = 1;
        }
        oneof opt_key {
          string key = 2;
        }
        oneof opt_writer {
          string writer = 3;
        }
        AccountDetailPaginationMeta pagination_meta = 4;
    }

.. note::
    pagination meta 以外のすべてのフィールドがオプションであることに注意。
    その理由は以下で説明する。

.. warning::
    互換性の理由により、リクエストでページネーションメタデータが欠落している場合があるが、この動作は非推奨であり避けるべきである。

リクエスト構造
-----------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

        "アカウント ID", "詳細を取得するアカウント ID", "<account_name>@<domain_id>", "account@domain"
        "キー", "詳細を取得するキー", "string", "age"
        "ライター", "ライターのアカウント ID", "<account_name>@<domain_id>", "account@domain"
        AccountDetailPaginationMeta.page_size, "リクエストされたページサイズ。レスポンス内のレコード数はこの値を超えない。レスポンスが切り詰められた場合、返されたレコードの直後のレコード ID が next_record_id で提供される。", 0 < page_size < 32 bit unsigned int max (4294967296), 100
        AccountDetailPaginationMeta.first_record_id.writer, ライターによるリクエストページの開始位置, name#domain, my_asset#my_domain
        AccountDetailPaginationMeta.first_record_id.key, キーによるリクエストページの開始位置, string, age

.. note::
    先頭レコード ID を指定する場合、メインクエリで未設定の属性 (writer, key) を提供すれば十分である。

レスポンススキーマ
------------------

.. code-block:: proto

    message AccountDetailResponse {
        string detail = 1;
        uint64 total_number = 2;
        AccountDetailRecordId next_record_id = 3;
    }

レスポンス構造
------------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

        "詳細", "アカウント詳細を含むキーと値のペア", "JSON", "下記参照"
        total_number, ページ制限なしでクエリに一致するレコード数, 0 < total_number < 32 bit unsigned int max (4294967296), 100
        next_record_id.writer, 現在のページの直後のレコードのライターアカウント, <account_name>@<domain_id>, pushkin@lyceum.tsar
        next_record_id.key, 現在のページの直後のレコードのキー, string, "cold and sun"

ステートフルバリデーションエラー
-----------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "アカウント詳細を取得できませんでした", "内部エラーが発生しました", "再試行するか開発者に連絡してください"
    "2", "権限がありません", "クエリの作成者にはアカウント詳細を取得する権限がいずれもありません", "必要な権限 (個人、グローバル、またはドメイン) を付与してください"
    "3", "無効な署名", "このクエリの署名が検証を通過しませんでした", "署名を追加し、クエリの署名がアカウントの署名者の部分集合であることを確認してください"
    "4", "無効なページネーションメタデータ", "ページサイズが不正、または先頭レコードが存在しません", "有効なページサイズを設定し、先頭レコード ID が有効であることを確認するか、先頭レコード ID を未指定のままにしてください"

使用例
--------------

再度、冒頭の詳細の例を検討し、異なるバリアントの `GetAccountDetail` クエリが結果のレスポンスをどのように変えるかを見てみよう。

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18,
            "hobbies": "crypto"
        },
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

**account_id が未設定の場合**

account_id が未設定の場合 - 他のフィールドが空でも空でなくても - 自動的にクエリ作成者のアカウントで置き換えられ、次のいずれかのケースにつながる。

**account_id のみが設定されている場合**

この場合、そのアカウントに関するすべての詳細が返され、次のレスポンスとなる:

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18,
            "hobbies": "crypto"
        },
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

**account_id と key が設定されている場合**

ここでは、すべてのライターによってそのキーの下に追加された詳細が返される。例えば、キー "age" を要求した場合、次のレスポンスが得られる:

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18
        },
        "account@b_domain": {
            "age": 20
        }
    }

**account_id と writer が設定されている場合**

この場合、レスポンスにはこのアカウントに関する、特定のライター1人によって追加されたすべての詳細が含まれる。例えば、ライター "account@b_domain" を要求した場合、次のようになる:

.. code-block:: json

    {
        "account@b_domain": {
            "age": 20,
            "sports": "basketball"
        }
    }

**account_id、key、writer がすべて設定されている場合**

最後に、3つのフィールドすべてが設定されている場合、結果には特定のライターが特定のキーの下に追加した詳細が含まれる。例えば、キー "age" とライター "account@a_domain" を要求した場合、次のようになる:

.. code-block:: json

    {
        "account@a_domain": {
            "age": 18
        }
    }

アセット情報の取得
^^^^^^^^^^^^^^^^^^

目的
-------

指定されたアセットに関する情報 (現在のところ、その精度) を取得するために、ユーザーは `GetAssetInfo` クエリを送信できる。

リクエストスキーマ
------------------

.. code-block:: proto

    message GetAssetInfo {
        string asset_id = 1;
    }

リクエスト構造
-----------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "アセット ID", "関連情報を知りたいアセット ID", "<asset_name>#<domain_id>", "jpy#japan"


レスポンススキーマ
------------------

.. code-block:: proto

    message Asset {
        string asset_id = 1;
        string domain_id = 2;
        uint32 precision = 3;
    }

.. note::
    既知の問題により、無効な precision 値を渡しても例外は発生しないことに注意。
    有効な範囲は: 0 <= precision <= 255

ステートフルバリデーションエラー
-----------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "アセット情報を取得できませんでした", "内部エラーが発生しました", "再試行するか開発者に連絡してください"
    "2", "権限がありません", "クエリの作成者にはアセット情報を取得する権限がいずれもありません", "必要な権限 (個人、グローバル、またはドメイン) を付与してください"
    "3", "無効な署名", "このクエリの署名が検証を通過しませんでした", "署名を追加し、クエリの署名がアカウントの署名者の部分集合であることを確認してください"

レスポンス構造
------------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "アセット ID", "残高確認に使用されるアセットの識別子", "<asset_name>#<domain_id>", "jpy#japan"
    "ドメイン ID", "このアセットに関連するドメイン", "RFC1035 [#f1]_, RFC1123 [#f2]_", "japan"
    "精度", "カンマの後の桁数", "0 <= precision <= 255", "2"

ロールの取得
^^^^^^^^^^^^

目的
-------

システム内の既存のロールを取得するために、ユーザーは `GetRoles` クエリを Iroha ネットワークに送信できる。

リクエストスキーマ
------------------

.. code-block:: proto

    message GetRoles {
    }

レスポンススキーマ
------------------

.. code-block:: proto

    message RolesResponse {
        repeated string roles = 1;
    }

レスポンス構造
------------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "ロール", "ネットワーク内に作成されたロールの配列", "システム内のロールの集合", "{MoneyCreator, User, Admin, …}"

ステートフルバリデーションエラー
-----------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "ロールを取得できませんでした", "内部エラーが発生しました", "再試行するか開発者に連絡してください"
    "2", "権限がありません", "クエリの作成者にはロールを取得する権限がいずれもありません", "必要な権限 (個人、グローバル、またはドメイン) を付与してください"
    "3", "無効な署名", "このクエリの署名が検証を通過しませんでした", "署名を追加し、クエリの署名がアカウントの署名者の部分集合であることを確認してください"

ロール権限の取得
^^^^^^^^^^^^^^^^^^^^

目的
-------

システム内のロールごとに利用可能な権限を取得するために、ユーザーは `GetRolePermissions` クエリを Iroha ネットワークに送信できる。

リクエストスキーマ
------------------

.. code-block:: proto

    message GetRolePermissions {
        string role_id = 1;
    }

リクエスト構造
-----------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "ロール ID", "権限を取得するロール", "システム内の既存のロール", "MoneyCreator"

レスポンススキーマ
------------------

.. code-block:: proto

    message RolePermissionsResponse {
        repeated string permissions = 1;
    }

レスポンス構造
------------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "権限", "ロールに関連する権限の配列", "ロールに関連する権限の文字列", "{can_add_asset_qty, …}"

ステートフルバリデーションエラー
-----------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "ロール権限を取得できませんでした", "内部エラーが発生しました", "再試行するか開発者に連絡してください"
    "2", "権限がありません", "クエリの作成者にはロール権限を取得する権限がいずれもありません", "必要な権限 (個人、グローバル、またはドメイン) を付与してください"
    "3", "無効な署名", "このクエリの署名が検証を通過しませんでした", "署名を追加し、クエリの署名がアカウントの署名者の部分集合であることを確認してください"

.. [#f1] https://www.ietf.org/rfc/rfc1035.txt
.. [#f2] https://www.ietf.org/rfc/rfc1123.txt


ピアの取得
^^^^^^^^^^

目的
-------

Iroha ネットワーク内のピアのリストを返すクエリ。

リクエストスキーマ
------------------

.. code-block:: proto

    message GetPeers {
    }

レスポンススキーマ
------------------

.. code-block:: proto

    message Peer {
        string address = 1;
        string peer_key = 2; // hex string
    }

    message PeersResponse {
        repeated Peer peers = 1;
    }

レスポンス構造
------------------

ピアのアドレスと公開鍵を含むピアのリストが返される。

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "ピア", "ネットワークからのピアの配列", "空でないピアのリスト", "{Peer{""peer.domain.com"", ""292a8714694095edce6be799398ed5d6244cd7be37eb813106b217d850d261f2""}, …}"

ステートフルバリデーションエラー
-----------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "ピアを取得できませんでした", "内部エラーが発生しました", "再試行するか開発者に連絡してください"
    "2", "権限がありません", "クエリの作成者にはピアを取得する十分な権限がありません", "can_get_blocks または can_get_peers 権限を持つロールを追加してください"
    "3", "無効な署名", "このクエリの署名が検証を通過しませんでした", "署名を追加し、クエリの署名がアカウントの署名者の部分集合であることを確認してください"

.. warning::

    現在、Get Peers クエリは互換性のために "can_get_blocks" 権限を使用している。
    次の Iroha のメジャーリリースで "can_get_peers" に変更される予定である。

コミットの取得
^^^^^^^^^^^^^^

目的
-------

ブロックがコミットされ次第すぐに新しいブロックを取得するために、ユーザーは `FetchCommits` RPC 呼び出しを Iroha ネットワークに対して実行できる。

リクエストスキーマ
------------------

リクエスト引数は不要


レスポンススキーマ
------------------

.. code-block:: proto

    message BlockQueryResponse {
        oneof response {
            BlockResponse block_response = 1;
            BlockErrorResponse block_error_response = 2;
        }
    }

    message BlockResponse {
        Block block = 1;
    }

    message BlockErrorResponse {
        string message = 1;
    }

これは `BlockQueryResponse` のストリームを返すことに注意。

レスポンス構造
------------------

.. csv-table::
    :header: "フィールド", "説明", "制約", "例"
    :widths: 15, 30, 20, 15

    "ブロック", "Iroha ブロック", "コミット済みブロックのみ", "{ 'block_v1': ....}"

ステートフルバリデーションエラー
-----------------------------------

.. csv-table::
    :header: "コード", "エラー名", "説明", "解決方法"

    "1", "ブロックストリーミングを取得できませんでした", "内部エラーが発生しました", "再試行するか開発者に連絡してください"
    "2", "権限がありません", "クエリの作成者にはブロックを取得する権限がいずれもありません", "`can_get_block <permissions.html#can-get-blocks>`__ 権限を付与してください"
    "3", "無効な署名", "このクエリの署名が検証を通過しませんでした", "署名を追加し、クエリの署名がアカウントの署名者の部分集合であることを確認してください"

.. note::
    `BlockErrorResponse` には `message` フィールドのみが含まれる。
    ステートフルバリデーションエラーの場合、それは "stateful invalid" となる。
    `GetBlock <#get-block>`__ は同じ `can_get_block <permissions.html#can-get-blocks>`__ 権限を必要とする。
    したがって、`height = 1` (最初のブロックは常に存在する) で無効な署名者や不十分な権限をチェックするために使用できる。

例
-------
このクエリの使用例はここで確認できる:
https://github.com/x3medima17/twitter
