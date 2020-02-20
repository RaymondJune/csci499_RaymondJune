//
// Created by Ruimin Zeng on 2/15/20.
//
#include "kvstore.h"

#include "glog/logging.h"

Status KeyValueStoreServiceImpl::put(ServerContext* context, const PutRequest* request, PutReply* reply) {
    bool res = store_.Put(request->key(), request->value());
    if (res) {
        LOG(INFO) << "successfully add (key, value) pair, (" << request->key() << ", " << request->value() << ") into kvstore" << std::endl;
    }
    return res? (Status::OK) : kAlreadyExist_;
}

Status KeyValueStoreServiceImpl::remove(ServerContext* context, const RemoveRequest* request, RemoveReply* reply) {
    bool res = store_.Remove(request->key());
    if (res) {
        LOG(INFO) << "successfully removed key, (" << request->key() << ") from kvstore" << std::endl;
    }
    return res? (Status::OK): kNotFound_;
}

Status KeyValueStoreServiceImpl::get(ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) {
    GetRequest request;
    GetReply reply;
    while (stream->Read(&request)) {
        reply.set_value(store_.Get(request.key()));
        stream->Write(reply);
    }

    return Status::OK;
}
