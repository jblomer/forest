#ifndef RTREE_VIEW_H_
#define RTREE_VIEW_H_

#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include "iterator_tpl.h"

#include "RBranch.hxx"
#include "RColumnPointer.hxx"
#include "RColumnRange.hxx"
#include "RCargo.hxx"

namespace Toy {

class RColumnSource;

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

  struct ViewIterator {
    std::uint64_t pos;
    inline void next(const RTreeView<T>* ref) {
      pos++;
    }
    inline void begin(const RTreeView<T>* ref) {
      pos = 0;
    }
    inline void end(const RTreeView<T>* ref) {
      pos = ref->fBranch->GetNItems();
    }
    inline T get(RTreeView<T>* ref) {
      return (*ref)(RColumnPointer(pos));
    }
    inline const T get(const RTreeView<T>* ref)
    {
      return (*ref)(RColumnPointer(pos));
    }
    inline bool cmp(const ViewIterator& s) const {
      return (pos != s.pos);
    }
  };
  SETUP_ITERATORS(RTreeView, T, ViewIterator);
};


template <>
class RTreeView<RColumnOffset> {
protected:
  std::unique_ptr<RBranch<RColumnOffset>> fBranch;

private:
  // For offset columns, read both the index and the one before to
  // get the size TODO
  RColumnOffset fOffsetPair[2];
  RCargo<RColumnOffset> fCargo;

public:
  RTreeView(RBranch<RColumnOffset> *branch)
    : fBranch(branch)
    , fCargo(fBranch.get())
  { }

  RColumnRange GetRange(const RColumnPointer &p) {
    if (p.GetIndex() == 0) {
      fBranch->Read(0, &fCargo);
      return RColumnRange(0, *fCargo.Get());
    }
    fBranch->Read(p.GetIndex() - 1, &fCargo);
    RColumnOffset lower = *fCargo.Get();
    fBranch->Read(p.GetIndex(), &fCargo);
    return RColumnRange(lower, *fCargo.Get());
  }

  RColumnOffset operator ()(const RColumnPointer &p) {
    return GetRange(p).GetSize();
  }

  void ReadBulk(std::uint64_t start, std::uint64_t num, RColumnOffset *buf) {
    fBranch->ReadV(start, num, buf);
  }
};


class RTreeViewCollection : public RTreeView<RColumnOffset> {
private:
  RColumnSource *fSource;
public:
  RTreeViewCollection(RBranch<RColumnOffset> *b, RColumnSource *s) :
    RTreeView<RColumnOffset>(b), fSource(s) { }

  template <typename T>
  RTreeView<T> GetView(std::string_view name) {
    // TODO not with raw pointer
    auto branch = new RBranch<T>(fBranch->GetName() + "/" + std::string(name));
    branch->GenerateColumns(fSource, nullptr);
    return RTreeView<T>(branch);
  }

  RTreeViewCollection GetViewCollection(std::string_view name) {
    auto branch = new RBranch<RColumnOffset>(
      fBranch->GetName() + "/" + std::string(name));
    branch->GenerateColumns(fSource, nullptr);
    return RTreeViewCollection(branch, fSource);
  }
};


}  // namespace Toy

#endif
