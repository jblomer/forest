#include "RLeaf.hxx"

#include <vector>

namespace Toy {

}

template <>
void Toy::RLeaf<float>::Init() {
  fPrincipalElement = std::make_unique<Toy::RTreeElement<float>>(fValue.get());
  fIsSimple = true;
}

template <>
void Toy::RLeafCaptured<float>::Init() {
  fPrincipalElement = std::make_unique<Toy::RTreeElement<float>>(fValue);
  fIsSimple = true;
}

