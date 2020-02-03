//
// Created by Ruimin Zeng on 2/2/20.
//

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <grpcpp/grpcpp.h>

#include "kvstore.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using kvstore::PutRequest;
using kvstore::GetRequest;
using kvstore::RemoveRequest;
using kvstore::PutReply;
using kvstore::GetReply;
using kvstore::RemoveReply;
using kvstore::KeyValueStore;

class KeyValueStoreClient {
public:
    KeyValueStoreClient(std::shared_ptr<Channel> channel)
            : stub_(KeyValueStore::NewStub(channel)) {}

    bool remove(const std::string& key) {
        RemoveRequest request;
        request.set_key(key);

        RemoveReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->remove(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
            return true;
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return false;
        }
    }

    bool put(const std::string& key, const std::string& value) {
        ClientContext context;
        PutRequest request;
        PutReply reply;

        request.set_key(key);
        request.set_value(value);
        Status status = stub_->put(&context, request, &reply);

        if (status.ok()) {
            return true;
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return false;
        }
    }

    std::vector<std::string> get(const std::vector<std::string>& keys) {
        ClientContext context;

        std::shared_ptr<ClientReaderWriter<GetRequest, GetReply> > stream(
                stub_->get(&context));

        std::thread writer([stream, &keys]() {
            for (const std::string& key: keys) {
                GetRequest request;
                request.set_key(key);
                // TODO: add log messages
                stream->Write(request);
            }
            stream->WritesDone();
        });

        GetReply reply;
        std::vector<std::string> res;
        while (stream->Read(&reply)) {
            // TODO: add log messages
            res.push_back(reply.value());
        }
        writer.join();
        Status status = stream->Finish();
        if (!status.ok()) {
            // TODO: add log messages
            std::cout << "RouteChat rpc failed." << std::endl;
        }
        return res;
    }

private:
    std::unique_ptr<KeyValueStore::Stub> stub_;
};

int main(int argc, char** argv) {
    KeyValueStoreClient kvstore(grpc::CreateChannel(
            "localhost:50001", grpc::InsecureChannelCredentials()));
    std::string key("one");
    kvstore.put("one", "1");
    kvstore.put("two", "2");
    kvstore.put("three", "3");
    std::vector<std::string> keys({"one", "two", "three"});
    std::vector<std::string> replies = kvstore.get(keys);
    for (auto & reply : replies) {
        std::cout << "get reply: " << reply << std::endl;
    }
    return 0;
}
