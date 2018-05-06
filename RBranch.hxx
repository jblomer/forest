#ifndef RBRANCH_H_
#define RBRANCH_H_

#include <cassert>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

#include "RTreeColumn.hxx"
#include "RTreeMedium.hxx"
#include "RLeaf.hxx"

namespace Toy {

class RBranchBase {
  friend class RTree;  // REMOVE ME

  RBranchBase *fParent;
  std::vector<RBranchBase *> fChildren; /* TODO unique_ptr */

protected:
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

   virtual void DoWrite(RLeafBase *leaf) { assert(false); }

public:
  /*class Iterator {
    RBranchBase *fBranch;
    unsigned pos;
  public:
    typedef RBranchBase value_type;
    typedef RBranchBase& reference;
    typedef RBranchBase* pointer;
    typedef std::forward_iterator_tag iterator_category;
    //typedef int difference_type;
    Iterator(pointer ptr) : ptr_(ptr) { }
    self_type operator++() { self_type i = *this; ptr_++; return i; }
    self_type operator++(int junk) { ptr_++; return *this; }
    reference operator*() { return *ptr_; }
    pointer operator->() { return ptr_; }
    bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
    bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
  };*/

  std::string GetName() { return fName; }


  virtual ~RBranchBase() { }

  void Attach(RBranchBase *child) {
    child->fParent = this;
    fChildren.push_back(child);
  }

  virtual RTreeColumn* GenerateColumns(RTreeSink *sink) = 0;

  void Write(RLeafBase *leaf) {
    if (!fIsSimple) {
      DoWrite(leaf);
      return;
    }
    fPrincipalColumn->Append(*(leaf->fPrincipalElement));
  }
};


class RBranchRoot {};

template <typename T>
class RBranch : public RBranchBase {
};

template <>
class RBranch<RBranchRoot> : public RBranchBase {
public:
  RBranch() : RBranchBase("") { }

  virtual RTreeColumn* GenerateColumns(RTreeSink *sink) override {
    return nullptr;
  }
};

template <>
class RBranch<float> : public RBranchBase {
public:
  explicit RBranch(std::string_view name) : RBranchBase(name) {
    fIsSimple = true;
  }

  virtual RTreeColumn* GenerateColumns(RTreeSink *sink) override {
    fPrincipalColumn = new RTreeColumn(
      RTreeColumnModel(fName, fDescription, RTreeColumnType::kFloat, false),
      sink);
    return fPrincipalColumn;
  }
};


template <>
class RBranch<std::vector<float>> : public RBranchBase {
public:
  explicit RBranch(std::string_view name) : RBranchBase(name) { }

  virtual RTreeColumn* GenerateColumns(RTreeSink *sink) override {
    assert(false);
    return nullptr;
  }
};

}

#endif