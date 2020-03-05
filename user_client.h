//
// Created by Ruimin Zeng on 2/20/20.
//

#ifndef CS499_RAYMONDJUNE_USER_CLIENT_H
#define CS499_RAYMONDJUNE_USER_CLIENT_H

#include <grpcpp/grpcpp.h>
#include "build/func.grpc.pb.h"
#include "build/warble.grpc.pb.h"
#include "event.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using func::EventRequest;
using func::EventReply;
using func::FuncService;

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

// This client is responsible for sending event requests to func
class UserClient {
public:
    explicit UserClient(std::shared_ptr<Channel> channel);

    // send event request
    bool Event(int event_type, google::protobuf::Any* payload);

private:
    //stub used to call actual rpc
    std::unique_ptr<FuncService::Stub> stub_;
};

#endif //CS499_RAYMONDJUNE_USER_CLIENT_H
