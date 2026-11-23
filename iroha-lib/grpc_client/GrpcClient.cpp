#include "GrpcClient.hpp"


namespace iroha_lib {

namespace {

std::shared_ptr<grpc::Channel> createChannel(
        const std::string& address,
        const std::optional<iroha_lib::TlsOptions>& tls)
{
    if (not tls) {
        return grpc::CreateChannel(
                    address,
                    grpc::InsecureChannelCredentials());
    }

    grpc::SslCredentialsOptions options;
    options.pem_root_certs = tls->pem_root_certs;
    options.pem_cert_chain = tls->pem_cert_chain;
    options.pem_private_key = tls->pem_private_key;
    return grpc::CreateChannel(address, grpc::SslCredentials(options));
}

}  // namespace


template <class Service>
std::unique_ptr<typename Service::StubInterface> createClient(
        const std::string& address,
        const std::optional<iroha_lib::TlsOptions>& tls)
{
    return Service::NewStub(createChannel(address, tls));
}


template <class Service>
std::unique_ptr<typename Service::StubInterface> createClient(
        const std::string& ip,
        const size_t port,
        const std::optional<iroha_lib::TlsOptions>& tls)
{
    const auto peer_ip = ip + ":" + std::to_string(port);
    return createClient<Service>(peer_ip, tls);
}


GrpcClient::GrpcClient(
        const std::string& target_ip,
        const uint16_t port,
        const std::optional<TlsOptions>& tls)
    : command_stub_(createClient<CommandService_v1>(
                                  target_ip,
                                  port,
                                  tls)),
      query_stub_(createClient<QueryService_v1>(
                                target_ip,
                                port,
                                tls))
{}

grpc::Status GrpcClient::send(const Transaction& tx)
{
    google::protobuf::Empty empty;
    grpc::ClientContext context;
    return command_stub_->Torii(
                &context,
                tx,
                &empty);
}

grpc::Status GrpcClient::send(const TxList& tx_list)
{
    google::protobuf::Empty empty;
    grpc::ClientContext context;
    return command_stub_->ListTorii(
                &context,
                tx_list,
                &empty);
}

grpc::Status GrpcClient::sendQuery(
        const iroha::protocol::Query& query,
        QueryResponse* response)
{
    grpc::ClientContext context;
    return query_stub_->Find(
                &context,
                query,
                response);
}

QueryResponse GrpcClient::send(const iroha::protocol::Query& query)
{
    QueryResponse queryResponse;
    sendQuery(query, &queryResponse);
    return queryResponse;
}

grpc::Status GrpcClient::getTxStatus(
        const std::string& tx_hash,
        ToriiResponse* response)
{
    TxStatusRequest statusRequest;
    statusRequest.set_tx_hash(tx_hash);
    grpc::ClientContext context;
    return command_stub_->Status(
                &context,
                statusRequest,
                response);
}

ToriiResponse GrpcClient::getTxStatus(const std::string& tx_hash)
{
    ToriiResponse toriiResponse;
    getTxStatus(tx_hash, &toriiResponse);
    return toriiResponse;
}

bool TxStatusStream::Read(ToriiResponse* response)
{
    return reader_->Read(response);
}

void TxStatusStream::Cancel()
{
    context_.TryCancel();
}

bool BlocksStream::Read(BlockQueryResponse* response)
{
    return reader_->Read(response);
}

void BlocksStream::Cancel()
{
    context_.TryCancel();
}

std::unique_ptr<TxStatusStream> GrpcClient::sendStatusStream(
        const std::string& tx_hash)
{
    auto stream = std::make_unique<TxStatusStream>();
    TxStatusRequest statusRequest;
    statusRequest.set_tx_hash(tx_hash);
    stream->reader_ = command_stub_->StatusStream(
                &stream->context_,
                statusRequest);
    return stream;
}

std::unique_ptr<BlocksStream> GrpcClient::sendBlocksStreamQuery(
        const iroha::protocol::BlocksQuery& blocks_query)
{
    auto stream = std::make_unique<BlocksStream>();
    stream->reader_ = query_stub_->FetchCommits(
                &stream->context_,
                blocks_query);
    return stream;
}
}  // namespace iroha_lib
