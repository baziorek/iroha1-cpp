# ようこそ！

**言語:** [English](../../../README.md) · [Polski](../pl/README.md) · [Русский](../ru/README.md) · **日本語** · [Türkçe](../tr/README.md)

## このリポジトリについて

これは **Hyperledger Iroha 1**（C++ 実装）のコミュニティフォークです。
アップストリームの開発は実質停止しており、最後の公開コミットは
**2024年5月**にさかのぼりますが、Iroha 1 は現在もカンボジア国立銀行
（Bakong 決済システム）を含む複数のイニシアチブで本番環境で
活発に利用されています。

このフォークの目標は、Iroha 1 を最新のツールチェーンでビルド・
利用・保守可能な状態に保ち、プロジェクトに健全な第二の人生を
もたらすことです。

### このフォークとアップストリームの違い

最後のアップストリームコミット（`118fced3f3`、2024年5月）と比較して、
このフォークは以下を追加しています：

**ビルドのモダナイゼーション**

- C++ 標準を **C++23** に引き上げ。**GCC 14 / Ubuntu 24.04** で
  ビルドを検証済み
- **vcpkg** ベースラインを 2021 年から 2026 年 11 月に更新
  （protobuf、gRPC、Boost、OpenSSL 3.x、soci 4.x）
- 標準ライブラリで十分な箇所のレガシー依存を置き換え
  （`fmt` → `std::format`、`boost::format`、`boost::variant`、
  `boost::filesystem`、および未使用の Boost ライブラリ数点を削除）
- 警告ゼロのビルド（約 70 件の警告を解消）

**新しいコンポーネント**

- **`iroha-lib/`** — Iroha 1 向けのスタンドアロン C++ クライアント
  ライブラリ（トランザクション、クエリ、ページネーション、
  ストリーミング、TLS トランスポート、鍵/genesis/ピアリストの
  ユーティリティ）。`iroha-python` から移植した 9 つの例付き。
  `iroha-lib/README.md` と `docs/source/develop/libraries/cpp.rst`
  を参照。
- **`iroha-cli/`** — 古典的なインタラクティブ/非インタラクティブ
  CLI。`iroha-lib` のトランスポートとモデル構築を使用するよう移植し、
  `--tls_root_cert` をサポート。
- **`iroha-python`** — Python クライアントライブラリは別リポジトリに
  あります: [baziorek/iroha1-python](https://github.com/baziorek/iroha1-python)
  （オリジナルソース、依存関係を更新済み）。

**サーバーの修正**

- すべてのサーバーに gRPC **キープアライブ** を追加 — NAT/LB の背後で
  接続が切断され、長寿命ストリームがおよそ 24 時間ごとに失敗して
  いた問題を修正（アップストリーム issue hyperledger/iroha#1351）
- `FetchCommits` のハング修正、`GetPendingTransactions` の oneof 修正、
  `creator_account_id` の修正 — すべて新しい統合テストで発見

**リリースと CI**

- **CPack** による DEB/RPM パッケージ + AppImage アーティファクト。
  バージョンタグでの **GitHub Releases**（`1.6.1-reanimation` は
  アップストリームのバージョン系列を継続。パッケージバージョンは
  `git describe` から導出）
- モダナイズされた GitHub Actions: Node 24 ランナー、アクションを
  コミット SHA にピン留め、pip 依存をハッシュロック、共有 vcpkg
  依存キャッシュジョブ
- **OpenSSF Scorecard** + **SECURITY.md** + Dependabot。
  `iroha1-main` にブランチ保護
- 負荷テストスタックを `locustio` から `locust` 2.x に移行

**ドキュメント**

- Sphinx ドキュメントを再構築し **GitHub Pages** に公開:
  <https://baziorek.github.io/iroha1-cpp/>
- **ポーランド語、ロシア語、日本語、トルコ語**への完全な翻訳 —
  この README を含む（上部の言語リンクを参照）

**削除されたデッドコード**

- `snap/` パッケージング（現代の依存関係ではビルド不可）
- `housekeeping/` スクリプト（機能しない Soramitsu CI）、`utils/`
  スクリプト（代替済み。genesis パースは Python フォークへ移動）
- 古い vcpkg パッチファイルと Iroha 2 の CI ワークフロー

> **免責事項:** これらのコミュニティフォークは、AI の支援を受けた
> 一人のメンテナーによって維持されています。保守、ビルドシステム、
> 依存関係の更新を含みますが、Iroha の機能変更は含みません。ただし
> **プロダクションまたは商用利用の保証はありません**。Iroha 1 自体は
> 実績のあるプロジェクトで、実際のデプロイがあります（例：
> カンボジア国立銀行による Bakong 決済システム）。

この README の残りの部分はアップストリームプロジェクトを説明する
もので、そのまま有効です。

## Hyperledger Iroha とは？

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/960/badge)](https://bestpractices.coreinfrastructure.org/projects/960)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/baziorek/iroha1-cpp/badge.svg?branch=iroha1-main)](https://scorecard.dev/viewer/?uri=github.com/baziorek/iroha1-cpp)

Iroha は、日本のカイゼンの原則 — 過剰さ（muri）の排除 — に触発された、
シンプルな分散型台帳技術（DLT）です。Iroha は、資産・情報・
アイデンティティ管理に必要な本質的な機能を備え、同時に企業ニーズに
応える効率的で信頼できるクラッシュフォールトトレラントなツールです。

[ドキュメント](https://baziorek.github.io/iroha1-cpp/)の
[概要](https://baziorek.github.io/iroha1-cpp/)ページをご覧ください。
会議や解説動画をアップロードしている
[YouTube チャンネル](https://www.youtube.com/channel/UCYlK9OrZo9hvNYFuf0vrwww)
もあります — チェックしてみてください！

<img height="300px" src="../../../docs/image_assets/Iroha_3_sm.png"
 alt="Iroha ロゴ" title="Iroha" align="right" />

Iroha には以下の機能があります：
1. 通貨、金のキログラムなど、カスタムの代替可能資産の作成と管理
2. ユーザーアカウントの管理
3. システム内の _ドメイン_ に基づくアカウントの分類
4. システム内でのトランザクションとクエリの実行に対するユーザー権限の
   仕組みと検証
5. システム内のトランザクションとクエリに対するビジネスルールの検証
6. マルチシグネチャトランザクション

Iroha は _クラッシュフォールトトレラント_ であり、独自のコンセンサス
アルゴリズム [YAC](https://arxiv.org/pdf/1809.00554.pdf) を持ちます。
Iroha 2 では、新しいアルゴリズム [Sumeragi](https://iroha.tech/#sumeragi)
に置き換えられています。

## ドキュメント

このフォークのドキュメントは Sphinx でビルドされ、**GitHub Pages** に
公開されています:
<https://baziorek.github.io/iroha1-cpp/> — 英語に加え、ポーランド語、
ロシア語、日本語、トルコ語でも利用できます（ドキュメントのサイドバーと
この README の言語スイッチャーを参照）。

#### ドキュメントの翻訳があります！

このフォークには **ポーランド語、ロシア語、日本語、トルコ語**への
完全な翻訳が含まれており、`docs/i18n/` で管理されています
（アップストリームの
[ローカライズリポジトリ](https://github.com/hyperledger/iroha-docs-l10n)
は古いスナップショットをカバーしています — プルリクエストであなたの
言語への翻訳をお気軽にどうぞ）。

### Iroha をすばやく体験するには？

[使い始める](https://baziorek.github.io/iroha1-cpp/getting_started/index.html)
ドキュメントのセクションをご覧ください。
古い Katacoda サンドボックスは終了しましたが、
[統合テスト](../../../test/)と
[`iroha-lib` の例](../../../iroha-lib/)がライブノードの動作を示しています。

### Iroha をビルドするには？

[ビルドガイド](https://baziorek.github.io/iroha1-cpp/build/index.html)
をご利用ください。コードを変更して貢献したい場合に役立ちます。

### SDK はありますか？

はい — このフォーク向けには：このリポジトリの [`iroha-lib/`](../../../iroha-lib/)
にあるモダンな **C++ クライアントライブラリ**と、メンテナンスされている
**Python** クライアント
[baziorek/iroha1-python](https://github.com/baziorek/iroha1-python) です。
アップストリームには
[Java](https://github.com/hyperledger/iroha-java)、
[Python](https://github.com/hyperledger/iroha-python)、
[JavaScript](https://github.com/hyperledger/iroha-javascript)、
[iOS](https://github.com/hyperledger/iroha-ios) の SDK もあります
（メンテナンスされていませんが、参考用に保持）。

### サンプルアプリケーションはありますか？

[Android ポイントアプリ](https://github.com/hyperledger/iroha-android/tree/master/iroha-android-sample)
と [JavaScript ウォレット](https://github.com/soramitsu/iroha-wallet-js)。

[The Borsello App](https://github.com/claudiocandio/borsello) も
お忘れなく — Android と iOS のウォレットに加え、Web ブラウザ
アプリケーションを備えた、[Claudio](https://github.com/claudiocandio)
による貢献です。JS Wallet ベースですが最新です。

### Iroha の素晴らしいツール

[Iroha Explorer](https://codeberg.org/diva.exchange/iroha-explorer)
[Postgres 10 付き Iroha Docker コンテナ](https://hub.docker.com/r/divax/iroha)
とその[ソースコード](https://codeberg.org/diva.exchange/iroha)
[Iroha インスタンスをデプロイするツール](https://github.com/kuvaldini/iroha-swarm)

### Iroha の開発を手伝いたいですか？

素晴らしいです！
[CONTRIBUTING.rst](../../../CONTRIBUTING.rst) をご覧ください —
このリポジトリの
[GitHub Issues](https://github.com/baziorek/iroha1-cpp/issues)
で問題や改善提案を歓迎します。

## お困りですか？

* [Telegram チャット](https://t.me/hyperledgeriroha) または
[Hyperledger RocketChat](https://chat.hyperledger.org/channel/iroha)
に参加してください。メンテナー、コントリビューター、ユーザーが
喜んでお手伝いします。そこで、あるいは Gitter で Iroha について
議論したり、ただおしゃべりしたりできます
[![https://gitter.im/hyperledger-iroha/Lobby のチャットに参加](https://badges.gitter.im/hyperledger-iroha/Lobby.svg)](https://gitter.im/hyperledger-iroha/Lobby)
* 問題や改善提案は
[GitHub Issues](https://github.com/baziorek/iroha1-cpp/issues) から
お寄せください — アップストリームの Iroha 2 に関する質問は
[Hyperledger Jira](https://jira.hyperledger.org/secure/CreateIssue!default.jspa)
をご覧ください
* [メーリングリスト](https://lists.hyperledger.org/g/iroha) に
登録して、最新かつ最も重要なニュースを受け取り、Iroha コミュニティで
あなたの声を広めてください

## ライセンス

Iroha のコードベースは Apache License,
Version 2.0（「ライセンス」）でライセンスされています。このファイルを
ライセンスに従って以外の方法で使用することはできません。ライセンスの
コピーは http://www.apache.org/licenses/LICENSE-2.0 で入手できます

適用される法律で要求されるか、書面で合意された場合を除き、
ライセンスに基づいて配布されるソフトウェアは、「現状有姿」で
配布され、明示または黙示を問わず、いかなる種類の保証も
条件も伴いません。特定の言語における権利と制限については
ライセンスをご覧ください。

Iroha のドキュメントファイルは Creative Commons
Attribution 4.0 International License（CC-BY-4.0）で提供されます。
http://creativecommons.org/licenses/by/4.0/ で入手できます
