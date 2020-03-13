//
// Created by Ruimin Zeng on 2/15/20.
//

#ifndef CS499_RAYMONDJUNE_FUNC_SERVER_H
#define CS499_RAYMONDJUNE_FUNC_SERVER_H

#include <grpcpp/grpcpp.h>

#include "build/func.grpc.pb.h"
#include "kvstore_client.h"
#include "warble_server.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using func::EventReply;
using func::EventRequest;
using func::FuncService;
using func::HookReply;
using func::HookRequest;
using func::UnhookReply;
using func::UnhookRequest;

/*
 *  Service class which gets hook/unhook request from bootstrap client
 * (service_operator), used by func
 */
class FuncServiceImpl final : public FuncService::Service {
 public:
  // constructor, initialize kvstore client
  FuncServiceImpl();

  // hook gRPC call in server side
  Status hook(ServerContext* context, const HookRequest* request,
              HookReply* reply) override;

  // unhook gRPC call in server side
  Status unhook(ServerContext* context, const UnhookRequest* request,
                UnhookReply* reply) override;

  // event gRPC call in server side
  Status event(ServerContext* context, const EventRequest* request,
               EventReply* reply) override;

 private:
  KeyValueStoreClient kvstore_;
  WarbleServer warbleServer_;
};

#endif  // CS499_RAYMONDJUNE_FUNC_SERVER_H
