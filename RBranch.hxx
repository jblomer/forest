#ifndef RBRANCH_H_
#define RBRANCH_H_

#include "RBasket.hxx"
#include "RBranchModel.hxx"

namespace Toy {

class RBranch {
   RBranchModel fModel;
   unsigned fClusterSize;
   RBasket fBasketHead;

public:
   static const unsigned kDefaultClusterSize = 100;

   RBranch(
     const RBranchModel &model,
     unsigned cluster_size = kDefaultClusterSize)
     : fModel(model)
     , fClusterSize(cluster_size)
     , fBasketHead(fModel.GetElementSize() * fClusterSize)
   { }

   RBranchModel GetModel() { return fModel; }
};

}  // namespace Toy


#endif  // RBRANCH_H_
