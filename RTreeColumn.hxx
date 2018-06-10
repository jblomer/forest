#ifndef RTREECOLUMN_H_
#define RTREECOLUMN_H_

#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "RBasket.hxx"
#include "RTreeColumnModel.hxx"
#include "RTreeElement.hxx"

namespace Toy {

class RTreeSource;
class RTreeSink;

class RTreeColumn {
   RTreeColumnModel fModel;
   RTreeSource *fSource;
   RTreeSink* fSink;
   std::unique_ptr<RBasket> fHeadBasket;
   std::uint64_t fMaxElement;

   std::unique_ptr<RBasket> fCurrentSlice;
   std::int64_t fCurrentSliceStart;
   std::int64_t fCurrentSliceEnd;

   void ShipHeadBasket();
   void MapSlice(std::uint64_t num);

public:
   static const unsigned kDefaultNumElements = 10000;

   RTreeColumn(const RTreeColumnModel &model,
               RTreeSource *source, RTreeSink *sink);

   RTreeColumnModel GetModel() { return fModel; }
   RTreeColumnType GetColumnType() { return fModel.GetType(); }

   void Append(const RTreeElementBase &element) {
     //std::cout << "appending to " << fModel.GetName()
     //          << "(max: " << fMaxElement << ")" << std::endl;
     assert(element.GetColumnType() == fModel.GetType());
     void *dst = fHeadBasket->Reserve(element.GetSize());
     if (dst == nullptr) {
       ShipHeadBasket();
       dst = fHeadBasket->Reserve(element.GetSize());
       assert(dst != nullptr);
     }
     element.Serialize(dst);
     fMaxElement++;
     fHeadBasket->Release();
   }

   void Flush() {
     //std::cout << "flushing head basket" << std::endl;
     if (fMaxElement > fHeadBasket->GetRangeStart())
       ShipHeadBasket();
   }


   void Read(const std::int64_t num, RTreeElementBase *element) {
     if ((num < fCurrentSliceStart) || (num > fCurrentSliceEnd)) {
       MapSlice(num);
       std::cout << "Mapped slice [" << fCurrentSliceStart << "-"
                 << fCurrentSliceEnd << "] for element " << num
                 << std::endl;
     }
     void *buf = reinterpret_cast<unsigned char *>(fCurrentSlice->GetBuffer())
                 + (num - fCurrentSliceStart) * element->GetSize();
     element->Deserialize(buf);
   }
};  // RTreeColumn

using RTreeColumnCollection = std::vector<RTreeColumn*>;

}  // namespace Toy

#endif  // RTREECOLUMN_H_