#ifndef BASE_PLATFORM_MEM_H_
#define BASE_PLATFORM_MEM_H_

#include <stdint.h>
#include <stddef.h>

namespace base {

void* aligned_malloc(size_t size, int minimum_alignment=16);
void aligned_free(void* aligned_memory);

} // namespace base 

#endif // BASE_PLATFORM_MEM_H_
