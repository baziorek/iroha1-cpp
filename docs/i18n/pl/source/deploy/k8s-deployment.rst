======================================
Wdrażanie Irohy na klastrze Kubernetes
======================================

.. warning:: **Ten przewodnik ma charakter historyczny i nie można go wykonać.**

  Opisuje wdrażanie Irohy 1 na Kubernetes za pomocą Terraform,
  Ansible i Kubespray z katalogu ``deploy/`` repozytorium
  upstream. Ten katalog — moduły Terraform, playbooki Ansible,
  submoduł Kubespray, manifesty k8s — **nie jest już częścią
  źródeł Irohy 1** i nie jest utrzymywany. Oryginalne materiały
  są zachowane w historii repozytorium upstream; zobacz
  `hyperledger-iroha/iroha <https://github.com/hyperledger-iroha/iroha>`__
  (gałąź ``iroha1-main`` i wcześniejsze tagi, np. ``v1.5.0``).

Co ten przewodnik kiedyś obejmował
----------------------------------

- provisionowanie klastra Kubernetes na AWS za pomocą Terraform +
  Kubespray,
- generowanie konfiguracji i kluczy peerów Irohy za pomocą
  playbooków Ansible z ``deploy/ansible/``,
- wdrażanie wielopeerowej sieci Irohy na klastrze.

Jeśli dziś potrzebujesz Irohy na Kubernetes, praktyczne opcje to:

- uruchomienie ``irohad`` (lub community image Docker/Podman)
  jako StatefulSet z wolumenami trwałymi dla block store i WSV,
  za własnym ingress kończącym TLS — zobacz :ref:`deploy-guide`
  i :doc:`../configure/torii-tls`;
- dla wdrożenia jednowęzłowego prostsze są wydane pakiety
  ``.deb``/``.rpm`` lub AppImage — zobacz :ref:`getting-started`.
