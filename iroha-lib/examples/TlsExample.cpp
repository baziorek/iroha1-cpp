// Demonstrates TLS-encrypted transport in iroha-lib.
//
// The node must be started with torii_tls_params enabled (server
// certificate and key). The client connects over TLS when
// IROHA_TLS_ROOT_CERT points to a PEM file with the root certificate
// used to verify the node (in Iroha's Torii setup this is typically
// the node's own self-signed certificate).
//
// Environment variables:
//   IROHA_HOST_ADDR        node address (default 127.0.0.1)
//   IROHA_PORT             torii TLS port (no default - must be set)
//   ADMIN_ACCOUNT_ID       account used to sign the query
//   ADMIN_PRIVATE_KEY      hex private key of that account
//   IROHA_TLS_ROOT_CERT    path to PEM root certificate (required)

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "common.hpp"
#include "model/Query.hpp"
#include "model/utils/Utils.h"


namespace {

using namespace iroha_example;

void printAccount(const QueryResponse& response,
                  const std::string& account_id)
{
    if (response.has_error_response()) {
        throw std::runtime_error(
                    "GetAccount error: "
                    + response.error_response().message());
    }
    if (not response.has_account_response()) {
        throw std::runtime_error("Unexpected response type");
    }
    const auto& account = response.account_response().account();
    std::cout << "account_id = " << account.account_id()
              << ", quorum = " << account.quorum()
              << ", json_data = " << account.json_data() << std::endl;
    if (account.account_id() != account_id) {
        throw std::runtime_error(
                    "Unexpected account in response: "
                    + account.account_id());
    }
}

}  // namespace


int main()
{
    try {
        const auto config = loadConfig();
        if (not config.tlsEnabled()) {
            throw std::runtime_error(
                        "IROHA_TLS_ROOT_CERT must point to the node's "
                        "TLS root certificate for this example");
        }

        auto client = makeClient(config);
        std::cout << "connected to " << config.address()
                  << " over TLS (root cert: "
                  << config.tls_root_cert_path << ")" << std::endl;

        // signed query over the encrypted channel
        const auto keypair = generateKeypairFromPrivateKey(
                    config.admin_private_key);
        const auto query = iroha_lib::Query(
                    keypair, nextQueryCounter(), config.admin_account_id)
                .getAccount(config.admin_account_id)
                .signAndAddSignature();
        printAccount(client->send(query), config.admin_account_id);

        // command service over the same TLS channel
        ToriiResponse status;
        const auto status_status = client->getTxStatus(
                    "0000000000000000000000000000000000000000000000000000000000000000",
                    &status);
        if (not status_status.ok()) {
            throw std::runtime_error(
                        "GetTxStatus failed: "
                        + status_status.error_message());
        }
        std::cout << "tx status for unknown hash: "
                  << TxStatus_Name(status.tx_status()) << std::endl;

        std::cout << "TLS example finished successfully" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "TLS example failed: " << e.what() << std::endl;
        return 1;
    }
}
