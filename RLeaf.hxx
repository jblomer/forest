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

  RBranchBase* fBranch;

protected:
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


class RLeafCollection : public RLeafBase {
  unsigned offset;
  std::vector<RLeafBase *> fChildren;

  void Init();

public:
  RLeafCollection(RBranchBase *branch)
    : RLeafBase(branch)
    , offset(0)
  { }

  void Fill() {
    offset++;
  }
};


}  // namespace Toy


#endif // RLEAF_H_
