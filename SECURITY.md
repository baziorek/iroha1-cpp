# Security Policy

## Supported versions

This repository is a community-maintained fork of Hyperledger Iroha 1
(`iroha1-cpp`). It contains **no functional changes** to Iroha itself —
only build-system updates, dependency upgrades and maintenance work.

| Version | Status |
|---|---|
| `iroha1-main` branch | Community-maintained; builds and basic tests pass, but **no production or commercial-support guarantees** |
| Release tags (`1.6.x`) | Same status as above — packages are published for convenience, not as a supported product |

## Upstream (mainline) Iroha

The original Hyperledger Iroha 1 project is **end-of-life**. The main
Hyperledger development effort has moved to Iroha 2 (a Rust rewrite,
`hyperledger/iroha`). The upstream project's security process —
reporting channel, defect response and supported versions — is
documented at
[hyperledger/iroha SECURITY.md](https://github.com/hyperledger-iroha/iroha/blob/iroha1-main/SECURITY.md)
and applies to the mainline codebase this fork is based on.

Mainline Iroha 1 is a mature, production-proven platform — it has run
national payment infrastructure (e.g. Bakong, the National Bank of
Cambodia's payment system) since 2020. Its security properties were
designed, tested and operated by the original Hyperledger team.

## This fork — security considerations

The fork is maintained by a **single person** (with AI assistance),
not by a team of blockchain and security specialists as in the
mainline project. Consequences:

- There is **no dedicated security contact and no SLA** for this
  repository. Security-relevant issues can be raised through regular
  GitHub issues/PRs on this repository, but response time is not
  guaranteed.
- The fork applies dependency and toolchain updates to keep the code
  buildable on current systems. These changes are intended to be
  safe, but they have **not** undergone the multi-year operational
  validation of the mainline project, and **no production or
  commercial-fitness guarantees are given**.
- If you plan a commercial or production deployment, evaluate the
  fork carefully, or consider the officially supported Hyperledger
  Iroha v2 instead.

## Reporting a security issue

For issues in **this fork**, please open a GitHub issue on
`baziorek/iroha1-cpp` describing the problem — do not include
sensitive exploit details beyond what is needed to reproduce.

For issues in **mainline Iroha** (the code this fork derives from),
follow the upstream Hyperledger security process linked above.
