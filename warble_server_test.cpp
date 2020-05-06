#include "warble_server.h"
#include <unordered_set>

#include <gtest/gtest.h>

#include <iostream>

// unit tests for stream feature of Warble Server
// It only tests GetHashtags because the rest of the Stream functionality is
// either:
// 1) Tested by store_test (GETs and PUTs)
// 2) is not to be tested (GRPC calls)
class WarbleServerStreamTest : public ::testing::Test {
 protected:
  WarbleServerStreamTest() {}
  ~WarbleServerStreamTest() {}

  void SetUp() override {
    kv_client_ = new KeyValueStoreClient(grpc::CreateChannel(
        "localhost:50001", grpc::InsecureChannelCredentials()));
    warble_server_ = new WarbleServer(*kv_client_);
  }

  void TearDown() override {
    delete kv_client_;
    delete warble_server_;
  }
  // we don't actually connect to this, it is only a placeholder
  KeyValueStoreClient* kv_client_;

  // WarbleServer is a backend class for func_server so testing it this way is
  // appropriate. Testing the associated KeyValueStoreClient, however, is better
  // done through a special testing class which inherits a common interface
  // with KeyValueStoreClient. I did not implement it that way because it would
  // mean refactoring the entire exiting service with this new interface which
  // does not seem worth the tradeoff for a testing only a single extra feature.
  WarbleServer* warble_server_;
};

// test that all hashtags in a single warble are identified
TEST_F(WarbleServerStreamTest, GetHashtagsGetsAllHashtags) {
  Warble warble;
  warble.set_text("Should identify #this hashtag as well as #that hashtag");
  std::vector<std::string> hashtags = warble_server_->GetHashtags(warble);

  std::unordered_set<std::string> s_hashtags;
  for (auto const& s : hashtags) {
    s_hashtags.insert(s);
  }
  EXPECT_EQ((s_hashtags.find("#this") != s_hashtags.end()), true);
  EXPECT_EQ((s_hashtags.find("#that") != s_hashtags.end()), true);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
