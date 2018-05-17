#include "RLeaf.hxx"

#include <vector>

#include "RBranch.hxx"

namespace Toy {

void RLeafSubtree::Fill() {
  for (auto child_leaf : fChildren)
    child_leaf->fBranch->Write(child_leaf.get());
  fOffset++;
}

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
