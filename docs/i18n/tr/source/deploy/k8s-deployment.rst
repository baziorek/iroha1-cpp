=====================================
Kubernetes Kümesinde Iroha Dağıtımı
=====================================

.. warning:: **Bu kılavuz tarihseldir ve takip edilemez.**

  Upstream deposunun ``deploy/`` dizinini kullanarak Terraform,
  Ansible ve Kubespray ile Iroha 1'i Kubernetes'te dağıtmayı
  anlatır. Bu dizin — Terraform modülleri, Ansible
  playbook'ları, Kubespray alt modülü, k8s manifestleri —
  **artık Iroha 1 kaynaklarının parçası değildir** ve
  bakımı yapılmamaktadır. Orijinal materyaller upstream depo
  geçmişinde korunmuştur; bkz.
  `hyperledger-iroha/iroha <https://github.com/hyperledger-iroha/iroha>`__
  (``iroha1-main`` dalı ve ``v1.5.0`` gibi önceki etiketler).

Bu kılavuzun eskiden kapsadıkları
---------------------------------

- Terraform + Kubespray ile AWS'te Kubernetes kümesi
  sağlama,
- ``deploy/ansible/`` altındaki Ansible playbook'larıyla
  Iroha peer yapılandırmalarını ve anahtarlarını üretme,
- kümede çoklu-peer Iroha ağı dağıtımı.

Bugün Kubernetes'te Iroha gerekiyorsa pratik seçenekler:

- ``irohad``'ı (veya bir topluluk Docker/Podman imajını)
  block store ve WSV için kalıcı birimlerle StatefulSet
  olarak çalıştırın, kendi TLS sonlandıran ingress'inizin
  arkasında — bkz. :ref:`deploy-guide` ve
  :doc:`../configure/torii-tls`;
- tek düğümlü dağıtım için yayınlanmış ``.deb``/``.rpm``
  paketleri veya AppImage sürümleri daha basittir — bkz.
  :ref:`getting-started`.
