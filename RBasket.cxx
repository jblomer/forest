#include "RBasket.hxx"

namespace Toy {

RBasket::RBasket(std::size_t capacity) : fCapacity(capacity), fSize(0) {
   assert(fCapacity > 0);
   fBuffer = static_cast<unsigned char *>(std::malloc(fCapacity));
   assert(fBuffer);
}

RBasket::~RBasket() {
   std::free(fBuffer);
}



}  // namespace Toy
