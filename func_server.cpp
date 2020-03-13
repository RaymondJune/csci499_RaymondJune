//
// Created by Ruimin Zeng on 2/15/20.
//

#include "func_server.h"

#include "kvstore_client.h"

Status FuncServiceImpl::hook(ServerContext* context, const HookRequest* request,
                             HookReply* reply) {
  int event_type = request->event_type();
  std::string event_function = request->event_function();
  KeyValueStoreClient kvstore(grpc::CreateChannel(
      "localhost:50001", grpc::InsecureChannelCredentials()));
  kvstore.Put(std::to_string(event_type), event_function);
  return Status::OK;
}

Status FuncServiceImpl::unhook(ServerContext* context,
                               const UnhookRequest* request,
                               UnhookReply* reply) {
  int event_type = request->event_type();
  KeyValueStoreClient kvstore(grpc::CreateChannel(
      "localhost:50001", grpc::InsecureChannelCredentials()));
  kvstore.Remove(std::to_string(event_type));
  return Status::OK;
}

Status FuncServiceImpl::event(ServerContext* context,
                              const EventRequest* request, EventReply* reply) {
  return Status::OK;
}
