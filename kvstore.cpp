//
// Created by Ruimin Zeng on 2/15/20.
//
#include "kvstore.h"

Status KeyValueStoreServiceImpl::put(ServerContext* context, const PutRequest* request, PutReply* reply) {
    mu_.lock();
    std::pair<UOMIterator, bool> result = umap_.insert(std::make_pair(request->key(), request->value()));
    mu_.unlock();
    return result.second? (Status::OK) : already_exist_;
}

Status KeyValueStoreServiceImpl::remove(ServerContext* context, const RemoveRequest* request, RemoveReply* reply) {
    mu_.lock();
    size_t k = umap_.erase(request->key());
    mu_.unlock();
    return k == 1? (Status::OK): not_found_;
}

Status KeyValueStoreServiceImpl::get(ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) {
    GetRequest request;
    GetReply reply;
    while (stream->Read(&request)) {
        auto iter = umap_.find(request.key());
        if (iter == umap_.end()) {
            reply.set_value("");
        } else {
            reply.set_value(iter->second);
        }
        stream->Write(reply);
    }

    return Status::OK;
}
