# Hyperledger Iroha C++ library


**Current version of the library was tested and compatible with Iroha 1.5.0.**

The library was created to provide a convenient interface for C++ applications to communicate with [Iroha](https://github.com/hyperledger/iroha) blockchain. This includes sending transactions and queries, streaming transaction statuses and block commits.


# iroha-lib

Client library of [Iroha](https://github.com/hyperledger/iroha) written completely in modern C++.
Currently, the latest HL Iroha 1.5 release (`hyperledger/iroha:1.5.0` Docker image) is supported.


## Installation

Follow these steps to run the project:

1. Set up and run Iroha peer in a Docker container. [Follow the instructions from Iroha documentation](https://iroha.readthedocs.io/en/main/getting_started/).

2. Clone this repository.

3. Build the project:

``` bash
cmake --build ./build --target iroha_lib_model
```

4. Go to the `examples` directory:

``` bash
cd examples
```

5. Run the selected example:

``` bash
./tx_example
```

6. Check the logs to see if the scenario completed successfully.


## Examples

The `examples` directory contains ready-to-run programs that demonstrate the main features of `iroha-lib`. Each example is a single `.cpp` file; shared helpers live in the header-only `common.hpp` (environment configuration, status printing, transaction helpers) and `scenario.hpp` (a reusable domain/asset/account scenario), plus `error_codes.hpp` (human-readable mapping of Iroha error codes, ported from `iroha-python`'s `errorCodes2Hr.py`).

| Example | What it demonstrates | Python counterpart |
|---|---|---|
| `TxExample` | Full transaction workflow: `CreateDomain`, `CreateAsset`, `AddAssetQuantity`, `CreateAccount`, `TransferAsset`, `GrantPermission`, `SetAccountDetail`, and the `GetAssetInfo` / `GetAccountAssets` / `GetAccountDetail` queries | `tx-example.py` |
| `BatchExample` | Atomic exchange batch (MST batching, **not** multisig): two quorum-2 accounts swap assets atomically, including accept and decline flows via pending transactions | `batch-example.py` |
| `MstExample` | Multi-signature transaction: a `group` account with three signatories and quorum 2; transfers stay pending until a second signatory countersigns | `mst-example.py` |
| `QueryExample` | Transaction queries with time and height filters (`first_tx_time`, `last_tx_time`, `first_tx_height`, `last_tx_height`) | `query-transactions.py` |
| `TxOrderingExample` | Transaction pagination with `ordering` (ascending / descending) | `tx-ordering.py` |
| `BlocksQueryExample` | Signed `BlocksQuery` over the `FetchCommits` server stream: observe blocks as transactions are committed | `blocks-query.py` |
| `InfiniteBlocksStreamExample` | Long-running block stream; prints every new block until interrupted (Ctrl+C) | `infinite-blocks-stream.py` |
| `DomainAssetCreation` | Older standalone example (uses gflags) | — |

### Configuration

All ported examples read connection settings from environment variables, with defaults matching the `iroha-python` development setup:

| Variable | Default | Description |
|---|---|---|
| `IROHA_HOST_ADDR` | `127.0.0.1` | Iroha node address |
| `IROHA_PORT` | `50051` | Torii (gRPC) port |
| `ADMIN_ACCOUNT_ID` | `admin@test` | Account used to sign transactions/queries |
| `ADMIN_PRIVATE_KEY` | `f101537e…cab70` | Admin private key as hex (no key file needed) |

Private keys are accepted directly as hex strings via `Utils::generateKeypairFromPrivateKey`, so no key files are required.

### Transaction status stream

`GrpcClient::sendStatusStream(tx_hash)` returns a `TxStatusStream` — a wrapper over the `StatusStream` server-streaming RPC. Read `ToriiResponse` statuses until the stream closes (the node closes it on a final status: committed, rejected, or stateless failure):

``` c++
auto stream = client.sendStatusStream(tx_hash);
ToriiResponse status;
while (stream->Read(&status)) {
    // STATELESS_VALIDATION_SUCCESS -> ENOUGH_SIGNATURES_COLLECTED ->
    // MST_PENDING? -> STATEFUL_VALIDATION_SUCCESS -> COMMITTED
}
```

### Blocks stream

`GrpcClient::sendBlocksStreamQuery(blocks_query)` returns a `BlocksStream` over the `FetchCommits` RPC. Build the query with `iroha_lib::BlocksQuery(account_id, keypair).signAndAddSignature()`:

``` c++
auto stream = client.sendBlocksStreamQuery(blocks_query);
BlockQueryResponse response;
while (stream->Read(&response)) {
    // response.block_response().block() or block_error_response()
}
stream->Cancel();  // stop the stream
```

### Pending transactions and MST

`Query::getPendingTransactions(account_id)` fetches transactions waiting for more signatures (requires `mst_support` in the node config). To countersign, fetch the pending transaction, clear its signatures (`tx.mutable_signatures()->Clear()`), sign it with another signatory's key via `Utils::addSignature`, and send it again — see `BatchExample.cpp` and `MstExample.cpp`.

`TxBatch::batch(transactions, atomic)` attaches shared `BatchMeta` (type + SHA3-256 reduced hashes of every transaction's reduced payload) to each transaction; call it **before** signing, because the meta is part of the signed payload.

### Key files

`model/utils/key_files.hpp` wraps `KeysManagerImpl` so applications don't need the legacy key manager directly:

- `createKeyFiles(name, pass_phrase, keys_directory, log)` — generate a keypair and store it as `<keys_directory>/<name>.pub` / `<name>.priv` (existing files are overwritten, like `createKeys`)
- `loadKeyFiles(name, pass_phrase, keys_directory, log)` — load an existing keypair from those files

An empty `pass_phrase` means unencrypted keys; an empty `keys_directory` means the current directory.

### Genesis block generation

`model/utils/genesis.hpp` replicates `iroha-cli --genesis_block` / `--peers_address`:

- `readPeersAddresses(path)` — read whitespace-separated `ip:port` peer addresses from a file
- `generateGenesisBlock(config, log)` — build a signed genesis block (height 1) registering each peer, creating `admin`/`user`/`money_creator` roles, the `test` domain, the `coin` asset and the `admin@test`/`test@test` accounts. Fresh key files for peers (`node0..nodeN`) and accounts are generated in `config.keys_directory`
- `generateGenesisBlockFromTransactions(transactions)` — wrap already-signed transactions into a genesis block (the `--genesis_transaction` equivalent)

`iroha-cli` uses both helpers for its `--genesis_block`, `--peers_address`, `--new_account` and `--key_path` modes.

### Integration tests

The examples are covered by `test/integration/iroha_lib_examples` (`iroha_lib_examples_test`), which spins up a real single-node Iroha instance via `IntegrationTestFramework` (PostgreSQL backend, `mst_support` enabled, genesis replicating the `admin@test` / `test` domain environment) and runs each example binary against it. Run it with:

``` bash
cd build
ctest -R iroha_lib_examples_test --output-on-failure
```


## GrpcClient class

With GrpcClient you can create a connection with Iroha. Use one of the `send()` methods to do this.


### Create transaction

To create a transaction, you can call a command from a list of commands or create your own from scratch.

``` c++
iroha_lib::Tx(
	account_name,
	keypair)
	.createDomain(
		domain_id,
		user_default_role)
	.createAsset(
		asset_name,
		domain_id,
	    0)
.signAndAddSignature();
```


### Create batch

You can send transactions in batches. To create a batch, you need a list of defined transactions. The batch will only work if all the transactions in it pass validation. If at least one transaction doesn't pass validation, the whole batch is rejected.

Below is an example of creating a batch for two transactions:

``` c++
iroha_lib::TxBatch tx_batch;

std::vector<iroha::protocol::Transaction> transactions({tx_a, tx_b});

iroha_lib::GrpcClient(
	peer_ip,
	torii_port)
	.send(
		tx_batch
	.batch(transactions));
```


## Commands

- [x] [addAssetQuantity](https://iroha.readthedocs.io/en/main/develop/api/commands.html#add-asset-quantity)
- [x] [addPeer](https://iroha.readthedocs.io/en/main/develop/api/commands.html#add-peer)
- [x] [addSignatory](https://iroha.readthedocs.io/en/main/develop/api/commands.html#add-signatory)
- [x] [appendRole](https://iroha.readthedocs.io/en/main/develop/api/commands.html#append-role)
- [x] [createAccount](https://iroha.readthedocs.io/en/main/develop/api/commands.html#create-account)
- [x] [createAsset](https://iroha.readthedocs.io/en/main/develop/api/commands.html#create-asset)
- [x] [createDomain](https://iroha.readthedocs.io/en/main/develop/api/commands.html#create-domain)
- [x] [createRole](https://iroha.readthedocs.io/en/main/develop/api/commands.html#create-role)
- [x] [detachRole](https://iroha.readthedocs.io/en/main/develop/api/commands.html#detach-role)
- [x] [grantPermission](https://iroha.readthedocs.io/en/main/develop/api/commands.html#grant-permission)
- [x] [removeSignatory](https://iroha.readthedocs.io/en/main/develop/api/commands.html#remove-signatory)
- [x] [revokePermission](https://iroha.readthedocs.io/en/main/develop/api/commands.html#revoke-permission)
- [x] [setAccountDetail](https://iroha.readthedocs.io/en/main/develop/api/commands.html#set-account-detail)
- [x] [setAccountQuorum](https://iroha.readthedocs.io/en/main/develop/api/commands.html#set-account-quorum)
- [x] [subtractAssetQuantity](https://iroha.readthedocs.io/en/main/develop/api/commands.html#subtract-asset-quantity)
- [x] [transferAsset](https://iroha.readthedocs.io/en/main/develop/api/commands.html#transfer-asset)
- [x] [сompareAndSetAccountDetail](https://iroha.readthedocs.io/en/main/develop/api/commands.html#compare-and-set-account-detail)
- [x] [removePeer](https://iroha.readthedocs.io/en/main/develop/api/commands.html#remove-peer)


## Queries

- [x] [getAccount](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-account)
- [x] [getAccountAssetTransactions](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-account-asset-transactions)
- [x] [getAccountAssets](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-account-assets)
- [x] [getAccountDetail](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-account-detail)
- [x] [getAccountTransactions](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-account-transactions)
- [x] [getPendingTransactions](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-pending-transactions)
- [x] [getTransactions](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-transactions)
- [x] [getSignatories](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-signatories)
- [x] [getAssetInfo](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-asset-info)
- [x] [getRoles](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-roles)
- [x] [getRolePermissions](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-role-permissions)
- [x] [getPeers](https://iroha.readthedocs.io/en/main/develop/api/queries.html#get-peers)


## Compatibility and release policy

The `develop` branch is compatible with tagged releases of Iroha.
