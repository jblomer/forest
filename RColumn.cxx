#include "RColumn.hxx"

#include <cassert>
#include <iostream>

#include "RColumnStorage.hxx"

namespace Toy {

RColumn::RColumn(
  const RColumnModel &model,
  RColumnSource *source,
  RColumnSink *sink)
  : fModel(model)
  , fSource(source)
  , fSink(sink)
  , fHeadSlice(nullptr)
  , fMaxElement(0)
  , fCurrentSlice(nullptr)
  , fCurrentSliceStart(-1)
  , fCurrentSliceEnd(-1)
{
  if (fSink) {
    fSink->OnAddColumn(this);
    fHeadSlice =
      std::make_unique<RColumnSlice>(4 /*TODO*/ * kDefaultNumElements, 0);
  }
  if (fSource) {
    fSource->OnAddColumn(this);
    fCurrentSlice =
      std::make_unique<RColumnSlice>(4 /*TODO*/ * kDefaultNumElements, 0);
    fMaxElement = fSource->GetNElements(this);
  }
}

void RColumn::ShipHeadSlice() {
  assert(fSink);
  fHeadSlice->Freeze();
  fSink->OnFullSlice(fHeadSlice.get(), this);
  fHeadSlice->Reset(fMaxElement);
  //std::cout << "RESETTING TO " << fMaxElement << std::endl;
}

void RColumn::MapSlice(std::uint64_t num) {
  fSource->OnMapSlice(this, num, fCurrentSlice.get());
  fCurrentSliceStart = fCurrentSlice->GetRangeStart();
  fCurrentSliceEnd = fCurrentSlice->GetRangeStart() +
    (fCurrentSlice->GetSize() / fModel.GetElementSize()) - 1;
}

}  // namespace Toy
