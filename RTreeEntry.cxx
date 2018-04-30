#include "RTreeEntry.hxx"

#include "RTreeModel.hxx"

namespace Toy {

bool RTreeEntry::HasCompatibleModelId(RTreeModel *model) {
   return model->GetModelId() == fModel->GetModelId();
}

}  // namespace Toy
