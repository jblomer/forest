#ifndef RCOLUMN_SLICE_H_
#define RCOLUMN_SLICE_H_

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <shared_mutex>

namespace Toy {

class RColumnSlice {
   unsigned char *fBuffer;
   std::size_t fCapacity;
   std::size_t fSize;
   std::uint64_t fRangeStart;

public:
   RColumnSlice(std::size_t capacity, std::uint64_t range_start);
   // TODO: copy, move
   ~RColumnSlice();

   std::size_t GetCapacity() const { return fCapacity; }
   std::size_t GetSize() const { return fSize; }
   void *GetBuffer() { return fBuffer; }
   std::uint64_t GetRangeStart() { return fRangeStart; }

   void *Reserve(std::size_t nbyte) {
      size_t pos = fSize;

      if (pos + nbyte > fCapacity) {
        return nullptr;
      }
      fSize += nbyte;

      return fBuffer + pos;
   }
   void Release() const {  }

   void Freeze() {
   }
   void Reset(uint64_t range_start) {
     fSize = 0;
     fRangeStart = range_start;
   }
};  // class RColumnSlice

}  // namespace Toy

#endif  // RCOLUMN_SLICE_H_
