#include "RBranchModel.hxx"

namespace Toy {
}  // namespace Toy

// Has to be in Float_t namespace
template <>
Toy::RBranchType Toy::RBranchModel::MapType<float>() {
   return Toy::RBranchType::kFloat;
}