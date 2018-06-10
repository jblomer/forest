#ifndef RBRANCH_H_
#define RBRANCH_H_

#include <cassert>
#include <iterator>
#include <stack>
#include <string>
#include <string_view>
#include <vector>

#include "iterator_tpl.h"

#include "RTreeColumn.hxx"
#include "RTreeMedium.hxx"
#include "RLeaf.hxx"

namespace Toy {

class RBranchBase {
  friend class RTree;  // REMOVE ME

protected:
   RBranchBase *fParent;
   std::vector<RBranchBase *> fChildren; /* TODO unique_ptr */
   std::string fDescription;
   std::string fName;
   bool fIsSimple;
   RTreeColumn *fPrincipalColumn;

   explicit RBranchBase(std::string_view name)
     : fParent(nullptr)
     , fName(name)
     , fIsSimple(false)
     , fPrincipalColumn(nullptr)
   { }

   virtual void DoAppend(RLeafBase *leaf) { assert(false); }
   virtual void DoRead(std::uint64_t num, RLeafBase *leaf) { assert(false); }

public:
  struct IteratorState {
    std::stack<unsigned> pos_stack;
    const RBranchBase *current;
    unsigned pos;
    inline void next(const RBranchBase* ref) {
      if (pos < current->fChildren.size()) {
        current = current->fChildren[pos];
        pos_stack.push(pos);
        pos = 0;
      }
      while ((pos == current->fChildren.size()) && !pos_stack.empty()) {
        current = current->fParent;
        pos = pos_stack.top() + 1;
        pos_stack.pop();
      }
    }
    inline void begin(const RBranchBase* ref) {
      current = ref;
      pos = 0;
    }
    inline void end(const RBranchBase* ref) {
      current = ref;
      pos = ref->fChildren.size();
    }
    inline RBranchBase* get(RBranchBase* ref) {
      return current->fChildren[pos];
    }
    inline const RBranchBase* get(const RBranchBase* ref)
    {
      return current->fChildren[pos];
    }
    inline bool cmp(const IteratorState& s) const {
      return (current != s.current) || (pos != s.pos);
    }
  };
  SETUP_ITERATORS(RBranchBase, RBranchBase*, IteratorState);

  std::string GetName() { return fName; }
  void PrependName(std::string_view parent) {
    fName = std::string(parent) + "/" + fName;
  }

  virtual ~RBranchBase() { }

  void Attach(RBranchBase *child) {
    child->fParent = this;
    fChildren.push_back(child);
  }

  virtual RTreeColumn* GenerateColumns(RTreeSource *source, RTreeSink *sink)
    = 0;

  void Append(RLeafBase *leaf) {
    if (!fIsSimple) {
      DoAppend(leaf);
      return;
    }
    fPrincipalColumn->Append(*(leaf->fPrincipalElement));
  }

  void Read(std::uint64_t num, RLeafBase *leaf) {
    if (!fIsSimple) {
      DoRead(num, leaf);
      return;
    }
    fPrincipalColumn->Read(num, leaf->fPrincipalElement.get());
  }

  //void
};


class RBranchCollectionTag {};

template <typename T>
class RBranch : public RBranchBase {
};

template <>
class RBranch<RBranchCollectionTag> : public RBranchBase {
public:
  RBranch() : RBranchBase("") {
    fIsSimple = true;
  }
  explicit RBranch(std::string_view name) : RBranchBase(name) {
    fIsSimple = true;
  }

  virtual RTreeColumn* GenerateColumns(RTreeSource *source, RTreeSink *sink)
    override
  {
    fPrincipalColumn = new RTreeColumn(
      RTreeColumnModel(fName, fDescription, RTreeColumnType::kOffset, false),
      source, sink);
    return fPrincipalColumn;
  }

  bool IsRoot() { return fName.empty(); }
  void MakeSubBranch(std::string_view name) { fName = name; }
};


template <>
class RBranch<float> : public RBranchBase {
public:
  explicit RBranch(std::string_view name) : RBranchBase(name) {
    fIsSimple = true;
  }

  virtual RTreeColumn* GenerateColumns(RTreeSource *source, RTreeSink *sink)
    override
  {
    fPrincipalColumn = new RTreeColumn(
      RTreeColumnModel(fName, fDescription, RTreeColumnType::kFloat, false),
      source, sink);
    return fPrincipalColumn;
  }
};

}

#endif