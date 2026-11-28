#pragma once

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "iroha-lib.hpp"


namespace iroha_example {

using namespace iroha::protocol;
using namespace iroha_lib;

inline std::string envOr(const char* name, const std::string& default_value)
{
    const char* value = std::getenv(name);
    return value != nullptr ? std::string(value) : default_value;
}

struct Config {
    std::string host;
    std::string port;
    std::string admin_account_id;
    std::string admin_private_key;
    /// optional path to PEM root certificate for TLS connections
    std::string tls_root_cert_path;

    std::string address() const { return host + ":" + port; }
    uint16_t torii_port() const { return static_cast<uint16_t>(std::stoul(port)); }
    bool tlsEnabled() const { return not tls_root_cert_path.empty(); }
};

inline Config loadConfig()
{
    return Config{
        envOr("IROHA_HOST_ADDR", "127.0.0.1"),
        envOr("IROHA_PORT", "50051"),
        envOr("ADMIN_ACCOUNT_ID", "admin@test"),
        envOr("ADMIN_PRIVATE_KEY",
              "f101537e319568c765b2cc89698325604991dca57b9716b58016b253506cab70"),
        envOr("IROHA_TLS_ROOT_CERT", "")};
}

inline std::string readFile(const std::string& path)
{
    std::ifstream file(path);
    if (not file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 * Creates a GrpcClient honouring Config::tls_root_cert_path:
 * when the path is set, TLS credentials with the given PEM root
 * certificate are used; otherwise an insecure channel is created.
 */
inline std::unique_ptr<GrpcClient> makeClient(const Config& config)
{
    if (config.tlsEnabled()) {
        TlsOptions tls;
        tls.pem_root_certs = readFile(config.tls_root_cert_path);
        return std::make_unique<GrpcClient>(
                    config.host, config.torii_port(), tls);
    }
    return std::make_unique<GrpcClient>(config.host, config.torii_port());
}

inline std::string commandCaseName(Command::CommandCase command_case)
{
    switch (command_case) {
        case Command::kAddAssetQuantity: return "add_asset_quantity";
        case Command::kAddPeer: return "add_peer";
        case Command::kAddSignatory: return "add_signatory";
        case Command::kAppendRole: return "append_role";
        case Command::kCreateAccount: return "create_account";
        case Command::kCreateAsset: return "create_asset";
        case Command::kCreateDomain: return "create_domain";
        case Command::kCreateRole: return "create_role";
        case Command::kDetachRole: return "detach_role";
        case Command::kGrantPermission: return "grant_permission";
        case Command::kRemoveSignatory: return "remove_signatory";
        case Command::kRevokePermission: return "revoke_permission";
        case Command::kSetAccountDetail: return "set_account_detail";
        case Command::kSetAccountQuorum: return "set_account_quorum";
        case Command::kSubtractAssetQuantity: return "subtract_asset_quantity";
        case Command::kTransferAsset: return "transfer_asset";
        case Command::kRemovePeer: return "remove_peer";
        case Command::kCompareAndSetAccountDetail: return "compare_and_set_account_detail";
        case Command::kSetSettingValue: return "set_setting_value";
        case Command::kCallEngine: return "call_engine";
        default: return "unknown";
    }
}

inline std::vector<std::string> getCommandsFromTx(const Transaction& tx)
{
    std::vector<std::string> commands;
    for (const auto& command : tx.payload().reduced_payload().commands()) {
        commands.emplace_back(commandCaseName(command.command_case()));
    }
    return commands;
}

inline std::string joinCommands(const Transaction& tx)
{
    std::string result;
    for (const auto& command : getCommandsFromTx(tx)) {
        if (not result.empty()) {
            result += ", ";
        }
        result += command;
    }
    return result;
}

inline bool isFailureStatus(TxStatus status)
{
    return status == TxStatus::STATEFUL_VALIDATION_FAILED
        || status == TxStatus::STATELESS_VALIDATION_FAILED
        || status == TxStatus::REJECTED;
}

inline void printTxStatus(const ToriiResponse& status, int index)
{
    std::cout << index << ": status_name=" << TxStatus_Name(status.tx_status())
              << ", status_code=" << status.tx_status()
              << ", err_or_cmd_name=\"" << status.err_or_cmd_name() << "\""
              << ", failed_cmd_index=" << status.failed_cmd_index()
              << ", error_code=" << status.error_code() << std::endl;
}

inline void throwOnFailureStatus(const ToriiResponse& status)
{
    if (isFailureStatus(status.tx_status())) {
        const auto hr = errorCodeToHumanReadable(
                    status.err_or_cmd_name(),
                    status.error_code());
        throw std::runtime_error(
                    TxStatus_Name(status.tx_status())
                    + std::string(" failed, tx hash=") + status.tx_hash()
                    + ", reason=" + std::to_string(status.error_code())
                    + (hr.empty() ? "" : ": " + hr));
    }
}

inline void sendTransactionAndPrintStatus(
        GrpcClient& client,
        const Transaction& tx)
{
    const auto hash = getTransactionHash(tx);
    std::cout << "Transaction \"" << joinCommands(tx) << "\","
              << " hash = " << hash
              << ", creator = " << tx.payload().reduced_payload().creator_account_id()
              << std::endl;

    client.send(tx);

    auto stream = client.sendStatusStream(hash);
    ToriiResponse status;
    int index = 0;
    while (stream->Read(&status)) {
        printTxStatus(status, index++);
        throwOnFailureStatus(status);
    }
}

inline void sendBatchAndPrintStatus(
        GrpcClient& client,
        const std::vector<Transaction>& transactions)
{
    TxList tx_list;
    for (const auto& tx : transactions) {
        *tx_list.add_transactions() = tx;
    }
    client.send(tx_list);

    for (const auto& tx : transactions) {
        const auto hash = getTransactionHash(tx);
        std::cout << "Transaction hash = " << hash
                  << ", creator = " << tx.payload().reduced_payload().creator_account_id()
                  << std::endl;
        auto stream = client.sendStatusStream(hash);
        ToriiResponse status;
        int index = 0;
        while (stream->Read(&status)) {
            printTxStatus(status, index++);
            throwOnFailureStatus(status);
        }
    }
}

inline uint64_t nextQueryCounter()
{
    static uint64_t counter = 0;
    return ++counter;
}

inline Tx makeTx(
        const std::string& account_id,
        const iroha::keypair_t& keypair,
        uint32_t quorum = 1)
{
    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    return Tx(account_id, keypair, now, quorum);
}

}  // namespace iroha_example
