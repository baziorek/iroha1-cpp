#include "GrpcClient.hpp"


namespace iroha_lib {

template <class Service>
std::shared_ptr<grpc::Channel> createUnauthenticatedChanel(const std::string& address)
{
    return grpc::CreateChannel(
                address,
                grpc::InsecureChannelCredentials());
}


template <class Service>
std::unique_ptr<typename Service::StubInterface> createClient(const std::string& address)
{
    return Service::NewStub(createUnauthenticatedChanel<Service>(address));
}


template <class Service>
std::unique_ptr<typename Service::StubInterface> createClient(
        const std::string& ip,
        const size_t port)
{
    const auto peer_ip = ip + ":" + std::to_string(port);
    return createClient<Service>(peer_ip);
}


GrpcClient::GrpcClient(
        const std::string& target_ip,
        const uint16_t port)
    : command_stub_(createClient<CommandService_v1>(
                                  target_ip,
                                  port)),
      query_stub_(createClient<QueryService_v1>(
                                target_ip,
                                port))
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

QueryResponse GrpcClient::send(const iroha::protocol::Query& query)
{
    QueryResponse queryResponse;
    grpc::ClientContext context;
    query_stub_->Find(
                &context,
                query,
                &queryResponse);
    return queryResponse;
}

ToriiResponse GrpcClient::getTxStatus(const std::string& tx_hash)
{
    TxStatusRequest statusRequest;
    statusRequest.set_tx_hash(tx_hash);
    ToriiResponse toriiResponse;
    grpc::ClientContext context;
    command_stub_->Status(
                &context,
                statusRequest,
                &toriiResponse);
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
