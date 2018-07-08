#include "RColumnSlice.hxx"

namespace Toy {

RColumnSlice::RColumnSlice(std::size_t capacity, std::uint64_t range_start)
   : fCapacity(capacity)
   , fSize(0)
   , fRangeStart(range_start)
{
   assert(fCapacity > 0);
   fBuffer = static_cast<unsigned char *>(std::malloc(fCapacity));
   assert(fBuffer);
}

RColumnSlice::~RColumnSlice() {
   std::free(fBuffer);
}



}  // namespace Toy
