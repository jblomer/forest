#ifndef RTREE_VIEW_H_
#define RTREE_VIEW_H_

#include <memory>
#include <string_view>
#include <utility>

#include "RBranch.hxx"
#include "REntryPointer.hxx"
#include "RLeaf.hxx"

namespace Toy {

template <typename T>
class RTreeView {
private:
  std::unique_ptr<RBranch<T>> fBranch;
  REntryPointer fLastFetchedEntry;
  RLeaf<T> fLeaf;

public:
  RTreeView(RBranch<T> *branch)
    : fBranch(branch)
    , fLastFetchedEntry(REntryPointer::kInvalidEntry)
    , fLeaf(fBranch.get())
  { }

  T operator ()(const REntryPointer &p) {
    if (p != fLastFetchedEntry) {
      fBranch->Read(p.fEntryNumber, &fLeaf);
      fLastFetchedEntry = p;
    }
    return *fLeaf.Get();
  }
};

}  // namespace Toy

#endif
