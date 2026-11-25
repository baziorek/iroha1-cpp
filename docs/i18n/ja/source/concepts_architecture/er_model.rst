エンティティ・リレーションシップモデル
======================================

各 Hyperledger Iroha ピアは「World State View」と呼ばれる状態を持
ち、これはエンティティのセットとそれらの間のリレーションで表現され
ます。システムにどのエンティティが存在し、それらの間にどのような
リレーションがあるかを説明するため、このセクションには ER 図とそ
のコンポーネントの説明が含まれています。

ER 図
----

.. image:: ./../../image_assets/er-model.png

Peer
----

 - address — ネットワークアドレスと内部ポート。同期、コンセンサス、
   ordering サービスとの通信に使用される
 - public_key — コンセンサスプロセス中にブロック署名に使用される
   鍵

Asset
-----

 - asset_id — アセットの識別子。asset_name#domain_id の形式
 - domain_id — アセットが作成されたドメインの識別子。既存ドメイン
   への参照
 - precision — 小数部分のサイズ

Signatory
---------

 - public_key — 公開鍵

Domain
------

 - domain_id — ドメインの識別子
 - default_role — ドメインで作成されたユーザーごとのデフォルト
   ロール。既存ロールへの参照

Role
----

 - role_id — ロールの識別子

RoleHasPermissions
^^^^^^^^^^^^^^^^^^

 - role_id — ロールの識別子。既存ロールへの参照
 - permission_id — パーミッションの識別子

Account
-------

 - account_id — アカウントの識別子。account_name@domain_id の形式
 - domain_id — アカウントが作成されたドメインの識別子。既存ドメ
   インへの参照
 - quorum — このアカウントからの有効なトランザクション作成に必
   要な署名者の数
 - transaction_count – このアカウントが作成したトランザクション
   のカウンター
 - data — アカウントに関連する任意の情報のキー・バリュー
   ストレージ。サイズは 268435455 バイト（0x0FFFFFFF）に制限
   （PostgreSQL JSONB フィールド）。

AccountHasSignatory
^^^^^^^^^^^^^^^^^^^

 - account_id — アカウントの識別子。既存アカウントへの参照
 - public_key — 公開鍵（署名者とも呼ばれる）。既存署名者への参照

AccountHasAsset
^^^^^^^^^^^^^^^

 - account_id — アカウントの識別子。既存アカウントへの参照
 - asset_id — アセットの識別子。既存アセットへの参照
 - amount — アカウントが所有するアセットの量

AccountHasRoles
^^^^^^^^^^^^^^^

 - account_id — アカウントの識別子。既存アカウントへの参照
 - role_id — ロールの識別子。既存ロールへの参照

AccountHasGrantablePermissions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

 - account_id — アカウントの識別子。既存アカウントへの参照。こ
   のアカウントは、許可者に対して自身に対する操作を実行する伝達
   可能なパーミッションを与える。
 - permittee_account_id — アカウントの識別子。既存アカウントへ
   の参照。このアカウントは account_id に対する操作を実行する
   パーミッションを与えられる。
 - permission_id — grantable_permission の識別子
