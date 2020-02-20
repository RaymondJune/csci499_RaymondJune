//
// Created by Ruimin Zeng on 2/2/20.
//

#include <iostream>
#include <memory>
#include <vector>

#include <gtest/gtest.h>
#include <glog/logging.h>
#include "gflags/gflags.h"
#include "kvstore_client.h"
#include "func_server.h"


void RunServer() {
    std::string server_address("0.0.0.0:50000");
    FuncServiceImpl service;

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());

    LOG(INFO) << "Func Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}


int main(int argc, char** argv) {
    fLS::FLAGS_log_dir = "/Users/zengruim/CLionProjects/cs499_raymondjune/logs";
    google::InitGoogleLogging(argv[0]);
    RunServer();
    return 0;
}