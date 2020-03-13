//
// Created by Ruimin Zeng on 2/15/20.
//
#include "bootstrap.h"
#include "event.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_bool(default, false, "default to hook all five event types");
DEFINE_bool(hook, true,
            "hook event type to event function, when set to false, unhook the "
            "event type");
DEFINE_string(events, "0,1,2,3,4",
              "a string of event types seperated by comma to hook/unhook, only "
              "0,1,2,3,4 are allowed at present, 0: registeruser, 1: warble, "
              "2: follow, 3: read, 4: profile");

static bool IsValidEvents(const char *flagname, const std::string &value) {
  size_t i = 0;
  while (i < value.size()) {
    if (i % 2 == 0 && !(value[i] >= '0' && value[i] <= '4')) return false;
    if (i % 2 == 1 && value[i] != ',') return false;
    i++;
  }
  return true;
}
DEFINE_validator(events, &IsValidEvents);

// TODO: validator about default and hook are mutual exclusive

int main(int argc, char **argv) {
  BootstrapClient bootstrap(grpc::CreateChannel(
      "localhost:50000", grpc::InsecureChannelCredentials()));
  gflags::SetUsageMessage(
      "usage example: ./bootstrap [--default] | [--hook=false --event 0]");
  gflags::SetVersionString("1.0.0");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_default) {
    LOG(INFO) << "default hooking..." << std::endl;
    bootstrap.Hook(EVENT::REGISTER, event_functions[EVENT::REGISTER]);
    bootstrap.Hook(EVENT::WARBLE, event_functions[EVENT::WARBLE]);
    bootstrap.Hook(EVENT::FOLLOW, event_functions[EVENT::FOLLOW]);
    bootstrap.Hook(EVENT::READ, event_functions[EVENT::READ]);
    bootstrap.Hook(EVENT::PROFILE, event_functions[EVENT::PROFILE]);
  } else if (FLAGS_hook) {
    LOG(INFO) << "user defined hooking..." << std::endl;
    size_t i = 0;
    std::string value = FLAGS_events;
    while (i < value.size()) {
      bootstrap.Hook(value[i] - '0', event_functions[value[i] - '0']);
      i += 2;
    }
    LOG(INFO) << "user defined hooking done." << std::endl;
  } else {
    LOG(INFO) << "user defined unhooking..." << std::endl;
    size_t i = 0;
    std::string value = FLAGS_events;
    while (i < value.size()) {
      bootstrap.Unhook(value[i] - '0');
      i += 2;
    }
    LOG(INFO) << "user defined unhooking done." << std::endl;
  }
  LOG(INFO) << "bootstrap done." << std::endl;
  gflags::ShutDownCommandLineFlags();
  return 0;
}