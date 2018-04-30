#include "RTree.hxx"

#include <cassert>
#include <iterator>
#include <iostream>

#include "RTreeEntry.hxx"
#include "RTreeModel.hxx"

namespace Toy {

RTree::RTree(std::shared_ptr<RTreeModel> model) : fModel(model) {
   fModel->Freeze();
   // Create branches from model
   for (auto branch_model : fModel->GetBranchModelsRef()) {
      fBranches.emplace_back(std::make_unique<RBranch>(branch_model));
   }
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
      (*iter_leafs)->Serialize(dst);
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
