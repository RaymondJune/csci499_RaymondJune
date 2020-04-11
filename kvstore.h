//
// Created by Ruimin Zeng on 2/15/20.
//

#ifndef CS499_RAYMONDJUNE_KVSTORE_H
#define CS499_RAYMONDJUNE_KVSTORE_H

#include <grpcpp/grpcpp.h>

#include <mutex>
#include <unordered_map>

#include "build/kvstore.grpc.pb.h"
#include "store.h"

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

// key value store service class, which depends on Store class to actually store
// data
class KeyValueStoreServiceImpl final : public KeyValueStore::Service {
 public:
  explicit KeyValueStoreServiceImpl(std::optional<std::string> filename);
  // gRPC call put
  Status put(ServerContext* context, const PutRequest* request,
             PutReply* reply) override;

  // gRPC call remove
  Status remove(ServerContext* context, const RemoveRequest* request,
                RemoveReply* reply) override;

  // gRPC call get
  Status get(ServerContext* context,
             ServerReaderWriter<GetReply, GetRequest>* stream) override;

  // if the key-value store service is cleanly terminated, dump store pairs into
  // given file
  void dumpStoreToFile(std::optional<std::string>& filename);

 private:
  Store store_;  // Store object to store data

  // self defined status codes, not_found_ implies remove request failed while
  // already_exist_ implies put request failed
  const Status kNotFound_ = Status(StatusCode::NOT_FOUND, "key not found");
  const Status kAlreadyExist_ =
      Status(StatusCode::ALREADY_EXISTS, "key already exists");
};

#endif  // CS499_RAYMONDJUNE_KVSTORE_H
