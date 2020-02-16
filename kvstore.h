//
// Created by Ruimin Zeng on 2/15/20.
//

#ifndef CS499_RAYMONDJUNE_KVSTORE_H
#define CS499_RAYMONDJUNE_KVSTORE_H

#include <unordered_map>
#include <mutex>
#include <grpcpp/grpcpp.h>
#include "kvstore.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using kvstore::PutRequest;
using kvstore::GetRequest;
using kvstore::RemoveRequest;
using kvstore::PutReply;
using kvstore::GetReply;
using kvstore::RemoveReply;
using kvstore::KeyValueStore;

typedef std::unordered_map<std::string, std::string>::iterator UOMIterator;

class KeyValueStoreServiceImpl final : public KeyValueStore::Service {
public:
    Status put(ServerContext* context, const PutRequest* request, PutReply* reply) override;

    Status remove(ServerContext* context, const RemoveRequest* request, RemoveReply* reply) override;

    Status get(ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) override;

private:
    std::mutex mu_;
    std::unordered_map<std::string, std::string> umap_; //unordered map used as the in-memory kvstore data structure

    // self defined status codes, not_found_ implies remove request failed while already_exist_ implies put request failed
    Status not_found_ = Status(StatusCode::NOT_FOUND, "key not found");
    Status already_exist_ = Status(StatusCode::ALREADY_EXISTS, "key already exists");
};

#endif //CS499_RAYMONDJUNE_KVSTORE_H
