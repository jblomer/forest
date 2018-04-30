#ifndef RTREEMODEL_H_
#define RTREEMODEL_H_

#include <cassert>
#include <memory>
#include <string_view>
#include <vector>
#include <utility>

#include <atomic>

#include "RBranchModel.hxx"
#include "RTreeEntry.hxx"

namespace Toy {

class RTreeModel {
   friend class RTree;

   using ModelId = unsigned;
   using BranchModelContainer = std::vector<RBranchModel>;

   static std::atomic<ModelId> gModelId;

   ModelId fModelId;
   BranchModelContainer fBranches;
   RTreeEntry fDefaultEntry;

public:
   RTreeModel() : fModelId(0), fDefaultEntry(this) { }

   /**
    * Convenience wrapper
    */
   template <typename T, typename... ArgsT>
   std::shared_ptr<T> Branch(std::string_view name, ArgsT&&... args) {
     MakeBranch<T>(name);
     return fDefaultEntry.AddLeaf<T>(std::forward<ArgsT>(args)...);
   }

   // TODO: Error handling in ROOT?
   template <typename T>
   void MakeBranch(std::string_view name) {
     assert(!IsFrozen());

     RBranchType branch_type = RLeaf<T>::MapType();
     fBranches.emplace_back(RBranchModel(name, branch_type));
   }

   // Model can be cloned and as long as it stays frozen the model id
   // is the same
   void Freeze() { if (fModelId == 0) fModelId = ++gModelId; }
   bool IsFrozen() { return fModelId > 0; }
   ModelId GetModelId() { return fModelId; }

   BranchModelContainer &GetBranchModelsRef() { return fBranches; }
};

}  // namespace Toy

#endif  // RTREEMODEL_H_