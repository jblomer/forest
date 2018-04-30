#include "RLeaf.hxx"

namespace Toy {

}  // namespace Toy


// Has to be in float namespace

template <>
Toy::RBranchType Toy::RLeaf<float>::MapType() {
   return Toy::RBranchType::kFloat;
}

template <>
void Toy::RLeaf<float>::Initialize() {
   fIsMovable = true;
   fRawContent = fValue.get();
   fIsFixedSize = true;
   fSize = sizeof(float);
}

template <>
std::string Toy::RLeaf<float>::GetMemoryType() {
   return "float";
}
