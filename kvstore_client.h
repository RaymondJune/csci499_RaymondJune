//
// Created by Ruimin Zeng on 2/15/20.
//

#ifndef CS499_RAYMONDJUNE_KVSTORE_CLIENT_H
#define CS499_RAYMONDJUNE_KVSTORE_CLIENT_H

#include <grpcpp/grpcpp.h>
#include "build/kvstore.grpc.pb.h"

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

// client class with interfaces func can call to access key value store service
class KeyValueStoreClient {
public:
    explicit KeyValueStoreClient(std::shared_ptr<Channel> channel);

    // remove key-value pair from store
    bool Remove(const std::string& key);

    // put new key-value pair into store
    bool Put(const std::string& key, const std::string& value);

    // get the associated values for keys, if a key does not exist, an empty string is returned
    std::vector<std::string> Get(const std::vector<std::string>& keys);

private:
    //stub used to call actual rpc
    std::unique_ptr<KeyValueStore::Stub> stub_;
};


#endif //CS499_RAYMONDJUNE_KVSTORE_CLIENT_H
