#include "storage/meta.h"

#include "base/logging.h"
#include "server/user_manager.h"
#include "base/io/path.h"

namespace mpr {
namespace chubby {

const std::string kTermFileName("term.data");
const std::string kVoteFileName("vote.data");
const std::string kRootFileName("root.data");

Meta::Meta(const std::string& db_path)
    : db_path_(db_path) {
  base::Status status = base::Env::Default()->CreateDirectory(db_path_);
  DCHECK(status.ok() || status.code() == base::error::ALREADY_EXISTS) << "Failed to create directory: " << db_path_;

  std::unique_ptr<base::WritableFile> term_file;
  std::unique_ptr<base::WritableFile> vote_file;
  std::unique_ptr<base::WritableFile> root_file;
  
  DCHECK(base::Env::Default()->NewAppendableFile(base::io::JoinPath(db_path_, kTermFileName), &term_file)
         .ok()) << "Failed to create appendable file: " << base::io::JoinPath(db_path_, kTermFileName);
  DCHECK(base::Env::Default()->NewAppendableFile(base::io::JoinPath(db_path_, kVoteFileName), &vote_file)
         .ok()) << "Failed to create appendable file: " << base::io::JoinPath(db_path_, kVoteFileName);
  DCHECK(base::Env::Default()->NewWritableFile(base::io::JoinPath(db_path_, kRootFileName), &root_file)
         .ok()) << "Failed to create appendable file: " << base::io::JoinPath(db_path_, kRootFileName);
  term_file_ = std::move(term_file);
  vote_file_ = std::move(vote_file);
  root_file_ = std::move(root_file);
}

int64_t Meta::ReadCurrentTerm() {
  int64_t current_term = 0, tmp = 0;
  //while (term_file_->
}

} // namespace chubby
} // namespace mpr
