.. _deploy-guide:

======
Deploy
======

Hyperledger Iroha は、目的や視点に応じてさまざまな方法で
デプロイできます。単一のノードの場合もあれば、ローカルマシン上の
複数のコンテナやネットワーク全体に分散した複数のノードの場合も
あります — 必要なケースを選んでください。このページでは異なる
シナリオを説明し、主に初めて Iroha を試すユーザー向けの
ハウツーガイドとして機能します。

.. rubric:: 歴史的な注意

古い Iroha 1 リリースには、`grpc/grpc#20418
<https://github.com/grpc/grpc/issues/20418>`_ の影響を受ける gRPC
バージョンが同梱されていました。この問題では、レガシーの
``epollex`` ポーリングエンジンがファイルディスクリプタを枯渇させ、
macOS や古い Linux カーネルでノードをクラッシュさせる可能性が
ありました。この poller は 2022 年に gRPC から削除されたため、
現在のビルド（gRPC >= 1.46、イベントエンジン）は影響を受けませ
ん — この問題は gRPC 更新前にビルドされたアーティファクトのみに
関係します。

.. toctree::
      :maxdepth: 1

      single.rst
      syncing-node.rst
      flags.rst
      multiple.rst
      k8s-deployment.rst
      troubles.rst
