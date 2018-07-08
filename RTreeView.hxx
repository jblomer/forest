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

class RTreeSource;

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
class RTreeView<RTreeOffset> {
protected:
  std::unique_ptr<RBranch<RTreeOffset>> fBranch;

private:
  // For offset columns, read both the index and the one before to
  // get the size TODO
  RTreeOffset fOffsetPair[2];
  RCargo<RTreeOffset> fCargo;

public:
  RTreeView(RBranch<RTreeOffset> *branch)
    : fBranch(branch)
    , fCargo(fBranch.get())
  { }

  RColumnRange GetRange(const RColumnPointer &p) {
    if (p.GetIndex() == 0) {
      fBranch->Read(0, &fCargo);
      return RColumnRange(0, *fCargo.Get());
    }
    fBranch->Read(p.GetIndex() - 1, &fCargo);
    RTreeOffset lower = *fCargo.Get();
    fBranch->Read(p.GetIndex(), &fCargo);
    return RColumnRange(lower, *fCargo.Get());
  }

  RTreeOffset operator ()(const RColumnPointer &p) {
    return GetRange(p).GetSize();
  }

  void ReadBulk(std::uint64_t start, std::uint64_t num, RTreeOffset *buf) {
    fBranch->ReadV(start, num, buf);
  }
};


class RTreeViewCollection : public RTreeView<RTreeOffset> {
private:
  RTreeSource *fSource;
public:
  RTreeViewCollection(RBranch<RTreeOffset> *b, RTreeSource *s) :
    RTreeView<RTreeOffset>(b), fSource(s) { }

  template <typename T>
  RTreeView<T> GetView(std::string_view name) {
    // TODO not with raw pointer
    auto branch = new RBranch<T>(fBranch->GetName() + "/" + std::string(name));
    branch->GenerateColumns(fSource, nullptr);
    return RTreeView<T>(branch);
  }

  RTreeViewCollection GetViewCollection(std::string_view name) {
    auto branch = new RBranch<RTreeOffset>(
      fBranch->GetName() + "/" + std::string(name));
    branch->GenerateColumns(fSource, nullptr);
    return RTreeViewCollection(branch, fSource);
  }
};


}  // namespace Toy

#endif
