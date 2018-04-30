#include "RBranchModel.hxx"

#include <cassert>

namespace Toy {

std::size_t RBranchModel::GetElementSize() {
   switch (fType) {
      case RBranchType::kFloat: return sizeof(float);
      case RBranchType::kInt: return sizeof(int);
      case RBranchType::kBlob: return 8;  // (or something)
      default: assert(false);
   };
}

}  // namespace Toy
