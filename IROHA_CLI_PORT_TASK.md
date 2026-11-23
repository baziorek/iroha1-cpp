# irohacli → iroha-lib port — task description & context dump

> Working document for the ongoing task: rework `iroha-cli` so it is built on
> top of `iroha-lib`, with all CLI-provided functionality also available in
> the library, plus tests. Written 2026-… (branch `iroha1-lib-standalone`).

## Goal

- `iroha-cli` must use `iroha-lib` (Tx/TxBatch/Query/BlocksQuery/GrpcClient/Utils)
  instead of the legacy `iroha::model::*` + `model_generators` +
  `model_converters` + old Torii clients stack.
- Every functionality the CLI provides must be available through the library;
  if something is missing (key generation, sample data, peer lists, genesis
  generation, …) it must be **added to `iroha-lib`** — not only implemented
  inside the CLI.
- Tests for the new library functionality and for the CLI.
- Thorough documentation when done.
- Do **not** push. Keep `iroha-python-fork/` and `todos.txt` untracked.

## Repository state

- Repo: `/home/agh/portable/iroha1-cpp`, branch `iroha1-lib-standalone`
  (do NOT switch to `iroha1-main`). Remote: `git@github.com:baziorek/iroha1-cpp.git`
- Build dir: `build-new` (Ninja, Release, vcpkg toolchain
  `vcpkg-build-new/scripts/buildsystems/vcpkg.cmake`, triplet `x64-linux`).
- Latest commits: `324753455c` (examples+ITF tests), `7fbddf374f` (MST example,
  FetchCommits cancel fix, Query pending/creator fixes, docs).
- `iroha-lib` already has: `Tx` (all commands), `TxBatch` (atomic/MST),
  `Query` (+`creator_account_id`, pagination, ordering, filters,
  `getPendingTransactions`, `getAccountAssetTransactions`), `BlocksQuery`,
  `GrpcClient` (`send`, `sendQuery`?, `getTxStatus`, `sendStatusStream`,
  `sendBlocksStreamQuery`), `Utils` (keypair from hex/private, random keypair,
  hex helpers, `addSignature` on proto Transaction).
- Examples in `iroha-lib/examples/` are the reference for library usage
  (`common.hpp` env config, `scenario.hpp`, `error_codes.hpp`).
- ITF integration test `test/integration/iroha_lib_examples/` runs example
  binaries against real nodes; **all 7 pass locally** with PostgreSQL in
  Docker (`iroha-test-pg`, `postgres:12`, `postgres`/`mysecretpassword`,
  port 5432). In CI it timed out at the 120 s integration limit — fixed by
  `TIMEOUT 900` property in its CMakeLists.

## irohacli inventory (what must keep working)

Flags (`iroha-cli/main.cpp`, gflags):
`--new_account`, `--account_name`, `--pass_phrase`, `--key_path`,
`--peer_ip`, `--torii_port`, `--json_transaction`, `--json_query`,
`--genesis_block`, `--genesis_transaction`, `--peers_address`,
`--interactive` (default true).

Modes:
1. `--new_account` — generate keypair, save to key file (KeysManagerImpl).
2. `--genesis_block` / `--genesis_transaction` — build genesis block/tx
   (old `TransactionGenerator`/`BlockGenerator`), peers from `--peers_address`.
3. `--json_transaction <file>` — parse JSON tx (JsonTransactionFactory) and send.
4. `--json_query <file>` — parse JSON query (JsonQueryFactory) and send.
5. Interactive mode — menu-driven:
   - `tx`: 15 commands — add_ast_qty, add_peer, add_sign, crt_acc, crt_ast,
     crt_dmn, rem_sign, set_qrm, set_acc_kv, sub_ast_qty, tran_ast, crt_role,
     apnd_role, detach, grant_perm, revoke_perm → result menu: send/save/add/back.
   - `qry`: get_acc, get_acc_ast, get_acc_ast_tx, get_acc_tx, get_tx (hashes),
     get_acc_sign, get_roles, get_ast_info, get_role_perm → send/save.
   - `st`: get_tx_info (tx hash) → prints human-readable status.

## Library gaps to fill (planned)

- Sync `getTxStatus` in `GrpcClient` if missing (CLI `st` + send flow need it;
  `send` currently may already return final status — verify).
- Genesis transaction + block construction (proto-level): domain, role,
  admin account, AddPeer per peer, quorum; block height 1, prevHash =
  hash of empty blob, signed by admin key. Reference:
  `pythonLikeGenesisBlock()` in `test/integration/iroha_lib_examples/iroha_lib_examples_test.cpp`.
- Peer-list file parsing for `--peers_address`.
- Key file handling (`--new_account`, `--key_path`, `--pass_phrase`) —
  `libs/crypto/keys_manager_impl` exists; wrap or reuse.
- JSON tx/query (de)serialization — **decision: reuse existing
  `JsonTransactionFactory`/`JsonQueryFactory` from `irohad/model/converters`**
  for file I/O compatibility; do NOT port JSON into iroha-lib.
- Possibly a small "cli support" section in iroha-lib for peer/genesis helpers.

## Known bugs / outdated bits in current irohacli (fix during port)

- `CliClient::sendTx` returns a **fake `TxStatus::OK`** — does not wait for a
  real status (`iroha-cli/client.cpp`). New impl must use real status.
- `REVOKE_PERM` menu entry is bound to `parseGrantPermission` instead of
  `parseRevokePermission` (`interactive/impl/interactive_transaction_cli.cpp`).
- `InteractiveStatusCli::parseSaveFile` is "Not implemented yet".
- Query CLI builds queries via old `iroha::model::generators::QueryGenerator`
  with manual time/counter — replace with `iroha_lib::Query`.

## Suggested architecture

- `iroha-cli` becomes a thin gflags/interactive front-end over `iroha-lib`:
  - `CliClient` → wrapper over `iroha_lib::GrpcClient` (or use it directly).
  - Interactive tx/query parsers call `iroha_lib::Tx`/`Query` builders.
  - Response printing handlers stay in CLI (presentation layer) but consume
    proto `TransactionResponse`/`QueryResponse` from `GrpcClient`.
  - JSON file I/O keeps using existing converters.
  - Menu helpers (`interactive_common_cli.hpp`, parser lib) unchanged.
- CMake: drop legacy deps (model_generators, converters, command_client,
  query_client, keys_manager, old crypto providers); link `iroha_lib` +
  needed infra (gflags, spdlog/logger, parser, json converters, keys_manager).

## Tests to add/update

- Unit tests for new library pieces (genesis builder, peer-list parsing,
  key file helpers, getTxStatus if added).
- Update/replace `test/module/iroha-cli/client_test.cpp`.
- Optional integration test: run `iroha-cli` non-interactive modes
  (`--json_transaction`, `--json_query`, `--new_account`) against ITF node,
  same pattern as `iroha_lib_examples_test`.

## CI notes (fixed in this session)

- Node.js 20 deprecation warnings: all GitHub Actions bumped to node24
  versions in `.github/*.src.yml` + regenerated `workflows/*.yml`
  (checkout@v5, cache@v5, upload-artifact@v6, download-artifact@v7,
  docker/*@v4/v6/v7, paths-filter@v4, setup-python@v6, configure-pages@v6,
  deploy-pages@v5, upload-pages-artifact@v4). Regenerate with
  `./.github/make-workflows.sh --worktree` (requires `yq`).
- Release CTest failed only on `integration_iroha_lib_examples_test`
  (Timeout 120 s) → added `TIMEOUT 900` property.
  `system_irohad_test` timeout is in `.github/TESTS_ALLOWED_TO_FAIL` (flake:
  port 55552 bind conflict).
- Debug job died with exit 137 (runner shutdown/OOM at ~80% build) — infra,
  not a code error.
- Compile warnings: `-Wunused-function randomHexSuffix` in
  `test/framework/integration_framework/iroha_instance.hpp` fixed with
  `[[maybe_unused]]`; other warnings in the CI log are pre-existing in
  shared_model/test code — fix only warnings in files touched by our work.
