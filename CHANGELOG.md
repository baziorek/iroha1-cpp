# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to a community-maintained versioning scheme:
releases continue the upstream Iroha 1 version line (`1.6.x`), with
package versions derived from `git describe` (e.g. `1.6.0.29.g<sha>`
for commits after the `1.6.0` tag, `1.6.1` for the tagged release).

## [Unreleased]

### Added

- `iroha-lib`: standalone CMake build for the C++ client library
- `iroha-lib`: 9 C++ examples ported from `iroha-python` (transactions,
  queries, pagination, ordering, blocks, streaming, atomic exchange,
  MST, TLS)
- `iroha-lib`: streaming APIs (`TxStatusStream`, `BlocksStream`),
  `BlocksQuery`/`FetchCommits`, transaction ordering, pending
  transactions
- `iroha-lib`: TLS transport via `TlsOptions` (one-way TLS, optional
  mutual TLS)
- `iroha-lib`: utilities for key files, genesis block generation and
  peer list parsing
- `iroha-cli`: ported to `iroha-lib` transport and model builders;
  `--tls_root_cert` for JSON transaction/query mode
- Unit tests for `iroha-lib` model API (`Tx`, `Query`, `TxBatch`)
- Integration tests for examples, CLI and TLS against a live node

### Changed

- Build modernized for GCC 14 / Ubuntu 24.04, C++23
- vcpkg baseline bumped to November 2026 (protobuf, gRPC, Boost,
  OpenSSL 3.x, soci 4.x)
- CI: GitHub Actions updated (Node 24 runners, `$GITHUB_OUTPUT`,
  disk-space cleanup before CPack, shared vcpkg deps job)

### Removed

- `snap/` packaging (could not build against modern dependencies)
- `housekeeping/` scripts (targeted defunct Soramitsu CI)
- `utils/` scripts (superseded by the `iroha1-python` fork;
  genesis parsing moved there as `iroha.genesis`)

## Notes

This is a community-maintained fork. Iroha 1 itself is a mature
project with real-world deployments; this fork contains build,
dependency and tooling updates but **no production or commercial-use
guarantee** — see the disclaimer in `README.md` and the documentation.
