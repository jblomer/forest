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

class RTreeSink;

class RTreeColumn {
   RTreeColumnModel fModel;
   RTreeSink* fSink;
   std::unique_ptr<RBasket> fHeadBasket;

   void ShipHeadBasket();

public:
   static const unsigned kDefaultNumElements = 10000;

   RTreeColumn(const RTreeColumnModel &model, RTreeSink *sink)
     : fModel(model)
     , fSink(sink)
     , fHeadBasket(std::make_unique<RBasket>(
         4 /*TODO*/ * kDefaultNumElements))
   {
     assert(fSink);
   }

   RTreeColumnType GetColumnType() { return fModel.GetType(); }

   void Append(const RTreeElementBase &element) {
     assert(element.GetColumnType() == fModel.GetType());
     void *dst = fHeadBasket->Reserve(element.GetSize());
     if (dst == nullptr) {
       ShipHeadBasket();
       dst = fHeadBasket->Reserve(element.GetSize());
       assert(dst != nullptr);
     }
     element.Serialize(dst);
     fHeadBasket->Release();
   }
};  // RTreeColumn

using RTreeColumnCollection = std::vector<RTreeColumn*>;

}  // namespace Toy

#endif  // RTREECOLUMN_H_