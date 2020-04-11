//
// Created by Ruimin Zeng on 2/19/20.
//

#include "store.h"

#include <gtest/gtest.h>

#include <iostream>

// unit test of class Store
class StoreTest : public ::testing::Test {
 protected:
  void SetUp() override {
    store_->Put("id", "zzfftt007");
    store_->Put("name", "Alice");
    store_->Put(
        "warble",
        "Nice weather! I went hiking at Malibu and enjoy my fantastic photos.");
  }
  std::optional<std::string> filename = std::nullopt;
  std::unique_ptr<Store> store_ = std::make_unique<Store>(filename);
};

// test constructor of class Store
TEST_F(StoreTest, constructorTest) {
  std::ofstream fout;
  fout.open("test_input.txt");
  fout << "users:"
       << "alice bob cindy" << std::endl;
  fout << "followers:"
       << "raymond june" << std::endl;
  fout << "warble1:"
       << "hope coronavirus disappear asap" << std::endl;
  std::optional<std::string> filename = std::make_optional("test_input.txt");
  std::unique_ptr<Store> store = std::make_unique<Store>(filename);
  ASSERT_EQ("alice bob cindy", store->Get("users"));
  ASSERT_EQ("raymond june", store->Get("followers"));
  ASSERT_EQ("hope coronavirus disappear asap", store->Get("warble1"));
  std::remove("test_input.txt");
}

// test get method of class Store
TEST_F(StoreTest, getTest) {
  ASSERT_EQ("zzfftt007", store_->Get("id"));
  ASSERT_EQ("Alice", store_->Get("name"));
  ASSERT_EQ(
      "Nice weather! I went hiking at Malibu and enjoy my fantastic photos.",
      store_->Get("warble"));
  ASSERT_EQ("", store_->Get(
                    "Hi"));  // when key not exist, an empty string is returned
}

// test put method of class Store
TEST_F(StoreTest, putTest) {
  bool res1 = store_->Put("foo1", "bar1");
  bool res2 = store_->Put("foo2", "bar2");
  bool res3 = store_->Put("foo3", "bar3");
  ASSERT_TRUE(res1);
  ASSERT_TRUE(res2);
  ASSERT_TRUE(res3);
  ASSERT_EQ("bar1", store_->Get("foo1"));
  ASSERT_EQ("bar2", store_->Get("foo2"));
  ASSERT_EQ("bar3", store_->Get("foo3"));
  res1 = store_->Put("foo1", "bar1");
  res2 = store_->Put("foo2", "bar2");
  res3 = store_->Put("foo3", "bar3");
  ASSERT_FALSE(res1);
  ASSERT_FALSE(res2);
  ASSERT_FALSE(res3);
}

// test remove method of class Store
TEST_F(StoreTest, removeTest) {
  bool idRemoved = store_->Remove("id");
  bool nameRemoved = store_->Remove("name");
  bool warbleRemoved = store_->Remove("warble");
  ASSERT_TRUE(idRemoved);
  ASSERT_TRUE(nameRemoved);
  ASSERT_TRUE(warbleRemoved);
  ASSERT_EQ("", store_->Get("id"));
  ASSERT_EQ("", store_->Get("name"));
  ASSERT_EQ("", store_->Get("warble"));
  idRemoved = store_->Remove("id");
  nameRemoved = store_->Remove("name");
  warbleRemoved = store_->Remove("warble");
  ASSERT_FALSE(idRemoved);
  ASSERT_FALSE(nameRemoved);
  ASSERT_FALSE(warbleRemoved);
}

// test dumpStoreToFile method of class Store
TEST_F(StoreTest, dumpStoreToFileTest) {
  std::optional<std::string> filename = std::make_optional("test_output.txt");
  store_->dumpStoreToFile(filename);
  std::unique_ptr<Store> store = std::make_unique<Store>(filename);
  ASSERT_EQ("zzfftt007", store->Get("id"));
  ASSERT_EQ("Alice", store->Get("name"));
  ASSERT_EQ(
      "Nice weather! I went hiking at Malibu and enjoy my fantastic photos.",
      store->Get("warble"));
  std::remove("test_output.txt");
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
