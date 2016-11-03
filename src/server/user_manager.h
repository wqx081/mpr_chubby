#ifndef MPR_CHUBBY_SERVER_USER_MANAGER_H_
#define MPR_CHUBBY_SERVER_USER_MANAGER_H_

#include <string>
#include <unordered_map>
#include "base/platform/mutex.h"
#include "proto/service.pb.h"
#include "storage/meta.h"
#include <leveldb/db.h>

namespace mpr {
namespace chubby {

class UserManager {
 public:
  UserManager(const std::string& db_path, const UserInfo& root);
  virtual ~UserManager() {}

  base::Status Login(const std::string& name, 
                     const std::string& password, 
                     const std::string& uuid);
  base::Status Logout(const std::string& uuid);
  base::Status Register(const std::string& name, const std::string& password);
  base::Status ForceOffline(const std::string& myid, const std::string& name);
  base::Status DeleteUser(const std::string& myid, const std::string& name);

  bool IsLoggedIn(const std::string& uuid);
  bool IsValidUser(const std::string& myid);

  base::Status TruncateOnlineUsers(const std::string& myid);
  base::Status TruncateAllUsers(const std::string& myid);

  std::string GetUsernameFromUUID(const std::string& uuid);
  
  static std::string CalculateUUID(const std::string& name);

 private:
  bool DoWriteToDatabase(const UserInfo& user);
  bool DoWriteToDatabase(const std::string& name, const std::string& password);
  bool DoDeleteUserFromDatabase(const std::string& name);
  bool DoTruncateDatabase();
  bool DoRecoverFromDatabase();

 private:
  base::mutex mu_;
  std::unordered_map<std::string, std::string> logged_users_;
  std::unordered_map<std::string, UserInfo> user_list_;
  std::string db_path_;
  leveldb::DB* db_;
};

} // namespace chubby
} // namespace mpr
#endif // MPR_CHUBBY_SERVER_USER_MANAGER_H_
