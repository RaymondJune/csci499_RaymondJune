//
// Created by Ruimin Zeng on 2/20/20.
//

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "user_client.h"

DEFINE_string(registeruser, "", "Registers the given username");
DEFINE_string(user, "", "Logs in as the given username");
DEFINE_string(warble, "", "Creates a new warble with the given text");
DEFINE_string(reply, "",
              "Indicates that the new warble is a reply to the given id");
DEFINE_string(follow, "", "Starts following the given username");
DEFINE_string(read, "", "Reads the warble thread starting at the given id");
DEFINE_bool(profile, false,
            "Gets the user’s profile of following and followers");
DEFINE_string(stream, "", "Hashtag you would like to track");

int main(int argc, char** argv) {
  UserClient userClient(grpc::CreateChannel(
      "localhost:50000", grpc::InsecureChannelCredentials()));
  gflags::SetUsageMessage(
      "usage example:\n./warble --registeruser alice\n./warble --user alice "
      "--warble 'hello, world!'\n./warble --user alice --warble 'hello, "
      "world!' --reply warble_id_1583442592195709\n./warble --user alice "
      "--follow Bob\n./warble --user alice --read "
      "warble_id_1583442592195709\n./warble "
      "--user alice --profile");
  gflags::SetVersionString("1.0.0");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  auto* payload = new google::protobuf::Any();

  // users need to login at frst so as to do tasks except for registeruser
  if (FLAGS_registeruser.empty() && FLAGS_user.empty()) {
    LOG(WARNING) << "users need to login at frst so as to do tasks except for "
                    "registeruser"
                 << std::endl;
    std::cout << "please specify --user to login frist" << std::endl;
    return 0;
  }

  // deal with different commands
  if (!FLAGS_registeruser.empty()) {
    // eg: ./warble --registeruser alice
    LOG(INFO) << "register username - " << FLAGS_registeruser << std::endl;
    RegisteruserRequest request;
    request.set_username(FLAGS_registeruser);
    payload->PackFrom(request);
    userClient.Event(EVENT::REGISTER, payload);
  } else if (!FLAGS_warble.empty()) {
    // eg: ./warble --user alice --warble 'hello, world!'
    LOG(INFO) << FLAGS_user << " publishes warble " << FLAGS_warble
              << std::endl;
    WarbleRequest request;
    request.set_username(FLAGS_user);
    request.set_text(FLAGS_warble);
    if (!FLAGS_reply.empty()) {
      request.set_parent_id(FLAGS_reply);
    } else {
      request.set_parent_id("warble_id_root");
    }

    payload->PackFrom(request);
    userClient.Event(EVENT::WARBLE, payload);
  } else if (!FLAGS_follow.empty()) {
    // eg: ./warble --user alice --follow Bob
    LOG(INFO) << FLAGS_user << " follows " << FLAGS_follow << std::endl;
    FollowRequest request;
    request.set_username(FLAGS_user);
    request.set_to_follow(FLAGS_follow);
    payload->PackFrom(request);
    userClient.Event(EVENT::FOLLOW, payload);
  } else if (!FLAGS_read.empty()) {
    // eg: ./warble --user alice --read warble_id_1583442592195709
    LOG(INFO) << "Reads warble id " << FLAGS_read << std::endl;
    ReadRequest request;
    request.set_username(FLAGS_user);
    request.set_warble_id(FLAGS_read);
    payload->PackFrom(request);
    userClient.Event(EVENT::READ, payload);
  } else if (FLAGS_profile) {
    // eg: ./warble --user alice --profile
    LOG(INFO) << "show " << FLAGS_user << "'s profile" << FLAGS_read
              << std::endl;
    ProfileRequest request;
    request.set_username(FLAGS_user);
    payload->PackFrom(request);
    userClient.Event(EVENT::PROFILE, payload);
  } else if (!FLAGS_stream.empty()) {
    // eg: ./warble --user alice --stream "#helloworld"
    LOG(INFO) << "streaming hashtag " << FLAGS_stream << std::endl;
    StreamRequest request;
    request.set_tag(FLAGS_stream);
    payload->PackFrom(request);
    userClient.Event(EVENT::STREAM, payload);
  } else {
    // wrong combinations of command line arguments, show usage information to
    // users
    std::cout << "Unknown command, Usage: \n./warble --registeruser "
                 "alice\n./warble --user alice --warble 'hello, "
                 "world!'\n./warble --user alice --warble 'hello, world!' "
                 "--reply warble_id_1583442592195709\n./warble --user alice "
                 "--follow Bob\n./warble --read "
                 "warble_id_1583442592195709 --user alice\n./warble --user "
                 "alice --profile"
              << std::endl;
  }

  gflags::ShutDownCommandLineFlags();
  return 0;
}