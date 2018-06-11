/**
 * The "leaf" represents that transient storage of simple of complex
 * C++ values that are supposed to be serialized on Fill or that just
 * have been deserialized.
 */

#ifndef RLEAF_H_
#define RLEAF_H_

#include <cassert>
#include <memory>
#include <utility>
#include <vector>

#include "RTreeColumn.hxx"
#include "RTreeElement.hxx"


namespace Toy {

class RBranchBase;

class RLeafBase {
  friend class RBranchBase;
  friend class RLeafSubtree;

protected:
  RBranchBase* fBranch;
  std::unique_ptr<RTreeElementBase> fPrincipalElement;
  bool fIsSimple;

  RLeafBase(RBranchBase *branch)
    : fBranch(branch)
    , fIsSimple(false)
  { }

public:
   virtual ~RLeafBase() { }

   RBranchBase* GetBranch() { return fBranch; }
};

using LeafCollection = std::vector<std::shared_ptr<RLeafBase>>;


template <typename T>
class RLeaf : public RLeafBase {
   std::shared_ptr<T> fValue;

   void Init();

public:
   template <typename... ArgsT>
   RLeaf(RBranchBase *branch, ArgsT&&... args) : RLeafBase(branch) {
     fValue = std::make_shared<T>(std::forward<ArgsT>(args)...);
     Init();
   }

   std::shared_ptr<T> Get() { return fValue; }
};


template <typename T>
class RLeafCaptured : public RLeafBase {
   T *fValue;

   void Init();

public:
   RLeafCaptured(RBranchBase *branch, T *value)
     : RLeafBase(branch)
     , fValue(value)
   {
     Init();
   }

   T *Get() { return fValue; }
};


class RLeafSubtree : public RLeafBase {
  std::uint64_t fOffset;
  LeafCollection fChildren;

  void Init();

public:
  RLeafSubtree(RBranchBase *branch, const LeafCollection &entry)
    : RLeafBase(branch)
    , fOffset(0)
  {
    fPrincipalElement = std::make_unique<RTreeElement<RTreeOffset>>(&fOffset);
    fIsSimple = true;
    fChildren = entry;
  }

  void Fill();
  void FillV(RLeafBase **leafs, unsigned size);
};


}  // namespace Toy


#endif // RLEAF_H_
