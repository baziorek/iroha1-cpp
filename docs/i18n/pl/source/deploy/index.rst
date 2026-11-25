.. _deploy-guide:

======
Deploy
======

Hyperledger Iroha można wdrożyć na różne sposoby, w zależności od
perspektywy i celu. Może to być pojedynczy węzeł albo wiele węzłów
pracujących w kilku kontenerach na maszynie lokalnej lub
rozproszonych w sieci — wybierz wariant, którego potrzebujesz.
Ta strona opisuje różne scenariusze i ma być przewodnikiem
instruktażowym dla użytkowników, głównie próbujących Irohę po raz
pierwszy.

.. rubric:: Uwaga historyczna

Starsze wydania Irohy 1 zawierały wersję gRPC podatną na
`grpc/grpc#20418 <https://github.com/grpc/grpc/issues/20418>`_,
w której legacy silnik pollingu ``epollex`` mógł zagłodzić
deskryptory plików i spowodować awarię węzła na macOS i starszych
kernelach Linuksa. Ten poller został usunięty z gRPC w 2022 roku,
więc obecne buildy (gRPC >= 1.46, event engine) nie są dotknięte —
problem dotyczy tylko artefaktów zbudowanych przed aktualizacją gRPC.

.. toctree::
      :maxdepth: 1

      single.rst
      syncing-node.rst
      flags.rst
      multiple.rst
      k8s-deployment.rst
      troubles.rst
