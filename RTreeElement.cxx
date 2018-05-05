#include "RTreeElement.hxx"

namespace Toy {

}  // namespace Toy


// Has to be in float namespace

template <>
Toy::RTreeColumnType Toy::RTreeElement<float>::MapType() {
   return Toy::RTreeColumnType::kFloat;
}

template <>
void Toy::RTreeElement<float>::Initialize() {
   fIsMovable = true;
   fRawContent = &fValue;
   fIsFixedSize = true;
   fSize = sizeof(float);
}

template <>
std::string Toy::RTreeElement<float>::GetMemoryType() {
   return "float";
}