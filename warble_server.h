//
// Created by Ruimin Zeng on 2/21/20.
//

#ifndef CS499_RAYMONDJUNE_WARBLE_SERVER_H
#define CS499_RAYMONDJUNE_WARBLE_SERVER_H

#include <iostream>
#include <optional>

#include "build/warble.grpc.pb.h"
#include "glog/logging.h"
#include "kvstore_client.h"

using warble::FollowReply;
using warble::FollowRequest;
using warble::ProfileReply;
using warble::ProfileRequest;
using warble::ReadReply;
using warble::ReadRequest;
using warble::RegisteruserReply;
using warble::RegisteruserRequest;
using warble::Timestamp;
using warble::Warble;
using warble::WarbleReply;
using warble::WarbleRequest;

// warble server class consistes of the functions to be called by func
class WarbleServer {
 public:
  std::unordered_map<std::string, std::optional<std::string> (WarbleServer::*)(
                                      const google::protobuf::Any&)>
      function_map_{{"registeruser", &WarbleServer::Registeruser},
                    {"warble", &WarbleServer::PublishWarble},
                    {"follow", &WarbleServer::Follow},
                    {"read", &WarbleServer::Read},
                    {"profile", &WarbleServer::Profile},
                    {"stream", &WarbleServer::Stream}};

  explicit WarbleServer(KeyValueStoreClient& client);

  // register the given username in warble server
  std::optional<std::string> Registeruser(const google::protobuf::Any& payload);

  // store the warble in warble server, return warble reply message
  std::optional<std::string> PublishWarble(
      const google::protobuf::Any& payload);

  // store to_follow into username's following, store username to to_follow's
  // follower
  std::optional<std::string> Follow(const google::protobuf::Any& payload);

  // given id, return the warble thread
  std::optional<std::string> Read(const google::protobuf::Any& payload);

  // return username's profile
  std::optional<std::string> Profile(const google::protobuf::Any& payload);

  // read the latest warble with given hashtag and return it packed in payload
  std::optional<std::string> Stream(const google::protobuf::Any& payload);

 private:
  KeyValueStoreClient& kvstore_;
  // private helper function to check if the string toCheck exists in the
  // stringstream
  bool Check(std::stringstream& ss, const std::string& toCheck);

  // private helper function to check if the user is registered
  bool ValidateUser(const std::string& username);
};

#endif  // CS499_RAYMONDJUNE_WARBLE_SERVER_H
