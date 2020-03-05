//
// Created by Ruimin Zeng on 2/21/20.
//

#ifndef CS499_RAYMONDJUNE_WARBLE_SERVER_H
#define CS499_RAYMONDJUNE_WARBLE_SERVER_H

#include <iostream>
#include <optional>

#include "glog/logging.h"
#include "build/warble.grpc.pb.h"
#include "kvstore_client.h"

using warble::RegisteruserReply;
using warble::RegisteruserRequest;
using warble::WarbleRequest;
using warble::WarbleReply;
using warble::FollowRequest;
using warble::FollowReply;
using warble::ReadRequest;
using warble::ReadReply;
using warble::ProfileRequest;
using warble::ProfileReply;
using warble::Warble;
using warble::Timestamp;

// warble server class consistes of the functions to be called by func
class WarbleServer {
public:
    std::unordered_map<std::string, std::optional<std::string> (WarbleServer::*)(const google::protobuf::Any&)> function_map_{
            {"registeruser", &WarbleServer::Registeruser},
            {"warble", &WarbleServer::PublishWarble},
            {"follow", &WarbleServer::Follow},
            {"read", &WarbleServer::Read},
            {"profile", &WarbleServer::Profile}
    };

    explicit WarbleServer(KeyValueStoreClient& client);

    // register the given username in warble server
    std::optional<std::string> Registeruser(const google::protobuf::Any& payload);

    // store the warble in warble server, return warble reply message
    std::optional<std::string> PublishWarble(const google::protobuf::Any& payload);

    // store to_follow into username's following, store username to to_follow's follower
    std::optional<std::string> Follow(const google::protobuf::Any& payload);

    // given id, return the warble thread
    std::optional<std::string> Read(const google::protobuf::Any& payload);

    // return username's profile
    std::optional<std::string> Profile(const google::protobuf::Any& payload);

private:
    KeyValueStoreClient& kvstore_;
};


#endif //CS499_RAYMONDJUNE_WARBLE_SERVER_H
