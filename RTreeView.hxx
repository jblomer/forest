#ifndef RTREE_VIEW_H_
#define RTREE_VIEW_H_

#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include "RBranch.hxx"
#include "RColumnPointer.hxx"
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

  T operator ()(const RColumnPointer &p) {
    fBranch->Read(p.GetIndex(), &fCargo);
    return *fCargo.Get();
  }

  void ReadBulk(std::uint64_t start, std::uint64_t num, T *buf) {
    fBranch->ReadV(start, num, buf);
  }
};


template <>
class RTreeView<RTreeOffset> {
private:
  std::unique_ptr<RBranch<RTreeOffset>> fBranch;
  // For offset columns, read both the index and the one before to
  // get the size TODO
  RTreeOffset fOffsetPair[2];
  RCargo<RTreeOffset> fCargo;

public:
  RTreeView(RBranch<RTreeOffset> *branch)
    : fBranch(branch)
    , fCargo(fBranch.get())
  { }

  RTreeOffset operator ()(const RColumnPointer &p) {
    if (p.GetIndex() == 0) {
      fBranch->Read(p.GetIndex(), &fCargo);
      return *fCargo.Get();
    }
    fBranch->Read(p.GetIndex() - 1, &fCargo);
    RTreeOffset lower = *fCargo.Get();
    fBranch->Read(p.GetIndex(), &fCargo);
    return *fCargo.Get() - lower;
  }

  void ReadBulk(std::uint64_t start, std::uint64_t num, RTreeOffset *buf) {
    fBranch->ReadV(start, num, buf);
  }
};


class RTreeViewCollection : public RTreeView<RTreeOffset> {
public:
  RTreeViewCollection(RBranch<RTreeOffset> *b) :
    RTreeView<RTreeOffset>(b) { }

  //template <typename T>
  //RTreeView<T> GetView(std::string_view name) {
  //  auto branch = new RBranch<T>(name);  // TODO not with raw pointer
  //  branch->GenerateColumns(fSource, nullptr);
  //  return RTreeView<T>(branch);
  //}

  //REntryRange GetEntryRange(RRangeType type, REntryPointer parent) {
  //  //return REntryRange(this);
  //}
};


}  // namespace Toy

#endif
