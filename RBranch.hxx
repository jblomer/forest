#ifndef RBRANCH_H_
#define RBRANCH_H_

#include <cstddef>
#include <memory>

#include "RBasket.hxx"
#include "RBranchModel.hxx"

namespace Toy {

class RBranch {
   RBranchModel fModel;
   unsigned fNumElements;
   std::unique_ptr<RBasket> fBasketHead;

public:
   static const unsigned kDefaultNumElements = 100;

   RBranch(
     const RBranchModel &model,
     unsigned num_elements = kDefaultNumElements)
     : fModel(model)
     , fNumElements(num_elements)
     , fBasketHead(new RBasket(fModel.GetElementSize() * fNumElements))
   { }

   void *Reserve(unsigned nbyte) const {
     return fBasketHead->Reserve(nbyte);
   }

   void Release() { fBasketHead->Release(); }

   RBasket* SealHead() {
      fBasketHead->Freeze();
      return fBasketHead.get();
   }

   void ResetHead() {
      fBasketHead->Reset();
   }
};

}  // namespace Toy


#endif  // RBRANCH_H_
