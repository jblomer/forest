#ifndef RBASKET_H_
#define RBASKET_H_

#include <cassert>
#include <cstddef>
#include <cstdlib>

namespace Toy {

class RBasket {
   std::size_t fCapacity;
   std::size_t fSize;
   unsigned char *fBuffer;

public:
   RBasket(std::size_t capacity);
   // TODO: copy, move
   ~RBasket();

   std::size_t GetCapacity() { return fCapacity; }
   std::size_t GetSize() { return fSize; }
   void *GetBuffer() { return fBuffer; }

   void *Reserve(std::size_t nbyte) {
      while (fSize + nbyte > fCapacity) {
        fBuffer = static_cast<unsigned char *>(
          std::realloc(fBuffer, 2 * fCapacity));
        assert(fBuffer);
        fCapacity *= 2;
      }
      void *ptr = fBuffer + fSize;
      fSize += nbyte;
      return ptr;
   }

   void Reset() { fSize = 0; }
};  // class RBasket

}  // namespace Toy

#endif  // RBASKET_H_
