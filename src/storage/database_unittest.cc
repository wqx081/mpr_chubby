#include <gtest/gtest.h>
#include "storage/database.h"
#include "base/status_test_util.h"
#include "proto/service.pb.h"

namespace mpr {
namespace chubby {

TEST(Database, OpenAndClose) {
  Database database("/tmp/storage_test");

  EXPECT_TRUE(database.Open("user1"));
  EXPECT_TRUE(database.Open("user2"));
  EXPECT_TRUE(database.Open("user1"));
  database.Close("user1");
  database.Close("user2");
  database.Close("user3");
  database.Close("user2");
}

TEST(Database, Operations) {
  Database database("/tmp/storage_test2");
  EXPECT_TRUE(database.Open("user1"));
  std::string value;
  MPR_EXPECT_OK(database.Put(Database::kAnonymousUser, "Hello", "World"));      
  MPR_EXPECT_OK(database.Get(Database::kAnonymousUser, "Hello", &value));
  EXPECT_EQ(value, "World");

  MPR_EXPECT_OK(database.Put("user1", "Name", "User1"));
  MPR_EXPECT_OK(database.Get("user1", "Name", &value));
  EXPECT_EQ(value, "User1");

  // Not found case
  base::Status status = database.Get(Database::kAnonymousUser, "Name", &value);
  EXPECT_EQ(base::error::NOT_FOUND, status.code());

  // Key Not existed
  status =  database.Get(Database::kAnonymousUser, "NotExistedKey", &value);
  EXPECT_EQ(base::error::NOT_FOUND, status.code());

  // UnLogged User
  status = database.Get("UnLoggedUser", "Hello", &value);
  EXPECT_EQ(base::error::NOT_FOUND, status.code());

  // Delete
  MPR_EXPECT_OK(database.Delete(Database::kAnonymousUser, "NotExistedKey"));
  MPR_EXPECT_OK(database.Delete(Database::kAnonymousUser, "Hello"));
  status = database.Get(Database::kAnonymousUser, "Hello", &value);
  EXPECT_EQ(base::error::NOT_FOUND, status.code());

  database.Close("user1");
}

TEST(Database, Iterator) {
  Database database("/tmp/storage_test3");
  EXPECT_TRUE(database.Open("user1"));

  const std::string value_string("value");
  std::set<std::string> default_value;
  std::set<std::string> user1_value;
  base::Status status;
  // Insert
  for (int i=0; i < 10; ++i) {
    std::string value = value_string + std::to_string(i);
    default_value.insert(value);
    MPR_EXPECT_OK(database.Put(Database::kAnonymousUser, std::to_string(i), value));
  }
  for (int i=100; i < 110; ++i) {
    std::string value = value_string + std::to_string(i);
    user1_value.insert(value);
    MPR_EXPECT_OK(database.Put("user1", std::to_string(i), value));
  }
  Database::Iterator* it = database.NewIterator(Database::kAnonymousUser); 
  for (it->Seek("0"); it->Valid(); it->Next()) {
    MPR_EXPECT_OK(it->status());
    default_value.erase(it->value());
  }
  EXPECT_TRUE(default_value.empty());
  delete it;
  
  it = database.NewIterator("user1");
  for (it->Seek("100"); it->Valid(); it->Next()) {
    MPR_EXPECT_OK(it->status());
    user1_value.erase(it->value());
  }
  EXPECT_TRUE(user1_value.empty());
  delete it;
  database.Close("user1");
}

} // namespace chubby
} // namespace mpr
