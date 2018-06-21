#ifndef RTREE_VIEW_H_
#define RTREE_VIEW_H_

#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include "RBranch.hxx"
#include "REntryPointer.hxx"
#include "RCargo.hxx"

namespace Toy {

template <typename T>
class RTreeView {
private:
  std::unique_ptr<RBranch<T>> fBranch;
  RCargo<T> fCargo;

public:
  RTreeView(RBranch<T> *branch)
    : fBranch(branch)
    , fCargo(fBranch.get())
  { }

  T operator ()(const REntryPointer &p) {
    fBranch->Read(p.fEntryNumber, &fCargo);
    return *fCargo.Get();
  }

  void ReadBulk(std::uint64_t start, std::uint64_t num, T *buf) {
    fBranch->ReadV(start, num, buf);
  }
};


class RTreeViewCollection : public RTreeView<RBranchCollectionTag> {
public:
  RTreeViewCollection(RBranch<RBranchCollectionTag> *b) :
    RTreeView<RBranchCollectionTag>(b) { }
};


}  // namespace Toy

#endif
