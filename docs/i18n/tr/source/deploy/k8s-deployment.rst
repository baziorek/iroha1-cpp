=====================================
Iroha'yı Kubernetes kümesine dağıtma
=====================================

.. warning:: **Bu kılavuz tarihseldir ve takip edilemez.**

  Iroha 1'in Kubernetes üzerinde Terraform, Ansible ve Kubespray
  ile dağıtımını anlatır; upstream deposunun ``deploy/`` dizinini
  kullanır. Bu dizin — Terraform modülleri, Ansible playbook'ları,
  Kubespray alt modülü, k8s manifestleri — **artık Iroha 1
  kaynaklarının parçası değildir** ve bakımsızdır. Orijinal
  materyaller upstream deposunun geçmişinde korunmaktadır; bkz.
  `hyperledger-iroha/iroha
  <https://github.com/hyperledger-iroha/iroha>`__ (``iroha1-main``
  dalı ve ``v1.5.0`` gibi önceki etiketler).

Bu kılavuzun eskiden kapsadıkları
----------------------------------------

- Terraform + Kubespray ile AWS üzerinde Kubernetes kümesi
  provizyonu,
- ``deploy/ansible/`` altındaki Ansible playbook'larıyla Iroha eş
  yapılandırmaları ve anahtarlarının üretilmesi,
- kümede çok eşli bir Iroha ağının dağıtılması.

Bugün Kubernetes'te Iroha'ya ihtiyacınız varsa pratik seçenekler:

- ``irohad``'i (veya topluluk Docker/Podman imajını) kalıcı
  birimler (persistent volumes) ile block store ve WSV için
  StatefulSet olarak, kendi TLS sonlandıran ingress'iniz arkasında
  çalıştırın — bkz. :ref:`deploy-guide` ve
  :doc:`../configure/torii-tls`;
- tek düğümlü dağıtım için paketlenmiş ``.deb``/``.rpm`` veya
  AppImage sürümleri daha basittir — bkz. :ref:`getting-started`.
