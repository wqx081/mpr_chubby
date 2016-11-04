#ifndef BASE_IO_INPUTSTREAM_INTERFACE_H_
#define BASE_IO_INPUTSTREAM_INTERFACE_H_

#include <string>
#include "base/status.h"
#include "base/port.h"


namespace base {
namespace io {

// \brief 输入流操作接口
//
class InputStreamInterface {
 public:
  InputStreamInterface() {}
  virtual ~InputStreamInterface() {}

  // \brief 从文件中读取 bytes_to_read
  // \param bytes_to_read 要读取的字节数目
  // \param result 读取的结果保存到 result
  // \return OK 表示成功, OUT_OF_RANGE 表示要读取流的大小不足 bytes_to_read
  virtual Status ReadNBytes(int64 bytes_to_read, string* result) = 0;

  // \brief 跳过bytes_to_skip 字节
  // \param bytes_to_skip 必须 >= 0
  // \return OK 表示成功, OUT_OF_RANGE 表示要读取流的大小不足 bytes_to_skip
  virtual Status SkipNBytes(int64 bytes_to_skip);

  // \brief 返回当前相对于文件起始位置的字节偏移, 如果返回-1, 表示出错
  virtual int64 Tell() const = 0;

  // \brief 重置流回到起始位置
  virtual Status Reset() = 0;
};

}  // namespace io
}  // namespace base
#endif  // BASE_IO_INPUTSTREAM_INTERFACE_H_
