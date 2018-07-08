#include "RTree.hxx"

#include <cassert>
#include <iterator>
#include <iostream>
#include <utility>

#include "RBasket.hxx"
#include "RColumnRange.hxx"
#include "RTreeEntry.hxx"
#include "RTreeMedium.hxx"
#include "RTreeModel.hxx"

namespace Toy {

RColumnRange RTree::GetEntryRange(RRangeType type, RTreeEntry *entry) {
  return RColumnRange(0, fNentries);
}

RTree::RTree(std::shared_ptr<RTreeModel> model, std::unique_ptr<RTreeSink> sink)
   : fSink(std::move(sink))
   , fModel(model)
   , fNentries(0)
{
  fModel->Freeze();
  fSink->Attach(this);

  for (auto branch : fModel->fRootBranch) {
    // Todo: column parent-children relationship
    fColumns.push_back(branch->GenerateColumns(nullptr, fSink.get()));
    std::cout << branch->GetName() << std::endl;
  }

  fSink->OnCreate();
}


RTree::RTree(
  std::shared_ptr<RTreeModel> model,
  std::unique_ptr<RTreeSource> source)
  : fSource(std::move(source))
  , fModel(model)
{
  fModel->Freeze();
  std::cout << "CREATING TREE FOR READING" << std::endl;
  fSource->Attach(this);
  fNentries = fSource->GetNentries();
}


RTree::~RTree() {
  //std::cout << "FLUSHING ALL COLUMNS" << std::endl;
  for (auto column : fColumns)
    column->Flush();
}


void RTree::FillV(RTreeEntry **entry, unsigned size) {
  for (unsigned i = 0; i < size; ++i) {
    assert(entry[i]);
    assert(entry[i]->IsCompatibleWith(fModel.get()));

    Fill(entry[i]);
  }
  fNentries += size;
}


void RTree::Fill(RTreeEntry *entry) {
  //assert(entry);
  //assert(entry->IsCompatibleWith(fModel.get()));

  for (auto&& ptr_cargo : entry->GetCargoRefs()) {
    //std::cout << "Filling " << ptr_cargo->GetBranch()->GetName() << std::endl;
    ptr_cargo->GetBranch()->Append(ptr_cargo.get());
  }
  fNentries++;
}

}  // namespace Toy
