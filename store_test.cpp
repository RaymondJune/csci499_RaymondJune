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
    store_.Put("id", "zzfftt007");
    store_.Put("name", "Alice");
    store_.Put(
        "warble",
        "Nice weather! I went hiking at Malibu and enjoy my fantastic photos.");
  }

  Store store_;
};

// test get method of class Store
TEST_F(StoreTest, getTest) {
  ASSERT_EQ("zzfftt007", store_.Get("id"));
  ASSERT_EQ("Alice", store_.Get("name"));
  ASSERT_EQ(
      "Nice weather! I went hiking at Malibu and enjoy my fantastic photos.",
      store_.Get("warble"));
  ASSERT_EQ(
      "", store_.Get("Hi"));  // when key not exist, an empty string is returned
}

// test put method of class Store
TEST_F(StoreTest, putTest) {
  bool res1 = store_.Put("foo1", "bar1");
  bool res2 = store_.Put("foo2", "bar2");
  bool res3 = store_.Put("foo3", "bar3");
  ASSERT_TRUE(res1);
  ASSERT_TRUE(res2);
  ASSERT_TRUE(res3);
  ASSERT_EQ("bar1", store_.Get("foo1"));
  ASSERT_EQ("bar2", store_.Get("foo2"));
  ASSERT_EQ("bar3", store_.Get("foo3"));
  res1 = store_.Put("foo1", "bar1");
  res2 = store_.Put("foo2", "bar2");
  res3 = store_.Put("foo3", "bar3");
  ASSERT_FALSE(res1);
  ASSERT_FALSE(res2);
  ASSERT_FALSE(res3);
}

// test remove method of class Store
TEST_F(StoreTest, removeTest) {
  bool idRemoved = store_.Remove("id");
  bool nameRemoved = store_.Remove("name");
  bool warbleRemoved = store_.Remove("warble");
  ASSERT_TRUE(idRemoved);
  ASSERT_TRUE(nameRemoved);
  ASSERT_TRUE(warbleRemoved);
  ASSERT_EQ("", store_.Get("id"));
  ASSERT_EQ("", store_.Get("name"));
  ASSERT_EQ("", store_.Get("warble"));
  idRemoved = store_.Remove("id");
  nameRemoved = store_.Remove("name");
  warbleRemoved = store_.Remove("warble");
  ASSERT_FALSE(idRemoved);
  ASSERT_FALSE(nameRemoved);
  ASSERT_FALSE(warbleRemoved);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
