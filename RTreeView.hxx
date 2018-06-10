#ifndef RTREE_VIEW_H_
#define RTREE_VIEW_H_

#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include "RBranch.hxx"
#include "REntryPointer.hxx"
#include "RLeaf.hxx"

namespace Toy {

template <typename T>
class RTreeView {
private:
  std::unique_ptr<RBranch<T>> fBranch;
  RLeaf<T> fLeaf;

public:
  RTreeView(RBranch<T> *branch)
    : fBranch(branch)
    , fLeaf(fBranch.get())
  { }

  T operator ()(const REntryPointer &p) {
    fBranch->Read(p.fEntryNumber, &fLeaf);
    return *fLeaf.Get();
  }

  void ReadBulk(std::uint64_t start, std::uint64_t num, T *buf) {
    fBranch->ReadV(start, num, buf);
  }
};


}  // namespace Toy

#endif
