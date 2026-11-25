=====================================
Deploying Iroha on Kubernetes cluster
=====================================

.. warning:: **This guide is historical and cannot be followed.**

  It describes deploying Iroha 1 on Kubernetes with Terraform, Ansible
  and Kubespray using the ``deploy/`` directory of the upstream
  repository. That directory — Terraform modules, Ansible playbooks,
  Kubespray submodule, k8s manifests — is **not part of the Iroha 1
  sources anymore** and is not maintained. The original materials are
  preserved in the upstream repository history; see
  `hyperledger-iroha/iroha <https://github.com/hyperledger-iroha/iroha>`__
  (branch ``iroha1-main`` and earlier tags such as ``v1.5.0``).

What this guide used to cover
-----------------------------

- provisioning a Kubernetes cluster on AWS with Terraform + Kubespray,
- generating Iroha peer configurations and keys with the Ansible
  playbooks under ``deploy/ansible/``,
- rolling out a multi-peer Iroha network on the cluster.

If you need Iroha on Kubernetes today, the practical options are:

- run ``irohad`` (or a community Docker/Podman image) as a StatefulSet
  with persistent volumes for the block store and WSV, behind your own
  TLS-terminating ingress — see :ref:`deploy-guide` and
  :doc:`../configure/torii-tls`;
- for a single-node deployment, the packaged ``.deb``/``.rpm`` or
  AppImage releases are simpler — see :ref:`getting-started`.
