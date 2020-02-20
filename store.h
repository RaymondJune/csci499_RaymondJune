//
// Created by Ruimin Zeng on 2/18/20.
//

#ifndef CS499_RAYMONDJUNE_STORE_H
#define CS499_RAYMONDJUNE_STORE_H

#include <unordered_map>
#include <mutex>


// Store class used by key value store service implementation class
class Store {
public:
    bool Put(const std::string& key, const std::string& value);
    bool Remove(const std::string& key);
    std::string Get(const std::string& key);
private:
    std::mutex mu_;
    std::unordered_map<std::string, std::string> umap_; //unordered map used as the in-memory kvstore data structure
};

#endif //CS499_RAYMONDJUNE_STORE_H
