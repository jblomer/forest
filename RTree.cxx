#include "RTree.hxx"

#include <cassert>
#include <iterator>

#include "RTreeEntry.hxx"
#include "RTreeModel.hxx"

namespace Toy {

RTree::RTree(std::shared_ptr<RTreeModel> model) : fModel(model) {
   model->Freeze();
   // Create branches from model

}

void RTree::Fill(RTreeEntry *entry) {
   assert(entry->IsCompatibleWith(fModel.get()));

   for (auto&& ptr_leaf : entry->GetLeafsRef()) {
     ptr_leaf->GetSizeDisk();
   }
   //for (auto iter = std::begin()) {
   //}

   //for (auto branch : fBranches) {
   //  branch.Reserve(5);
   //}

   // Iterate through entry leafes and serialize into basket buffers of
   // branches

   // Flush buffers
   // Checkpoint (Mini Footer)
}

}  // namespace Toy
