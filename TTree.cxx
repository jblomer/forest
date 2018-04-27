#include "TTree.hxx"

#include "TTreeModel.hxx"

namespace Toy {

TTree::TTree(std::shared_ptr<TTreeModel> model) : fModel(model) {
   model->Freeze();
}

void TTree::Fill() {
   // Iterate through branches and serialize into buffers
   // Flush buffers
   // Checkpoint (Mini Footer)
}

}  // namespace Toy
