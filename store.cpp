//
// Created by Ruimin Zeng on 2/18/20.
//

#include "store.h"

#include <glog/logging.h>

Store::Store(std::optional<std::string>& filename) {
  if (filename != std::nullopt) {
    std::ifstream fin;
    std::string line;
    std::string key;
    std::string val;
    fin.open(filename.value());
    if (!fin.is_open()) {
      LOG(WARNING)
          << "Unable to open file " << filename.value()
          << " or the file does not exist. Initialize an empty key value store"
          << std::endl;
      return;
    }
    std::stringstream ss;
    ss << fin.rdbuf();
    std::string s = ss.str();
    fin.close();
    size_t pos = 0;
    int p = 0;
    while ((pos = s.find(kDelimiter)) != std::string::npos) {
      line = s.substr(0, pos);
      p = line.find(':');
      key = line.substr(0, p);
      val = line.substr(p + 1);
      umap_[key] = val;
      s.erase(0, pos + kDelimiter.length());
    }
    LOG(INFO) << "load entries from the specified previously-stored file "
              << filename.value() << std::endl;
  } else {
    LOG(INFO) << "flag --store is not set, initialize an empty key value store "
                 "service"
              << std::endl;
  }
}

bool Store::Put(const std::string& key, const std::string& value) {
  std::lock_guard<std::mutex> lck(mu_);
  auto iter = umap_.find(key);
  if (iter == umap_.end()) {
    umap_.insert(std::make_pair(key, value));
    return true;
  }
  iter->second = value;
  return false;
}

bool Store::Remove(const std::string& key) {
  std::lock_guard<std::mutex> lck(mu_);
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

void Store::dumpStoreToFile(std::optional<std::string>& filename) {
  if (filename == std::nullopt) {
    LOG(INFO) << "flag --store is not set,  shut down kvstore service directly"
              << std::endl;
    return;
  }
  std::ofstream fout;
  fout.open(filename.value());
  if (!fout.is_open()) {
    LOG(ERROR) << "Unable to open file" << filename.value() << std::endl;
    exit(1);  // terminate with error
  }
  for (auto const& [key, val] : umap_) {
    if (!key.empty()) {
      fout << key << ":" << val << kDelimiter;
    }
  }
  fout.close();
  LOG(INFO) << "flag --store is set,  dump all key-value pairs into "
            << filename.value() << std::endl;
}
