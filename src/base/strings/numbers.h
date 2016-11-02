#ifndef BASE_STRINGS_NUMBERS_H_
#define BASE_STRINGS_NUMBERS_H_

#include <string>

#include "base/stringpiece.h"
#include "base/port.h"

namespace base {
namespace strings {

static const int kFastToBufferSize = 32;

char* FastInt32ToBufferLeft(int32 i, char* buffer);    // at least 12 bytes
char* FastUInt32ToBufferLeft(uint32 i, char* buffer);  // at least 12 bytes
char* FastInt64ToBufferLeft(int64 i, char* buffer);    // at least 22 bytes
char* FastUInt64ToBufferLeft(uint64 i, char* buffer);  // at least 22 bytes

char* DoubleToBuffer(double i, char* buffer);
char* FloatToBuffer(float i, char* buffer);

string FpToString(Fprint fp);
bool StringToFp(const string& s, Fprint* fp);

StringPiece Uint64ToHexString(uint64 v, char* buf);
bool HexStringToUint64(const StringPiece& s, uint64* v);

bool safe_strto32(StringPiece str, int32* value);
bool safe_strtou32(StringPiece str, uint32* value);

bool safe_strto64(StringPiece str, int64* value);
bool safe_strtou64(StringPiece str, uint64* value);

bool safe_strtof(const char* str, float* value);
bool safe_strtod(const char* str, double* value);

string HumanReadableNum(int64 value);
string HumanReadableNumBytes(int64 num_bytes);
string HumanReadableElapsedTime(double seconds);

}  // namespace strings
}  // namespace base

#endif  // BASE_STRINGS_NUMBERS_H_
