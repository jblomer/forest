/**
 * The "cargo" represents transient storage of simple of complex
 * C++ values that are supposed to be serialized on Fill or that just
 * have been deserialized for reading.
 */

#ifndef RCARGO_H_
#define RCARGO_H_

#include <cassert>
#include <memory>
#include <utility>
#include <vector>

#include "RTreeColumn.hxx"
#include "RTreeElement.hxx"


namespace Toy {

class RBranchBase;

class RCargoBase {
  friend class RBranchBase;
  friend class RCargoSubtree;

protected:
  RBranchBase* fBranch;
  std::unique_ptr<RTreeElementBase> fPrincipalElement;
  bool fIsSimple;

  RCargoBase(RBranchBase *branch)
    : fBranch(branch)
    , fIsSimple(false)
  { }

public:
   virtual ~RCargoBase() { }

   RBranchBase* GetBranch() { return fBranch; }
};

using CargoCollection = std::vector<std::shared_ptr<RCargoBase>>;


template <typename T>
class RCargo : public RCargoBase {
   std::shared_ptr<T> fValue;

   void Init();

public:
   template <typename... ArgsT>
   RCargo(RBranchBase *branch, ArgsT&&... args) : RCargoBase(branch) {
     fValue = std::make_shared<T>(std::forward<ArgsT>(args)...);
     Init();
   }

   std::shared_ptr<T> Get() { return fValue; }
};


template <typename T>
class RCargoCaptured : public RCargoBase {
   T *fValue;

   void Init();

public:
   RCargoCaptured(RBranchBase *branch, T *value)
     : RCargoBase(branch)
     , fValue(value)
   {
     Init();
   }

   T *Get() { return fValue; }
};


class RCargoSubtree : public RCargoBase {
  std::uint64_t fOffset;
  CargoCollection fChildren;

  void Init();

public:
  RCargoSubtree(RBranchBase *branch, const CargoCollection &entry)
    : RCargoBase(branch)
    , fOffset(0)
  {
    fPrincipalElement = std::make_unique<RTreeElement<RTreeOffset>>(&fOffset);
    fIsSimple = true;
    fChildren = entry;
  }

  void Fill();
  void FillV(RCargoBase **leafs, unsigned size);
};


}  // namespace Toy


#endif // RCARGO_H_
