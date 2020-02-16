//
// Created by Ruimin Zeng on 2/2/20.
//

#include <iostream>
#include <memory>
#include <vector>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "gflags/gflags.h"

#include "kvstore_client.h"

int main(int argc, char** argv) {
    KeyValueStoreClient kvstore(grpc::CreateChannel(
            "localhost:50001", grpc::InsecureChannelCredentials()));
    std::string key("one");
    kvstore.Put("one", "1");
    kvstore.Put("two", "2");
    kvstore.Put("three", "3");
    std::vector<std::string> keys({"one", "two", "three"});
    std::vector<std::string> replies = kvstore.Get(keys);
    for (auto & reply : replies) {
        std::cout << "get reply: " << reply << std::endl;
    }
    return 0;
}
