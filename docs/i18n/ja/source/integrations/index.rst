.. _integrations:

===================
統合プロジェクト
===================

Hyperledger Consortium のアイデアの 1 つは、互いに連携して可能な
限り最高のブロックチェーン体験を提供できるソリューションを
作ることです。Iroha では、他の素晴らしい Hyperledger ツールや
ソリューションとの統合が、ユースケースに合わせて Iroha をより
良くする方法だと考えています。だからこそ、私たちはいくつかの
プロジェクトとの統合に取り組み、Iroha が何と連携できるかを
もっとお伝えしたいと思います。

.. note::
	以下にリストされている統合はすべて**アーカイブ済み**です:
	相手方プロジェクトは End of Life に達したか、Iroha 1 の
	サポートを撤回しました。情報は歴史的参考のため、および
	古いデプロイメントを維持している人のために保持されていま
	す。

アーカイブ済みの統合
====================

以下の統合は Iroha 1 向けに開発されましたが、もはやメンテナンス
されていません。該当する場合、日付は Hyperledger TSC/TOC による
公式の End of Life 決定です。

.. _hyperledger-cacti:

Hyperledger Cacti（旧 Cactus）— Iroha コネクタは Cacti 2.0 で削除（2024）
------------------------------------------------------------------------------

`Hyperledger Cacti <https://github.com/hyperledger-cacti/cacti>`_
（旧 Hyperledger Cactus）は、異なるブロックチェーンと分散型
台帳が共通 API を通じて相互通信できるようにする相互運用
プラットフォームで、アプリケーションは個別の統合なしに複数の
台帳と連携できます。

Iroha には専用の Cactus コネクタがあり、wiki ページ
`Iroha と Cactus の統合
<https://lf-hyperledger.atlassian.net/wiki/spaces/INTERN/pages/21954723/HL+Iroha+and+HL+Cactus+Integration>`_
と `既存の Iroha–Cactus 統合の拡張
<https://lf-hyperledger.atlassian.net/wiki/spaces/INTERN/pages/21954791/Extend+existing+Iroha+-+Cactus+integration>`_
で説明されています。

.. note::
	Cacti 自体は依然としてアクティブな LF Decentralized Trust
	プロジェクトですが、**Iroha 1 コネクタは Cacti 2.0（2024）
	で削除され**、Iroha 2 コネクタも縮小されています。したがっ
	て、以下の資料は Iroha 1 デプロイメントにとって歴史的な
	興味を持つものです。

.. _hyperledger-ursa:

Hyperledger Ursa — EOL 2023 年 4 月 27 日
-----------------------------------------

`Hyperledger Ursa
<https://wiki.hyperledger.org/display/ursa/Hyperledger+Ursa>`_
は、個人（およびプロジェクト）が他の暗号化作業を重複させること
を避け、プロセス全体でセキュリティを高めることを可能にする
共有暗号化ライブラリです。このライブラリは、プロジェクト（お
よび潜在的に他の人）が暗号化コードを配置して使用するための
オプトインリポジトリです。Hyperledger Ursa は、暗号化コードの
まとまりのある実装またはそのインターフェースであるサブプロ
ジェクトで構成されています。

ビルド時に`フラグを 1 つ追加するだけ
<../build/index.html#main-parameters>`_で、Ursa ライブラリを
使って Iroha を簡単にビルドできます。これにより、標準の Iroha
暗号化の代わりに Ursa ライブラリの暗号化アルゴリズムを使用
できます。Ursa で新しいライブラリが開発されるにつれて、ます
ます多くのオプションが利用可能になります!

.. note::
	現在、Ursa からは ed25519 SHA-2 アルゴリズムのみを取得して
	います。よろしければ、より多くのオプションを追加するために
	コードに貢献してください。

デフォルトの ed25519/sha3 暗号化アルゴリズムと Ursa のアルゴリ
ズムの両方を使用できるようにするため、後者には Multihash 公開
鍵形式を使用しています。`キー
<../develop/keys.html>`_ について詳しく学べます。

.. _hyperledger-explorer:

Hyperledger Explorer — EOL 2022 年 5 月 12 日
---------------------------------------------

`Hyperledger Explorer
<https://wiki.hyperledger.org/display/explorer>`_ はブロック
チェーンモジュールであり、The Linux Foundation がホストする
Hyperledger プロジェクトの 1 つです。ユーザーフレンドリーな Web
アプリケーションを作成するように設計された Hyperledger Explorer
は、ブロック、トランザクション、および関連データ、ネットワーク
情報（名前、ステータス、ノードリスト）、チェーンコード、トラン
ザクションファミリー、および台帳に保存されているその他の関連
情報を表示、呼び出し、デプロイ、クエリできます。

`こちら
<https://github.com/turuslan/blockchain-explorer/blob/iroha-explorer-integration/iroha-explorer-integration.md>`_
で、Explorer を Iroha と一緒に使用する方法を学べます。

.. _hyperledger-burrow:

Hyperledger Burrow — EOL 2022 年 5 月 12 日
-------------------------------------------

`Hyperledger Burrow
<https://wiki.hyperledger.org/display/burrow>`_ は、Ethereum
Virtual Machine (EVM) の仕様に部分的に準拠して開発された許可
型スマートコントラクトインタプリタを備えたモジュラー型
ブロックチェーンクライアントを提供します。

つまり、HL Burrow を使えば Iroha 上で Solidity スマートコント
ラクトを使用できます。詳細は下をクリックしてください。

.. toctree::
    :maxdepth: 2

    burrow.rst
    burrow_example.rst

.. _hyperledger-quilt:

Hyperledger Quilt — EOL 2022 年 6 月 2 日
-----------------------------------------

`Hyperledger Quilt
<https://lf-hyperledger.atlassian.net/wiki/spaces/INTERN/pages/21954679/HL+Iroha+and+HL+Quilt+Integration>`_
は、共通のトランザクションおよびクエリモデルを通じて Iroha が
他の台帳と相互運用できるようにした相互運用フレームワークで
した。プロジェクトは 2021 年 6 月 23 日に Dormant 状態に移行し、
2022 年 6 月 2 日に End of Life に達しました。リポジトリはアー
カイブされています。リンク先の wiki ページには、元の Iroha–Quilt
統合作業が文書化されています。
