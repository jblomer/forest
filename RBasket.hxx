#ifndef RBASKET_H_
#define RBASKET_H_

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <shared_mutex>

namespace Toy {

class RBasket {
   bool fIsThreadsafe;
   unsigned char *fBuffer;
   std::size_t fCapacity;
   mutable std::atomic<std::size_t> fSize;
   mutable std::shared_mutex fLock;

public:
   RBasket(std::size_t capacity);
   // TODO: copy, move
   ~RBasket();

   std::size_t GetCapacity() const { return fCapacity; }
   std::size_t GetSize() const { return fSize; }
   void *GetBuffer() { return fBuffer; }

   void *Reserve(std::size_t nbyte) const {
      if (fIsThreadsafe) fLock.lock_shared();
      size_t pos = fSize.fetch_add(nbyte);

      if (pos + nbyte > fCapacity) {
        if (fIsThreadsafe) fLock.unlock_shared();
        return nullptr;
      }

      return fBuffer + pos;
   }
   void Release() const { if (fIsThreadsafe) fLock.unlock_shared(); }

   void Freeze() {
      if (fIsThreadsafe) fLock.lock();
   }
   void Reset() {
     fSize = 0;
     if (fIsThreadsafe) fLock.unlock();
   }
};  // class RBasket

}  // namespace Toy

#endif  // RBASKET_H_
