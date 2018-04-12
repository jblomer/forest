#include "TTreeModel.hxx"

#include <TROOT.h>

namespace Toy {
}  // namespace Toy

// Has to be in Float_t namespace
template <>
Toy::TBranchType Toy::TBranch::MapType<Float_t>() {
   return Toy::TBranchType::kTypeFloat;
}
