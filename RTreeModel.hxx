#ifndef RTREEMODEL_H_
#define RTREEMODEL_H_

#include <cassert>
#include <memory>
#include <string_view>
#include <vector>
#include <utility>

#include <atomic>

#include "RBranch.hxx"
#include "RTreeEntry.hxx"

namespace Toy {

class RTreeModel {
   friend class RTree;

   using ModelId = unsigned;

   static std::atomic<ModelId> gModelId;

   ModelId fModelId;
   RTreeEntry fDefaultEntry;
   RBranch<RBranchRoot> fRootBranch;

public:
   RTreeModel() : fModelId(0), fDefaultEntry(this) { }

   template <typename T, typename... ArgsT>
   std::shared_ptr<T> Branch(std::string_view name, ArgsT&&... args) {
     assert(!IsFrozen());

     RBranch<T> *branch = new RBranch<T>(name);
     fRootBranch.Attach(branch);

     return fDefaultEntry.AddLeaf<T>(branch, std::forward<ArgsT>(args)...);
   }

   // Model can be cloned and as long as it stays frozen the model id
   // is the same
   void Freeze() { if (fModelId == 0) fModelId = ++gModelId; }
   bool IsFrozen() { return fModelId > 0; }
   ModelId GetModelId() { return fModelId; }
};

}  // namespace Toy

#endif  // RTREEMODEL_H_