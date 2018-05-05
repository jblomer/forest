#include "RTreeColumn.hxx"

#include <cassert>
#include <iostream>

#include "RTreeMedium.hxx"

namespace Toy {

void RTreeColumn::ShipHeadBasket() {
  assert(fSink);
  fHeadBasket->Freeze();
  fSink->OnFullBasket(fHeadBasket.get(), this);
  fHeadBasket->Reset();
}

}  // namespace Toy
