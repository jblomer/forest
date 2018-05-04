#ifndef RBASKET_H_
#define RBASKET_H_

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <shared_mutex>

namespace Toy {

class RBasket {
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
      fLock.lock_shared();
      size_t pos = fSize.fetch_add(nbyte);

      if (pos + nbyte > fCapacity) {
        fLock.unlock_shared();
        return nullptr;
      }

      return fBuffer + pos;
   }
   void Release() const { fLock.unlock_shared(); }

   void Freeze() {
      fLock.lock();
   }
   void Reset() {
     fSize = 0;
     fLock.unlock();
   }
};  // class RBasket

}  // namespace Toy

#endif  // RBASKET_H_
