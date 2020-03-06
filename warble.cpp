//
// Created by Ruimin Zeng on 2/20/20.
//

#include "glog/logging.h"
#include "gflags/gflags.h"
#include "user_client.h"

DEFINE_string(registeruser, "", "Registers the given username");
DEFINE_string(user, "", "Logs in as the given username");
DEFINE_string(warble, "", "Creates a new warble with the given text");
DEFINE_string(reply, "", "Indicates that the new warble is a reply to the given id");
DEFINE_string(follow, "", "Starts following the given username");
DEFINE_string(read, "", "Reads the warble thread starting at the given id");
DEFINE_bool(profile, false, "Gets the user’s profile of following and followers");

int main(int argc, char** argv) {
    UserClient userClient(grpc::CreateChannel(
            "localhost:50000", grpc::InsecureChannelCredentials()));
    gflags::SetUsageMessage("usage example:\n./warble --registeruser alice\n./warble --user alice --warble 'hello, world!'\n./warble --user alice --warble 'hello, world!' --reply warble_id_1583442592195709\n./warble --user alice --follow Bob\n./warble --read warble_id_1583442592195709\n./warble --user alice --profile");
    gflags::SetVersionString("1.0.0");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    auto* payload = new google::protobuf::Any();

    // ./warble --registeruser alice
    if (!FLAGS_registeruser.empty()) {
        LOG(INFO) << "register username - " << FLAGS_registeruser << std::endl;
        RegisteruserRequest request;
        request.set_username(FLAGS_registeruser);
        payload->PackFrom(request);
        userClient.Event(EVENT::REGISTER, payload);
    }

    // ./warble --user alice --warble 'hello, world!'
    else if (!FLAGS_user.empty() && !FLAGS_warble.empty()) {
        LOG(INFO) << FLAGS_user << " publishes warble " << FLAGS_warble << std::endl;
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
    }

    // ./warble --user alice --follow Bob
    else if (!FLAGS_user.empty() && !FLAGS_follow.empty()) {
        LOG(INFO) << FLAGS_user << " follows " << FLAGS_follow << std::endl;
        FollowRequest request;
        request.set_username(FLAGS_user);
        request.set_to_follow(FLAGS_follow);
        payload->PackFrom(request);
        userClient.Event(EVENT::FOLLOW, payload);
    }

    // ./warble --read warble_id_1583442592195709
    else if (!FLAGS_read.empty()) {
        LOG(INFO) << "Reads warble id " << FLAGS_read << std::endl;
        ReadRequest request;
        request.set_warble_id(FLAGS_read);
        payload->PackFrom(request);
        userClient.Event(EVENT::READ, payload);
    }

    // ./warble --user alice --profile
    else if (!FLAGS_user.empty() && FLAGS_profile) {
        LOG(INFO) << "show " << FLAGS_user << "'s profile" << FLAGS_read << std::endl;
        ProfileRequest request;
        request.set_username(FLAGS_user);
        payload->PackFrom(request);
        userClient.Event(EVENT::PROFILE, payload);
    }

    // wrong combinations of command line arguments, show usage information to users
    else {
        std::cout << "Unknown command, Usage: \n./warble --registeruser alice\n./warble --user alice --warble 'hello, world!'\n./warble --user alice --warble 'hello, world!' --reply warble_id_1583442592195709\n./warble --user alice --follow Bob\n./warble --read warble_id_1583442592195709\n./warble --user alice --profile" << std::endl;
    }

    gflags::ShutDownCommandLineFlags();
    return 0;
}