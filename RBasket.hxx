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
   bool fIsThreadsafe;
   unsigned char *fBuffer;
   std::size_t fCapacity;
   mutable std::atomic<std::size_t> fSize;
   std::uint64_t fRangeStart;
   mutable std::shared_mutex fLock;

public:
   RBasket(std::size_t capacity, std::uint64_t range_start);
   // TODO: copy, move
   ~RBasket();

   std::size_t GetCapacity() const { return fCapacity; }
   std::size_t GetSize() const { return fSize; }
   void *GetBuffer() { return fBuffer; }
   std::uint64_t GetRangeStart() { return fRangeStart; }

   void *Reserve(std::size_t nbyte) const {
      if (fIsThreadsafe) fLock.lock_shared();
      size_t pos = fSize.fetch_add(nbyte);

      if (pos + nbyte > fCapacity) {
        fSize -= nbyte;
        if (fIsThreadsafe) fLock.unlock_shared();
        return nullptr;
      }

      return fBuffer + pos;
   }
   void Release() const { if (fIsThreadsafe) fLock.unlock_shared(); }

   void Freeze() {
      if (fIsThreadsafe) fLock.lock();
   }
   void Reset(uint64_t range_start) {
     fSize = 0;
     fRangeStart = range_start;
     if (fIsThreadsafe) fLock.unlock();
   }
};  // class RBasket

}  // namespace Toy

#endif  // RBASKET_H_
