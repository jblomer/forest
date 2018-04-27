#include "RTree.hxx"

#include "RTreeEntry.hxx"
#include "RTreeModel.hxx"

namespace Toy {

RTree::RTree(std::shared_ptr<RTreeModel> model) : fModel(model) {
   model->Freeze();
}

void RTree::Fill(RTreeEntry *entry) {
   // Assert entry belongs to same tree model

   // Iterate through entry leafes and serialize into basket buffers of
   // branches

   // Flush buffers
   // Checkpoint (Mini Footer)
}

}  // namespace Toy
