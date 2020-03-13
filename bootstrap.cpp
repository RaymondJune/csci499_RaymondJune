//
// Created by Ruimin Zeng on 2/15/20.
//

#include "bootstrap.h"

#include <glog/logging.h>

BootstrapClient::BootstrapClient(std::shared_ptr<Channel> channel)
    : stub_(FuncService::NewStub(channel)) {}

// send hook request
bool BootstrapClient::Hook(int event_type, const std::string& function_name) {
  HookRequest request;
  request.set_event_type(event_type);
  request.set_event_function(function_name);
  HookReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->hook(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    return true;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message()
               << std::endl;
    return false;
  }
}

// send unhook request
bool BootstrapClient::Unhook(int event_type) {
  UnhookRequest request;
  request.set_event_type(event_type);
  UnhookReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->unhook(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    return true;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message()
               << std::endl;
    return false;
  }
}