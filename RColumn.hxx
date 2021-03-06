#ifndef RCOLUMN_H_
#define RCOLUMN_H_

#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "RColumnSlice.hxx"
#include "RColumnModel.hxx"
#include "RColumnElement.hxx"

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

namespace Toy {

class RColumnSource;
class RColumnSink;

class RColumn {
   RColumnModel fModel;
   RColumnSource *fSource;
   RColumnSink* fSink;
   std::unique_ptr<RColumnSlice> fHeadSlice;
   std::uint64_t fMaxElement;

   std::unique_ptr<RColumnSlice> fCurrentSlice;
   std::int64_t fCurrentSliceStart;
   std::int64_t fCurrentSliceEnd;

   void ShipHeadSlice();
   void MapSlice(std::uint64_t num);

public:
   static const unsigned kDefaultNumElements = 10000;

   RColumn(const RColumnModel &model,
           RColumnSource *source, RColumnSink *sink);

   RColumnModel GetModel() { return fModel; }
   RColumnType GetColumnType() { return fModel.GetType(); }
   std::string GetName() { return fModel.GetName(); }

   void Append(const RColumnElementBase &element) {
     //std::cout << "appending to " << fModel.GetName()
     //          << "(max: " << fMaxElement << ")" << std::endl;
     //assert(element.GetColumnType() == fModel.GetType());
     void *dst = fHeadSlice->Reserve(element.GetSize());
     if (dst == nullptr) {
       ShipHeadSlice();
       dst = fHeadSlice->Reserve(element.GetSize());
       assert(dst != nullptr);
     }
     element.Serialize(dst);
     fMaxElement++;
     fHeadSlice->Release();
   }

   void Flush() {
     //std::cout << "flushing head basket" << std::endl;
     if (fMaxElement > fHeadSlice->GetRangeStart())
       ShipHeadSlice();
   }


   void Read(const std::int64_t num, RColumnElementBase *__restrict__ element) {
     if ((num < fCurrentSliceStart) || (num > fCurrentSliceEnd)) {
       MapSlice(num);
       //std::cout << "Mapped slice [" << fCurrentSliceStart << "-"
       //          << fCurrentSliceEnd << "] for element " << num
       //          << std::endl;
     }
     void *buf = reinterpret_cast<unsigned char *>(fCurrentSlice->GetBuffer())
                 + (num - fCurrentSliceStart) * element->GetSize();
     element->Deserialize(buf);
   }


   void ReadV(std::int64_t start, std::uint64_t num, void *dst)
   {
     if ((start < fCurrentSliceStart) || (start > fCurrentSliceEnd)) {
       MapSlice(start);
       //std::cout << "Mapped slice [" << fCurrentSliceStart << "-"
       //          << fCurrentSliceEnd << "] for element " << num
       //          << std::endl;
     }
     void *buf = reinterpret_cast<unsigned char *>(fCurrentSlice->GetBuffer())
                 + (start - fCurrentSliceStart) * fModel.GetElementSize();
     // TODO: what about RTreeElement?
     if (likely(fModel.GetType() == RColumnType::kFloat))
       memcpy(dst, buf, num * fModel.GetElementSize());
   }

   std::uint64_t GetNElements() { return fMaxElement; }
};  // RColumn

using RColumnCollection = std::vector<RColumn*>;

}  // namespace Toy

#endif  // RCOLUMN_H_