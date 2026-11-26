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

## CI/CD security posture

This repository intentionally keeps some patterns that automated
security scanners (e.g. OpenSSF Scorecard) flag as "dangerous". They
are documented here so reviewers know they are deliberate:

- **`build-iroha1-fork.yml`** uses `pull_request_target` together with
  a checkout of the pull request's head commit. This is the documented
  GitHub pattern for building pull requests that come from forks: the
  workflow runs with write-scoped `GITHUB_TOKEN` permissions
  (`packages: write` for GHCR image pushes) and access to the
  repository's Docker Hub credentials, so it must run in the trusted
  `pull_request_target` context — where the workflow definition always
  comes from the base branch and **cannot be modified by the pull
  request**.

  Honest risk assessment: this is an accepted-risk pattern, not a
  safe one. Untrusted fork code is compiled inside containers, but
  repository scripts executed on the runner could in principle reach
  CI credentials present in the job environment. We accept this for
  fork-PR builds because the workflow only publishes build artifacts
  (no deployment, no main-branch writes), and the alternative —
  dropping fork CI entirely — was judged worse for a revived
  community project. Pull requests from forks should be treated as
  untrusted code regardless of what CI reports.
- **Write-scoped job permissions** (`contents: write` for the release
  job, `packages: write` for image pushes) are limited to the single
  jobs that need them; every other job runs read-only.

If you believe either pattern can be improved without losing fork-PR
builds or release publishing, please open an issue.

## Reporting a security issue

For issues in **this fork**, please open a GitHub issue on
`baziorek/iroha1-cpp` describing the problem — do not include
sensitive exploit details beyond what is needed to reproduce.

For issues in **mainline Iroha** (the code this fork derives from),
follow the upstream Hyperledger security process linked above.
