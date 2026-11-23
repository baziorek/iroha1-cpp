#pragma once

#include <endpoint.grpc.pb.h>
#include <grpc++/grpc++.h>
#include <memory>


namespace iroha_lib {

using namespace iroha::protocol;

/**
 * Server-streaming reader for CommandService_v1::StatusStream.
 * Read statuses with Read() until it returns false (final status reached
 * or the node closed the stream). Destroying the object cancels the RPC.
 */
class TxStatusStream {
public:
    bool Read(ToriiResponse* response);
    void Cancel();

private:
    friend class GrpcClient;
    grpc::ClientContext context_;
    std::unique_ptr<grpc::ClientReaderInterface<ToriiResponse>> reader_;
};

/**
 * Server-streaming reader for QueryService_v1::FetchCommits (blocks stream).
 * Read blocks with Read() until it returns false or Cancel() is called.
 */
class BlocksStream {
public:
    bool Read(BlockQueryResponse* response);
    void Cancel();

private:
    friend class GrpcClient;
    grpc::ClientContext context_;
    std::unique_ptr<grpc::ClientReaderInterface<BlockQueryResponse>> reader_;
};

class GrpcClient {

public:
    GrpcClient(
            const std::string& target_ip,
            const uint16_t port);
    grpc::Status send(const Transaction& tx);
    grpc::Status send(const TxList& tx_list);
    QueryResponse send(const iroha::protocol::Query& query);
    grpc::Status sendQuery(
            const iroha::protocol::Query& query,
            QueryResponse* response);
    ToriiResponse getTxStatus(const std::string& tx_hash);
    grpc::Status getTxStatus(
            const std::string& tx_hash,
            ToriiResponse* response);

    std::unique_ptr<TxStatusStream> sendStatusStream(
            const std::string& tx_hash);

    std::unique_ptr<BlocksStream> sendBlocksStreamQuery(
            const iroha::protocol::BlocksQuery& blocks_query);

private:
    std::shared_ptr<CommandService_v1::StubInterface> command_stub_;
    std::shared_ptr<QueryService_v1::StubInterface> query_stub_;
};

}
