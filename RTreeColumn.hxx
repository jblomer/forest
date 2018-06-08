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
   std::uint64_t fMaxElement;

   void ShipHeadBasket();

public:
   static const unsigned kDefaultNumElements = 10000;

   RTreeColumn(const RTreeColumnModel &model, RTreeSink *sink);

   RTreeColumnModel GetModel() { return fModel; }
   RTreeColumnType GetColumnType() { return fModel.GetType(); }

   void Append(const RTreeElementBase &element) {
     //std::cout << "appending to " << fModel.GetName() << std::endl;
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
};  // RTreeColumn

using RTreeColumnCollection = std::vector<RTreeColumn*>;

}  // namespace Toy

#endif  // RTREECOLUMN_H_