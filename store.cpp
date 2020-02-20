//
// Created by Ruimin Zeng on 2/18/20.
//

#include "store.h"

#include <glog/logging.h>

typedef std::unordered_map<std::string, std::string>::iterator UOMIterator;

bool Store::Put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lck (mu_);
    std::pair<UOMIterator, bool> result = umap_.insert(std::make_pair(key, value));
    return result.second;
}

bool Store::Remove(const std::string& key) {
    std::lock_guard<std::mutex> lck (mu_);
    size_t k = umap_.erase(key);
    return k == 1;
}

std::string Store::Get(const std::string& key) {
    auto iter = umap_.find(key);
    std::string res;
    if (iter == umap_.end()) {
        res = "";
        LOG(WARNING) << "try to access a nonexistent key" << std::endl;
    } else {
        res = iter->second;
    }
    return res;
}