#include "TTree.hxx"

#include "TTreeModel.hxx"

namespace Toy {

TTree::TTree(std::shared_ptr<TTreeModel> model) : fModel(model) {
   model->Freeze();
}

}  // namespace Toy
