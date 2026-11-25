********************************
Hyperledger Iroha ドキュメント
********************************

.. image:: ../image_assets/iroha_logo.png

.. warning::
    ご注意: Hyperledger Iroha v1 はもはや** 積極的に開発されて
    いません** 。Iroha v1 のサポートは限定的です。主要な開発チーム
    は `Hyperledger Iroha v2
    <https://github.com/hyperledger/iroha/tree/iroha2-dev#hyperledger-iroha>`_
    — Iroha の Rust による全面的な書き直し — に注力しています。

    新規プロジェクトには **Iroha v2 の使用をお勧めします** 。
    Iroha v1 を使用している場合は、移行を計画し、Iroha v2 への
    移行を検討してください。

    長期サポート版の Iroha v2（`iroha2-lts` ）は、プロダクションで
    Iroha を使用し、こちらから技術サポートを受けたい場合の最良の
    選択です。残念ながら、Iroha v1 には同様のサポートを提供
    できません。Iroha v2 をご検討ください。

    **Iroha v1 と Iroha v2 は互換性がありません** 。両者の違いは
    `Iroha 2 ドキュメント
    <https://hyperledger.github.io/iroha-2-docs/guide/iroha-2.html>`_
    を参照してください。

ようこそ! Hyperledger Iroha 1 は、許可型ブロックチェーンと
クラッシュ故障耐性コンセンサスの力を活用し、信頼性が高く、
安全で高速なアプリケーションを構築できるシンプルなブロック
チェーンプラットフォームです。無料でオープンソースであり、
Linux と macOS で動作し、多様なモバイルおよびデスクトップ
ライブラリを備えています。

Hyperledger Iroha 1 のソースコードと最新リリースはコミュニティ
フォークの `iroha1-cpp
<https://github.com/baziorek/iroha1-cpp>`_ ページで入手でき
ます（` リリース
<https://github.com/baziorek/iroha1-cpp/releases>`_ には
ビルド済みバイナリ、``.deb`` パッケージ、AppImage があり、
C++ ソースは upstream リポジトリ
`hyperledger-iroha/iroha
<https://github.com/hyperledger-iroha/iroha/tree/iroha1-main>`_
の ``iroha1-main`` ブランチにもあります）。

このドキュメントでは、Iroha ネットワークのインストール、
デプロイ、実行方法と、それを使ったアプリケーションの書き方を
説明します。また、現在実現可能なユースケースと計画されている
ものについても見ていきます。

.. note::
    これはコミュニティフォーク `iroha1-cpp
    <https://github.com/baziorek/iroha1-cpp>`_ のドキュメントで
    あり、Iroha v1 をビルド可能な状態で維持しています。これは
    `iroha.readthedocs.io
    <https://iroha.readthedocs.io/>`_ の公式 ReadTheDocs
    ドキュメントよりも** 新しい** です。後者は Iroha v1 向けに
    更新されなくなっています。

    対応する Python クライアントはフォーク `iroha1-python
    <https://github.com/baziorek/iroha1-python>`_ です: 元の
    ``iroha-python`` ソースを** 無変更** のまま、最新の依存関係で
    リビルドしたもので、現在のシステムでインストールして
    動作します。

.. note::
    Iroha v1 はプロダクションで実績のある成熟したプラット
    フォームです。カンボジア国立銀行の国家決済システム
    `Bakong
    <https://www.nbc.gov.kh/english/economic_research/bakong.php>`_
    を2020年10月からプロダクションで運用しており、数千万の
    アカウントと年間10億件以上のトランザクション、銀行間の
    リアルタイム決済を処理しています。この技術は学術・研究
    プロジェクトでも使用されています。

.. warning::
    ``iroha1-cpp`` と ``iroha1-python`` のフォークには、Iroha
    自体の** 機能的変更は含まれていません** — ビルドシステムの
    更新、依存関係の更新、保守作業のみです。これらは元のチーム
    — システムを何年も設計、テスト、運用してきたチーム — では
    なく、1 人のメンテナー（AI の助けを借りて）によって
    メンテナンスされています。変更は安全であるはずですが、
    ** プロダクションおよび商用適性の保証は一切ありません** 。
    商用デプロイを計画している場合は、フォークを慎重に評価する
    か、公式にサポートされる `Hyperledger Iroha v2
    <https://github.com/hyperledger/iroha>`_ を検討してくだ
    さい。

Iroha はオープンソースプロジェクトなので、コードへの貢献方法と
変更の提出プロセスについても説明します。

.. toctree::
    :maxdepth: 2
    :numbered:
    :caption: 目次

    overview.rst
    concepts_architecture/index.rst
    getting_started/index.rst
    integrations/index.rst
    build/index.rst
    configure/index.rst
    deploy/index.rst
    maintenance/index.rst
    develop/index.rst
    community/index.rst
    faq/index.rst
