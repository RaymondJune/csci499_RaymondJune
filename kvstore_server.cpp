//
// Created by Ruimin Zeng on 2/2/20.
//

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <grpcpp/grpcpp.h>
#include <grpc/impl/codegen/status.h>
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
typedef std::unordered_map<std::string, std::string>::const_iterator UOMConstIterator;

class KeyValueStoreServiceImpl final : public KeyValueStore::Service {
public:
    Status put(ServerContext* context, const PutRequest* request,
               PutReply* reply) override {
        mu_.lock();
        std::pair<UOMIterator, bool> result = umap_.insert(std::make_pair(request->key(), request->value()));
        mu_.unlock();
        return result.second? (Status::OK) : (Status::CANCELLED);
    }

    Status remove(ServerContext* context, const RemoveRequest* request,
                    RemoveReply* reply) override {
        mu_.lock();
        size_t k = umap_.erase(request->key());
        mu_.unlock();
        return k == 1? (Status::OK): Status::CANCELLED;
    }

    Status get(ServerContext* context,
                     ServerReaderWriter<GetReply, GetRequest>* stream) override {
        GetRequest request;
        GetReply reply;
        while (stream->Read(&request)) {
            UOMConstIterator iter = umap_.find(request.key());
            if (iter == umap_.end()) {
                reply.set_value("");
            } else {
                reply.set_value(iter->second);
            }
            stream->Write(reply);
        }

        return Status::OK;
    }


private:
    std::mutex mu_;
    std::unordered_map<std::string, std::string> umap_;
    //const Status not_found_(StatusCode::NOT_FOUND, "key not found");
    //const Status already_exists(StatusCode::ALREADY_EXISTS, "key already exists");
};

void RunServer() {
    std::string server_address("0.0.0.0:50001");
    KeyValueStoreServiceImpl service;

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}


int main(int argc, char** argv) {
    RunServer();
    return 0;
}