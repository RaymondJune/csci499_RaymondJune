//
// Created by Ruimin Zeng on 2/20/20.
//

#include "user_client.h"

#include <glog/logging.h>

UserClient::UserClient(std::shared_ptr<Channel> channel)
    : stub_(FuncService::NewStub(channel)) {}

// send event request, and get event reply
bool UserClient::Event(int event_type, google::protobuf::Any* payload) {
  EventRequest request;
  request.set_event_type(event_type);
  request.set_allocated_payload(payload);
  EventReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  std::unique_ptr<ClientReader<EventReply> > reader(
      stub_->event(&context, request));

  // read the response into reply object
  while (reader->Read(&reply)) {
    const google::protobuf::Any& replyPayload = reply.payload();
    bool canParse;

    // stream has different processing logic as it is incapable of
    // returning an OK status since it is a potentially non-terminating call
    if (event_type == EVENT::STREAM) {
      StreamReply streamReply;
      canParse = replyPayload.UnpackTo(&streamReply);
      std::cout << "Streaming Warble! " << std::endl;
      std::cout << "the warble content is: " << std::endl;
      std::cout << streamReply.warble().text() << std::endl;
      std::cout << "the warble id is: " << std::endl;
      std::cout << streamReply.warble().id() << std::endl;
      continue;
    }

    // Return false if in error (reader isn't finished is not an error in a
    // non-terminating call like stream.)
    Status status = reader->Finish();
    if (!status.ok()) {
      LOG(ERROR) << status.error_code() << ": " << status.error_message()
                 << std::endl;
      std::cout << status.error_message() << std::endl;
      return false;
    }

    // TODO: deal with different reply messages
    if (event_type == EVENT::REGISTER) {
      std::cout << "successfully registed your username" << std::endl;
    } else if (event_type == EVENT::WARBLE) {
      WarbleReply warbleReply;
      canParse = replyPayload.UnpackTo(&warbleReply);
      if (canParse) {
        std::cout << "successfully published warble" << std::endl;
        std::cout << "the warble content is:" << std::endl;
        std::cout << warbleReply.warble().text() << std::endl;
        std::cout << "the warble id is:" << std::endl;
        std::cout << warbleReply.warble().id() << std::endl;
      } else {
        std::cout << "reply message parse error" << std::endl;
      }
    } else if (event_type == EVENT::READ) {
      ReadReply readReply;
      canParse = replyPayload.UnpackTo(&readReply);
      if (canParse) {
        std::cout << "successfully read the warble thread" << std::endl;
        for (int i = 1; i <= readReply.warbles_size(); i++) {
          std::cout << "warble " << i << ":" << std::endl;
          std::cout << readReply.warbles(i - 1).text() << std::endl;
        }
      } else {
        std::cout << "reply message parse error" << std::endl;
      }
    } else if (event_type == EVENT::FOLLOW) {
      std::cout << "successfully follow user" << std::endl;
    } else if (event_type == EVENT::PROFILE) {
      ProfileReply profileReply;
      canParse = replyPayload.UnpackTo(&profileReply);
      if (canParse) {
        std::cout << "successfully get profile" << std::endl;
        std::cout << "followers: " << std::endl;
        for (const std::string& follower : profileReply.followers()) {
          std::cout << follower << std::endl;
        }
        std::cout << "followings: " << std::endl;
        for (const std::string& following : profileReply.following()) {
          std::cout << following << std::endl;
        }
      } else {
        std::cout << "reply message parse error" << std::endl;
      }
      return true;
    }
  }
}