#ifndef RBASKET_H_
#define RBASKET_H_

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <shared_mutex>

namespace Toy {

class RBasket {
   unsigned char *fBuffer;
   std::size_t fCapacity;
   std::size_t fSize;
   std::uint64_t fRangeStart;

public:
   RBasket(std::size_t capacity, std::uint64_t range_start);
   // TODO: copy, move
   ~RBasket();

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
};  // class RBasket

}  // namespace Toy

#endif  // RBASKET_H_
