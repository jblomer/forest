#include "RTreeColumn.hxx"

#include <cassert>

#include "RTreeMedium.hxx"

namespace Toy {

void RTreeColumn::ShipHeadBasket() {
   fHeadBasket->Freeze();
   fSink->OnFullBasket(fHeadBasket.get(), this);
   fHeadBasket->Reset();
}

}  // namespace Toy
