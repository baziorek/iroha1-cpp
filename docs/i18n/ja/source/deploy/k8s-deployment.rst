=====================================
Kubernetes
=====================================

.. warning:: **このガイドは歴史的なものであり、実行できません。**

  アップストリームリポジトリの ``deploy/`` ディレクトリを
  使って、Terraform、Ansible、Kubespray で Kubernetes 上に
  Iroha 1 をデプロイする方法を説明しています。その
  ディレクトリ — Terraform モジュール、Ansible
  プレイブック、Kubespray サブモジュール、k8s マニフェスト
  — は **Iroha 1 のソースの一部ではなくなり**、
  メンテナンスされていません。元の資料はアップストリーム
  リポジトリの履歴に保存されています。参照:
  `hyperledger-iroha/iroha <https://github.com/hyperledger-iroha/iroha>`__
  (ブランチ ``iroha1-main`` および ``v1.5.0`` などの
  以前のタグ)。

このガイドが以前カバーしていた内容
----------------------------------

- Terraform + Kubespray を使った AWS 上の Kubernetes
  クラスターのプロビジョニング、
- ``deploy/ansible/`` 配下の Ansible プレイブックによる
  Iroha ピア設定と鍵の生成、
- クラスター上でのマルチピア Iroha ネットワークの展開。

今日 Kubernetes 上で Iroha が必要な場合、実用的な選択肢:

- ``irohad`` (またはコミュニティ製 Docker/Podman イメージ)
  を StatefulSet として実行し、block store と WSV 用の
  永続ボリュームを持たせ、独自の TLS 終端 ingress の後ろに
  配置 — :ref:`deploy-guide` と :doc:`../configure/torii-tls`
  を参照;
- シングルノードのデプロイには、リリースされた ``.deb``/
  ``.rpm`` パッケージまたは AppImage の方が簡単 —
  :ref:`getting-started` を参照。
