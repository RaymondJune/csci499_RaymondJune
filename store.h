//
// Created by Ruimin Zeng on 2/18/20.
//

#ifndef CS499_RAYMONDJUNE_STORE_H
#define CS499_RAYMONDJUNE_STORE_H

#include <fstream>
#include <mutex>
#include <unordered_map>

// Store class used by key value store service implementation class
class Store {
 public:
  explicit Store(std::optional<std::string>& filename);
  // when key does not exist, add (key, value) pair into unordered map, return
  // true, otherwise update the value but return false
  bool Put(const std::string& key, const std::string& value);

  // remove key and its associated value from unordered map, return true, when
  // key does not exist, return false
  bool Remove(const std::string& key);

  // access the associative value of key in unordered map, when key does not
  // exist, return an empty string
  std::string Get(const std::string& key);

  // make kvstore umap_ disk persistent
  void dumpStoreToFile(std::optional<std::string>& filename);

 private:
  std::mutex mu_;  // a mutex to avoid racing condition when a Store object is
                   // used by multiple threads
  std::unordered_map<std::string, std::string>
      umap_;  // unordered map used as the in-memory kvstore data structure
};

#endif  // CS499_RAYMONDJUNE_STORE_H
