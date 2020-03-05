//
// Created by Ruimin Zeng on 2/15/20.
//

#include "kvstore_client.h"

#include <thread>

#include <glog/logging.h>

KeyValueStoreClient::KeyValueStoreClient(std::shared_ptr<Channel> channel)
        : stub_(KeyValueStore::NewStub(channel)) {}

bool KeyValueStoreClient::Remove(const std::string& key) {
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
        LOG(ERROR) << status.error_code() << ": " << status.error_message()
                   << std::endl;
        return false;
    }
}

bool KeyValueStoreClient::Put(const std::string& key, const std::string& value) {
    ClientContext context;
    PutRequest request;
    PutReply reply;

    request.set_key(key);
    request.set_value(value);
    Status status = stub_->put(&context, request, &reply);

    if (status.ok()) {
        return true;
    } else {
        LOG(ERROR) << status.error_code() << ": " << status.error_message()
                   << std::endl;
        return false;
    }
}

std::vector<std::string> KeyValueStoreClient::Get(const std::vector<std::string>& keys) {
    ClientContext context;

    std::shared_ptr<ClientReaderWriter<GetRequest, GetReply> > stream(
            stub_->get(&context));

    std::thread writer([stream, &keys]() {
        for (const std::string& key: keys) {
            GetRequest request;
            request.set_key(key);
            LOG(INFO) << "request for key: " << key << std::endl;
            stream->Write(request);
        }
        stream->WritesDone();
    });

    GetReply reply;
    std::vector<std::string> res;
    while (stream->Read(&reply)) {
        LOG(INFO) << "get reply: " << reply.value() << std::endl;
        res.push_back(reply.value());
    }
    writer.join();
    Status status = stream->Finish();
    if (!status.ok()) {
        LOG(ERROR) << "RouteChat rpc failed." << std::endl;
    }
    return res;
}

