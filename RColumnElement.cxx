#include "RColumnElement.hxx"

namespace Toy {

}  // namespace Toy


// Has to be in float namespace

template <>
Toy::RColumnType Toy::RColumnElement<float>::MapType() {
   return Toy::RColumnType::kFloat;
}

template <>
void Toy::RColumnElement<float>::Initialize() {
   fIsMovable = true;
   fRawContent = fValue;
   fSize = sizeof(float);
}

template <>
std::string Toy::RColumnElement<float>::GetMemoryType() {
   return "float";
}


template <>
Toy::RColumnType Toy::RColumnElement<Toy::RColumnOffset>::MapType() {
   return Toy::RColumnType::kOffset;
}

template <>
void Toy::RColumnElement<Toy::RColumnOffset>::Initialize() {
   fIsMovable = true;
   fRawContent = fValue;
   fSize = sizeof(Toy::RColumnOffset);
}

template <>
std::string Toy::RColumnElement<Toy::RColumnOffset>::GetMemoryType() {
   return "index";
}
