//
// Created by Ruimin Zeng on 2/15/20.
//

#include "func_server.h"

#include "event.h"
#include "warble_server.h"

FuncServiceImpl::FuncServiceImpl()
    : kvstore_(grpc::CreateChannel("localhost:50001",
                                   grpc::InsecureChannelCredentials())),
      warbleServer_(WarbleServer(kvstore_)) {}

Status FuncServiceImpl::hook(ServerContext* context, const HookRequest* request,
                             HookReply* reply) {
  int event_type = request->event_type();
  const std::string& event_function = request->event_function();
  kvstore_.Put(std::to_string(event_type), event_function);
  return Status::OK;
}

Status FuncServiceImpl::unhook(ServerContext* context,
                               const UnhookRequest* request,
                               UnhookReply* reply) {
  int event_type = request->event_type();
  kvstore_.Remove(std::to_string(event_type));
  return Status::OK;
}

Status FuncServiceImpl::event(ServerContext* context,
                              const EventRequest* request, EventReply* reply) {
  int event_type = request->event_type();
  std::string event_function =
      kvstore_.Get(std::vector<std::string>(1, std::to_string(event_type)))[0];
  LOG(INFO) << "calling " << event_function << std::endl;
  if (!event_function.empty()) {
    auto* payload = new google::protobuf::Any();
    std::optional<std::string> replyMessage =
        (warbleServer_.*
         (warbleServer_.function_map_[event_function]))(request->payload());
    if (event_type == EVENT::PROFILE) {
      ProfileReply profileReply;
      profileReply.ParseFromString(replyMessage.value());
      payload->PackFrom(profileReply);
    } else if (event_type == EVENT::READ) {
      ReadReply readReply;
      readReply.ParseFromString(replyMessage.value());
      payload->PackFrom(readReply);
    } else if (event_type == EVENT::WARBLE) {
      WarbleReply warbleReply;
      warbleReply.ParseFromString(replyMessage.value());
      payload->PackFrom(warbleReply);
    }
    reply->set_allocated_payload(payload);
  } else {
    // event is not hooked
    return Status(StatusCode::NOT_FOUND, "the event is not hooked");
  }
  return Status::OK;
}
