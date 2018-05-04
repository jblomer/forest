#include "RTree.hxx"

#include <cassert>
#include <iterator>
#include <iostream>
#include <utility>

#include "RBasket.hxx"
#include "RTreeEntry.hxx"
#include "RTreeMedium.hxx"
#include "RTreeModel.hxx"

namespace Toy {

RTree::RTree(std::shared_ptr<RTreeModel> model, std::unique_ptr<RTreeSink> sink)
   : fSink(std::move(sink))
   , fModel(model)
{
   fModel->Freeze();
   // Create branches from model
   for (auto branch_model : fModel->GetBranchModelsRef()) {
      fBranches.emplace_back(std::make_unique<RBranch>(branch_model));
   }

   fSink->Attach(this);
   fSink->OnCreate();
}


void RTree::Fill(RTreeEntry *entry) {
   assert(entry);
   assert(entry->IsCompatibleWith(fModel.get()));

   auto iter_leafs = entry->GetLeafsRef().begin();
   auto iter_leafs_end = entry->GetLeafsRef().end();
   auto iter_branches = fBranches.begin();
   for (; iter_leafs != iter_leafs_end; ++iter_leafs, ++iter_branches) {
      size_t nbytes = (*iter_leafs)->GetSize();
      void *dst = (*iter_branches)->Reserve(nbytes);
      if (dst == nullptr) {
          RBasket *basket = (*iter_branches)->SealHead();
          fSink->OnFullBasket(basket);
          (*iter_branches)->ResetHead();
          dst = (*iter_branches)->Reserve(nbytes);
          assert(dst != nullptr);
      }
      (*iter_leafs)->Serialize(dst);
      (*iter_branches)->Release();
   }

   /*for (auto&& ptr_leaf : entry->GetLeafsRef()) {
     ptr_leaf->GetSize();
   }*/

   //for (auto&& branch : fBranches) {
   //  branch->Reserve(5);
   //}
   /*for (auto&& ptr_leaf : entry->GetLeafsRef()) {
     ptr_leaf->GetSizeDisk();
   }*/
   //for (auto iter = std::begin()) {
   //}

   // Iterate through entry leafes and serialize into basket buffers of
   // branches

   // Flush buffers
   // Checkpoint (Mini Footer)
}

}  // namespace Toy
