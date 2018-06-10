#include "RTreeColumn.hxx"

#include <cassert>
#include <iostream>

#include "RTreeMedium.hxx"

namespace Toy {

RTreeColumn::RTreeColumn(
  const RTreeColumnModel &model,
  RTreeSource *source,
  RTreeSink *sink)
  : fModel(model)
  , fSource(source)
  , fSink(sink)
  , fHeadBasket(nullptr)
  , fMaxElement(0)
  , fCurrentSlice(nullptr)
  , fCurrentSliceStart(-1)
  , fCurrentSliceEnd(-1)
{
  if (fSink) {
    fSink->OnAddColumn(this);
    fHeadBasket =
      std::make_unique<RBasket>(4 /*TODO*/ * kDefaultNumElements, 0);
  }
  if (fSource) {
    fSource->OnAddColumn(this);
    fCurrentSlice =
      std::make_unique<RBasket>(4 /*TODO*/ * kDefaultNumElements, 0);
  }
}

void RTreeColumn::ShipHeadBasket() {
  assert(fSink);
  fHeadBasket->Freeze();
  fSink->OnFullBasket(fHeadBasket.get(), this);
  fHeadBasket->Reset(fMaxElement);
  //std::cout << "RESETTING TO " << fMaxElement << std::endl;
}

void RTreeColumn::MapSlice(std::uint64_t num) {
  fSource->OnMapSlice(this, num, fCurrentSlice.get());
  fCurrentSliceStart = fCurrentSlice->GetRangeStart();
  fCurrentSliceEnd = fCurrentSlice->GetRangeStart() +
    (fCurrentSlice->GetSize() / fModel.GetElementSize()) - 1;
}

}  // namespace Toy
