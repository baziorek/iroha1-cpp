*******************************
Hyperledger Iroha documentation
*******************************

.. image:: ../image_assets/iroha_logo.png

.. warning::
    Please note that Hyperledger Iroha v1 is **no longer under active development**; therefore, support for Iroha v1 is limited. The core development team is focused on `Hyperledger Iroha v2 <https://github.com/hyperledger/iroha/tree/iroha2-dev#hyperledger-iroha>`_, a complete rewrite of Iroha in Rust.
    
    For new projects, **we recommend using Iroha v2 instead of Iroha v1**. If you are using Iroha v1, we encourage you to plan an upgrade and switch to using Iroha v2.
    
    The long-term supported version of Iroha v2 (`iroha2-lts`) is your best choice if you want to use Iroha in production and receive tech support from our side. Unfortunately, we cannot offer the same support for Iroha v1, so please consider using Iroha v2.

    **Note that Iroha v1 and Iroha v2 are incompatible**. You can read about their differences in the `Iroha 2 documentation <https://hyperledger.github.io/iroha-2-docs/guide/iroha-2.html>`_.

Welcome! Hyperledger Iroha 1 is a simple blockchain platform you can use to make trusted, secure, and fast applications by bringing the power of permission-based blockchain with Crash fault-tolerant consensus. It's free, open-source, and works on Linux and Mac OS, with a variety of mobile and desktop libraries.

You can download the source code of Hyperledger Iroha 1 and latest releases from the `iroha1-cpp community fork <https://github.com/baziorek/iroha1-cpp>`_ (the C++ sources also live on the upstream `iroha1-main` branch of `hyperledger-iroha/iroha <https://github.com/hyperledger-iroha/iroha/tree/iroha1-main>`_).

This documentation will guide you through the installation, deployment, and launch of Iroha network, and explain to you how to write an application for it. We will also see which use case scenarios are feasible now, and are going to be implemented in the future.

.. note::
    You are reading the documentation of the `iroha1-cpp <https://github.com/baziorek/iroha1-cpp>`_ community fork, which keeps Iroha v1 buildable and maintained. It is **more up-to-date** than the official ReadTheDocs documentation at `iroha.readthedocs.io <https://iroha.readthedocs.io/>`_, which is no longer updated for Iroha v1.

    The matching Python client is the `iroha1-python <https://github.com/baziorek/iroha1-python>`_ fork — the original `iroha-python <https://github.com/hyperledger-iroha/iroha-python/tree/iroha1-main>`_ sources **unchanged**, rebuilt against modern dependencies so that it installs and works on current systems.

.. note::
    Iroha v1 is a mature, production-proven platform. It powers `Bakong <https://www.nbc.gov.kh/english/economic_research/bakong.php>`_, the national payment system of the National Bank of Cambodia, in production since October 2020 — tens of millions of accounts and over a billion transactions per year, with real-time settlement between banks. It was also used for **Byacco**, a campus digital currency at the University of Aizu (Japan) for the campus store and cafeteria, and for **Moeka**, a local-currency field test in Aizu-Wakamatsu run with the University of Tokyo, the University of Aizu and International University GLOCOM. Bank Central Asia (Indonesia) evaluated it for customer digital identity, and the technology appears in academic and research projects (e.g. drone identification, EU QualiChain credential verification, UK livestock-trading platforms).

.. warning::
    The ``iroha1-cpp`` and ``iroha1-python`` forks contain **no functional changes** to Iroha itself — only build-system updates, dependency refreshes and maintenance cleanups. They are maintained by a single person (with AI-assisted development), **not** by the original development team that designed, tested and operated the system for years. While the changes are intended to be safe, **no production or commercial-use warranty is provided**. If you plan a commercial deployment, evaluate the fork carefully or consider the officially supported `Hyperledger Iroha v2 <https://github.com/hyperledger/iroha>`_ instead.

As Hyperledger Iroha is an open-source project, we will also cover contribution part and explain you a working process.

.. toctree::
    :maxdepth: 2
    :numbered:
    :caption: Table of contents

    overview.rst
    concepts_architecture/index.rst
    getting_started/index.rst
    integrations/index.rst
    build/index.rst
    configure/index.rst
    deploy/index.rst
    maintenance/index.rst
    develop/index.rst
    community/index.rst
    faq/index.rst
