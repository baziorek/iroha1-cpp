# Welcome!

**Languages:** English · [Polski](docs/i18n/pl/README.md) · [Русский](docs/i18n/ru/README.md) · [日本語](docs/i18n/ja/README.md) · [Türkçe](docs/i18n/tr/README.md)

## About this repository

This is a community fork of **Hyperledger Iroha 1** (the C++ implementation).
Upstream development has effectively stopped — the last public commit dates
back to **May 2024** — yet Iroha 1 is still intensively used in production
by several initiatives, including the **National Bank of Cambodia**
(Bakong payment system).

The goal of this fork is to keep Iroha 1 buildable, usable and
maintainable with modern toolchains — and to give the project a healthy
second life.

### How this fork differs from upstream

Compared to the last upstream commit (`118fced3f3`, May 2024), this fork
adds:

**Build modernization**

- C++ standard raised to **C++23**; builds verified on **GCC 14 /
  Ubuntu 24.04**
- **vcpkg** baseline bumped from 2021 to November 2026 (protobuf, gRPC,
  Boost, OpenSSL 3.x, soci 4.x)
- Legacy dependencies replaced where the standard library suffices
  (`fmt` → `std::format`, `boost::format`, `boost::variant`,
  `boost::filesystem`, and several unused Boost libraries removed)
- Warning-free build (~70 warnings cleaned up)

**New components**

- **`iroha-lib/`** — a standalone C++ client library for Iroha 1
  (transactions, queries, pagination, streaming, TLS transport,
  key/genesis/peer-list utilities) with 9 examples ported from
  `iroha-python`. See `iroha-lib/README.md` and
  `docs/source/develop/libraries/cpp.rst`.
- **`iroha-cli/`** — the classic interactive/non-interactive CLI,
  ported to use `iroha-lib` for transport and model building, with
  `--tls_root_cert` support.
- **`iroha-python`** — the Python client library lives in a separate
  repository: [baziorek/iroha1-python](https://github.com/baziorek/iroha1-python)
  (original sources, updated dependencies).

**Server fixes**

- gRPC **keepalive** on all servers — fixes dropped connections behind
  NATs/LBs that made long-lived streams fail roughly every 24h
  (upstream issue hyperledger/iroha#1351)
- `FetchCommits` hang fix, `GetPendingTransactions` oneof fix,
  `creator_account_id` fixes — all found by the new integration tests

**Release & CI**

- **CPack** DEB/RPM packages + AppImage artifacts; **GitHub Releases**
  on version tags (`1.6.1-reanimation` continues the upstream version
  line; package versions derive from `git describe`)
- GitHub Actions modernized: Node 24 runners, actions pinned to commit
  SHA, pip dependencies hash-locked, shared vcpkg deps cache job
- **OpenSSF Scorecard** + **SECURITY.md** + Dependabot; branch
  protection on `iroha1-main`
- Load-test stack migrated from `locustio` to `locust` 2.x

**Documentation**

- Sphinx documentation rebuilt and published to **GitHub Pages**:
  <https://baziorek.github.io/iroha1-cpp/>
- Full translations into **Polish, Russian, Japanese and Turkish** —
  including this README (see the language links at the top)

**Removed dead weight**

- `snap/` packaging (unbuildable against modern dependencies)
- `housekeeping/` scripts (defunct Soramitsu CI), `utils/` scripts
  (superseded; genesis parsing moved to the Python fork)
- Stale vcpkg patch files and Iroha 2 CI workflows

> **Disclaimer:** these community forks are maintained by a single
> person with AI assistance. They contain maintenance, build-system and
> dependency updates — not changes to Iroha's functionality — but they
> provide **no production or commercial-use guarantee**. Iroha 1 itself
> is a mature project with real-world deployments (e.g. the Bakong
> payment system operated by the National Bank of Cambodia).

The rest of this README describes the upstream project and remains valid.

## What is Hyperledger Iroha?

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/960/badge)](https://bestpractices.coreinfrastructure.org/projects/960)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/baziorek/iroha1-cpp/badge.svg?branch=iroha1-main)](https://scorecard.dev/viewer/?uri=github.com/baziorek/iroha1-cpp)

Iroha is a straightforward distributed ledger technology (DLT), inspired by Japanese Kaizen principle — eliminate excessiveness (muri).
Iroha has essential functionality for your asset, information and identity management needs, at the same time being an efficient and trustworthy crash fault-tolerant tool for your enterprise needs.

Check the [overview](https://baziorek.github.io/iroha1-cpp/) page of our documentation.
[Here](https://www.youtube.com/channel/UCYlK9OrZo9hvNYFuf0vrwww) is a YouTube channel where we upload meetings and explanatory videos - check them out!

<img height="300px" src="docs/image_assets/Iroha_3_sm.png"
 alt="Iroha logo" title="Iroha" align="right" />

Iroha has the following features:
1. Creation and management of custom fungible assets, such as currencies, kilos of gold, etc.
2. Management of user accounts
3. Taxonomy of accounts based on _domains_ in the system
4. The system of rights and verification of user permissions for the execution of transactions and queries in the system
5. Validation of business rules for transactions and queries in the system
6. Multisignature transactions

Iroha is _Crash Fault Tolerant_ and has its own consensus algorithm — [YAC](https://arxiv.org/pdf/1809.00554.pdf).
In Iroha 2, it is replaced with a new algorithm — [Sumeragi](https://iroha.tech/#sumeragi).

## Documentation

This fork's documentation is built with Sphinx and published to
**GitHub Pages**: <https://baziorek.github.io/iroha1-cpp/> — in English
and in Polish, Russian, Japanese and Turkish (language switcher in the
docs sidebar and in this README).

#### We have documentation translations!

This fork ships full translations into **Polish, Russian, Japanese and
Turkish**, maintained under `docs/i18n/` (upstream's own
[localisations repository](https://github.com/hyperledger/iroha-docs-l10n)
covers older snapshots — feel free to help translate into your local
language via pull requests).

### How to explore Iroha really fast?

Check the [getting started](https://baziorek.github.io/iroha1-cpp/getting_started/index.html)
section of the documentation to start exploring the system.
The old Katacoda sandbox is gone, but the
[integration tests](test/) and the
[`iroha-lib` examples](iroha-lib/) show a live node in action.

### How to build Iroha?

Use the [build guide](https://baziorek.github.io/iroha1-cpp/build/index.html),
which might be helpful if you want to modify the code and contribute.

### Is there SDK available?

Yes — for this fork: a modern **C++ client library** in [`iroha-lib/`](iroha-lib/)
(this repository) and a maintained **Python** client at
[baziorek/iroha1-python](https://github.com/baziorek/iroha1-python).
Upstream also has SDKs in
[Java](https://github.com/hyperledger/iroha-java),
[Python](https://github.com/hyperledger/iroha-python),
[Javascript](https://github.com/hyperledger/iroha-javascript) and
[iOS](https://github.com/hyperledger/iroha-ios) (unmaintained, kept for
reference).

### Are there any example applications?

[Android point app](https://github.com/hyperledger/iroha-android/tree/master/iroha-android-sample) and [JavaScript wallet](https://github.com/soramitsu/iroha-wallet-js).

Also do not forget to check out [The Borsello App](https://github.com/claudiocandio/borsello) – it is a wallet for Android & iOS along with a web browser application contributed by [Claudio](https://github.com/claudiocandio).
It is based on JS Wallet but is up-to-date.

### Great tools for Iroha

[Iroha Explorer](https://codeberg.org/diva.exchange/iroha-explorer)
[Iroha Docker container with Postgres 10](https://hub.docker.com/r/divax/iroha) and it's [source code](https://codeberg.org/diva.exchange/iroha)
[Tool to deploy Iroha instances](https://github.com/kuvaldini/iroha-swarm)

### Want to help us develop Iroha?

That's great!
Check out [CONTRIBUTING.rst](CONTRIBUTING.rst) — issues and improvement
suggestions are welcome via this repository's
[GitHub Issues](https://github.com/baziorek/iroha1-cpp/issues).

## Need help?

* Join [Telegram chat](https://t.me/hyperledgeriroha) or [Hyperledger RocketChat](https://chat.hyperledger.org/channel/iroha) where the maintainers, contributors and fellow users are ready to help you.
You can also discuss your concerns and proposals and simply chat about Iroha there or in Gitter [![Join the chat at https://gitter.im/hyperledger-iroha/Lobby](https://badges.gitter.im/hyperledger-iroha/Lobby.svg)](https://gitter.im/hyperledger-iroha/Lobby)
* Submit issues and improvement suggestions via
[GitHub Issues](https://github.com/baziorek/iroha1-cpp/issues) — for
upstream Iroha 2 questions see the
[Hyperledger Jira](https://jira.hyperledger.org/secure/CreateIssue!default.jspa)
* Subscribe to our [mailing list](https://lists.hyperledger.org/g/iroha) to receive the latest and most important news and spread your word within Iroha community

## License

Iroha codebase is licensed under the Apache License,
Version 2.0 (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the
License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Iroha documentation files are made available under the Creative Commons
Attribution 4.0 International License (CC-BY-4.0), available at
http://creativecommons.org/licenses/by/4.0/
