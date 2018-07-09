#include "RTreeElement.hxx"

namespace Toy {

}  // namespace Toy


// Has to be in float namespace

template <>
Toy::RColumnType Toy::RTreeElement<float>::MapType() {
   return Toy::RColumnType::kFloat;
}

template <>
void Toy::RTreeElement<float>::Initialize() {
   fIsMovable = true;
   fRawContent = fValue;
   fSize = sizeof(float);
}

template <>
std::string Toy::RTreeElement<float>::GetMemoryType() {
   return "float";
}


template <>
Toy::RColumnType Toy::RTreeElement<Toy::RTreeOffset>::MapType() {
   return Toy::RColumnType::kOffset;
}

template <>
void Toy::RTreeElement<Toy::RTreeOffset>::Initialize() {
   fIsMovable = true;
   fRawContent = fValue;
   fSize = sizeof(Toy::RTreeOffset);
}

template <>
std::string Toy::RTreeElement<Toy::RTreeOffset>::GetMemoryType() {
   return "index";
}
