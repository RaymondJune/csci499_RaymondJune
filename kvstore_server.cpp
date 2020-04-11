//
// Created by Ruimin Zeng on 2/2/20.
//

#include <glog/logging.h>

#include <iostream>
#include <memory>

#include "gflags/gflags.h"
#include "kvstore.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using kvstore::GetReply;
using kvstore::GetRequest;
using kvstore::KeyValueStore;
using kvstore::PutReply;
using kvstore::PutRequest;
using kvstore::RemoveReply;
using kvstore::RemoveRequest;

KeyValueStoreServiceImpl* kvService;
std::optional<std::string> filename;

DEFINE_string(store, "", "a file path used to make store disk persistent");

void RunServer() {
  std::string server_address("0.0.0.0:50001");

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(kvService);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());

  LOG(INFO) << "Kvstore Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

void signal_handler(int signal_num) {
  LOG(INFO) << "The interrupt signal is (" << signal_num << "). \n";
  kvService->dumpStoreToFile(filename);
  // terminate program
  exit(signal_num);
}

int main(int argc, char** argv) {
  //  fLS::FLAGS_log_dir =
  //  "/Users/zengruim/CLionProjects/cs499_raymondjune/logs";
  google::InitGoogleLogging(argv[0]);
  gflags::SetUsageMessage(
      "usage example: ./kvstore_server [--store output.txt]");
  gflags::SetVersionString("1.0.0");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  signal(SIGINT, signal_handler);
  filename =
      FLAGS_store.empty() ? std::nullopt : std::make_optional(FLAGS_store);
  KeyValueStoreServiceImpl service(filename);
  kvService = &service;
  RunServer();
  return 0;
}