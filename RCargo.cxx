#include "RCargo.hxx"

#include <vector>

#include "RBranch.hxx"

namespace Toy {

void RCargoSubtree::Fill() {
  for (auto child_cargo : fChildren)
    child_cargo->fBranch->Append(child_cargo.get());
  fOffset++;
  //std::cout << "Filling " << fBranch->GetName() << " with " << fOffset
  //  << std::endl;
}

void RCargoSubtree::FillV(RCargoBase **cargo, unsigned size) {
  for (unsigned i = 0; i < size; ++i)
    cargo[i]->fBranch->Append(cargo[i]);
  fOffset += size;
}

template <>
void Toy::RCargo<RColumnOffset>::Init() {
  fPrincipalElement =
    std::make_unique<Toy::RColumnElement<RColumnOffset>>(fValue.get());
  fIsSimple = true;
}

}

template <>
void Toy::RCargo<float>::Init() {
  fPrincipalElement =
    std::make_unique<Toy::RColumnElement<float>>(fValue.get());
  fIsSimple = true;
}

template <>
void Toy::RCargoCaptured<float>::Init() {
  fPrincipalElement = std::make_unique<Toy::RColumnElement<float>>(fValue);
  fIsSimple = true;
}
