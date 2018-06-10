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
   fSize = sizeof(float);
}

template <>
std::string Toy::RTreeElement<float>::GetMemoryType() {
   return "float";
}


template <>
Toy::RTreeColumnType Toy::RTreeElement<Toy::RTreeOffset>::MapType() {
   return Toy::RTreeColumnType::kOffset;
}

template <>
void Toy::RTreeElement<Toy::RTreeOffset>::Initialize() {
   fIsMovable = true;
   fRawContent = &fValue;
   fSize = sizeof(Toy::RTreeOffset);
}

template <>
std::string Toy::RTreeElement<Toy::RTreeOffset>::GetMemoryType() {
   return "index";
}
