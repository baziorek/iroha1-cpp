.. _deploy-guide:

======
Deploy
======

Hyperledger Iroha can be deployed in different ways, depending on the perspective and the purpose.
There can be either a single node deployed, or multiple nodes running in several containers on a local machine or spread across the network — so pick any case you need.
This page describes different scenarios and is intended to act as a how-to guide for users, primarily trying out Iroha for the first time.

.. rubric:: Historical note

Older Iroha 1 releases bundled a gRPC version affected by `grpc/grpc#20418 <https://github.com/grpc/grpc/issues/20418>`_, where the legacy ``epollex`` polling engine could starve file descriptors and crash the node on macOS and older Linux kernels. That poller was removed from gRPC in 2022, so current builds (gRPC >= 1.46, event engine) are not affected — the issue only concerns artifacts built before the gRPC update.

.. toctree::
      :maxdepth: 1

      single.rst
      flags.rst
      multiple.rst
      k8s-deployment.rst
      troubles.rst
