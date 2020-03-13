//
// Created by Ruimin Zeng on 2/15/20.
//

#ifndef CS499_RAYMONDJUNE_BOOTSTRAP_H
#define CS499_RAYMONDJUNE_BOOTSTRAP_H

#include <grpcpp/grpcpp.h>

#include "build/func.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using func::FuncService;
using func::HookReply;
using func::HookRequest;
using func::UnhookReply;
using func::UnhookRequest;

// This client is responsible for sending hook/unhook requests to func
class BootstrapClient {
 public:
  explicit BootstrapClient(std::shared_ptr<Channel> channel);

  // send hook request
  bool Hook(int event_type, const std::string& function_name);

  // send unhook request
  bool Unhook(int event_type);

 private:
  // stub used to call actual rpc
  std::unique_ptr<FuncService::Stub> stub_;
};

#endif  // CS499_RAYMONDJUNE_BOOTSTRAP_H
