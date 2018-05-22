#include "RTree.hxx"

#include <cassert>
#include <iterator>
#include <iostream>
#include <utility>

#include "RBasket.hxx"
#include "RTreeEntry.hxx"
#include "RTreeMedium.hxx"
#include "RTreeModel.hxx"

namespace Toy {

RTree::RTree(std::shared_ptr<RTreeModel> model, std::unique_ptr<RTreeSink> sink)
   : fSink(std::move(sink))
   , fModel(model)
{
  fModel->Freeze();
  fSink->Attach(this);

  for (auto branch : fModel->fRootBranch) {
    // Todo: column parent-children relationship
    fColumns.push_back(branch->GenerateColumns(fSink.get()));
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

  /*for (auto branch : fModel->fRootBranch) {
    // Todo: column parent-children relationship
    fColumns.push_back(branch->GenerateColumns(fSink.get()));
    std::cout << branch->GetName() << std::endl;
  }

  fSink->OnCreate();*/
}


RTree::~RTree() {
}


void RTree::FillV(RTreeEntry **entry, unsigned size) {
  for (unsigned i = 0; i < size; ++i) {
    assert(entry[i]);
    assert(entry[i]->IsCompatibleWith(fModel.get()));

    Fill(entry[i]);
  }
}


void RTree::Fill(RTreeEntry *entry) {
  assert(entry);
  assert(entry->IsCompatibleWith(fModel.get()));

  for (auto&& ptr_leaf : entry->GetLeafsRef()) {
    //std::cout << "Filling " << ptr_leaf->GetBranch()->GetName() << std::endl;
    ptr_leaf->GetBranch()->Write(ptr_leaf.get());
    //ptr_leaf->GetSize();
  }

   // Checkpoint (Mini Footer)
}

}  // namespace Toy
