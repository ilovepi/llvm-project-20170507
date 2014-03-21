/*===- InstrProfilingBuffer.c - Write instrumentation to a memory buffer --===*\
|*
|*                     The LLVM Compiler Infrastructure
|*
|* This file is distributed under the University of Illinois Open Source
|* License. See LICENSE.TXT for details.
|*
\*===----------------------------------------------------------------------===*/

#include "InstrProfiling.h"
#include <string.h>

uint64_t __llvm_profile_get_size_for_buffer(void) {
  /* Match logic in __llvm_profile_write_buffer(). */
  return sizeof(uint64_t) * PROFILE_HEADER_SIZE +
     PROFILE_RANGE_SIZE(data) * sizeof(__llvm_profile_data) +
     PROFILE_RANGE_SIZE(counters) * sizeof(uint64_t) +
     PROFILE_RANGE_SIZE(names) * sizeof(char);
}

int __llvm_profile_write_buffer(char *Buffer) {
  /* Match logic in __llvm_profile_get_size_for_buffer().
   * Match logic in __llvm_profile_write_file().
   */
  const __llvm_profile_data *DataBegin = __llvm_profile_data_begin();
  const __llvm_profile_data *DataEnd = __llvm_profile_data_end();
  const uint64_t *CountersBegin = __llvm_profile_counters_begin();
  const uint64_t *CountersEnd   = __llvm_profile_counters_end();
  const char *NamesBegin = __llvm_profile_names_begin();
  const char *NamesEnd   = __llvm_profile_names_end();

  /* Calculate size of sections. */
  const uint64_t DataSize = DataEnd - DataBegin;
  const uint64_t CountersSize = CountersEnd - CountersBegin;
  const uint64_t NamesSize = NamesEnd - NamesBegin;

  /* Create the header. */
  uint64_t Header[PROFILE_HEADER_SIZE] = {
    __llvm_profile_get_magic(),
    __llvm_profile_get_version(),
    DataSize,
    CountersSize,
    NamesSize,
    (uintptr_t)CountersBegin,
    (uintptr_t)NamesBegin
  };

  /* Write the data. */
#define UPDATE_memcpy(Data, Size) \
  do {                            \
    memcpy(Buffer, Data, Size);   \
    Buffer += Size;               \
  } while (0)
  UPDATE_memcpy(Header,  PROFILE_HEADER_SIZE * sizeof(uint64_t));
  UPDATE_memcpy(DataBegin,     DataSize      * sizeof(__llvm_profile_data));
  UPDATE_memcpy(CountersBegin, CountersSize  * sizeof(uint64_t));
  UPDATE_memcpy(NamesBegin,    NamesSize     * sizeof(char));
#undef UPDATE_memcpy

  return 0;
}
