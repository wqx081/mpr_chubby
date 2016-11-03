#ifndef MPR_CHUBBY_STORAGE_META_H_
#define MPR_CHUBBY_STORAGE_META_H_

#include <map>
#include "base/platform/env.h"
#include "proto/service.pb.h"

namespace mpr {
namespace chubby {

class UserManager;

class Meta {
 public:
  Meta(const std::string& db_path);
  ~Meta();
  
  int64_t ReadCurrentTerm();
  void ReadVotedFor(std::map<int64_t, std::string>& voted_for);
  UserInfo ReadRootInfo();
  void WriteCurrentTerm(int64_t term);
  void WriteVotedFor(int64_t term, const std::string& server_id);
  void WriteRootInfo(const UserInfo& root);

 private:
  std::string db_path_;
  std::unique_ptr<base::WritableFile> term_file_;
  std::unique_ptr<base::WritableFile> vote_file_;
  std::unique_ptr<base::WritableFile> root_file_;

  DISALLOW_COPY_AND_ASSIGN(Meta);
};

} // namespace chubby
} // namespace mpr
#endif // MPR_CHUBBY_STORAGE_META_H_
