#include "RTreeColumn.hxx"

#include <cassert>
#include <iostream>

#include "RTreeMedium.hxx"

namespace Toy {

RTreeColumn::RTreeColumn(const RTreeColumnModel &model, RTreeSink *sink)
  : fModel(model)
  , fSink(sink)
  , fHeadBasket(std::make_unique<RBasket>(
    4 /*TODO*/ * kDefaultNumElements))
{
  assert(fSink);
  fSink->OnAddColumn(this);
}

void RTreeColumn::ShipHeadBasket() {
  assert(fSink);
  fHeadBasket->Freeze();
  fSink->OnFullBasket(fHeadBasket.get(), this);
  fHeadBasket->Reset();
}

}  // namespace Toy
