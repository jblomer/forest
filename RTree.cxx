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
  fSink->Attach(this);

  // TODO: iteration
  for (auto branch : fModel->fRootBranch.fChildren) {
    fColumns.push_back(branch->GenerateColumns(fSink.get()));
  }

  fSink->OnCreate();
}


void RTree::Fill(RTreeEntry *entry) {
  assert(entry);
  assert(entry->IsCompatibleWith(fModel.get()));

  for (auto&& ptr_leaf : entry->GetLeafsRef()) {
    ptr_leaf->GetBranch()->Write(ptr_leaf.get());
    //ptr_leaf->GetSize();
  }

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
