.. _deploy-guide:

======
Deploy
======

Hyperledger Iroha, bakış açısına ve amacına bağlı olarak farklı
şekillerde dağıtılabilir. Tek bir düğüm olabilir ya da yerel
makinede birden fazla konteynerde çalışan ya da ağ genelinde
dağıtılmış birden fazla düğüm olabilir — ihtiyacınız olan durumu
seçin. Bu sayfa farklı senaryoları açıklar ve özellikle ilk kez
Iroha deneyen kullanıcılar için bir nasıl-yapılır kılavuzu olmayı
amaçlar.

.. rubric:: Tarihsel not

Eski Iroha 1 sürümleri, `grpc/grpc#20418
<https://github.com/grpc/grpc/issues/20418>`_ sorunundan etkilenen
bir gRPC sürümü içeriyordu; burada eski ``epollex`` polling
motoru dosya tanımlayıcılarını tüketebilir ve macOS ile eski Linux
çekirdeklerinde düğümü çökertabilirdi. Bu poller 2022'de gRPC'den
kaldırıldı, bu nedenle güncel derlemeler (gRPC >= 1.46, event
engine) etkilenmez — sorun yalnızca gRPC güncellemesinden önce
derlenen artefaktları ilgilendirir.

.. toctree::
      :maxdepth: 1

      single.rst
      syncing-node.rst
      flags.rst
      multiple.rst
      k8s-deployment.rst
      troubles.rst
