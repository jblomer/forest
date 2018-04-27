#ifndef RTREEMODEL_H_
#define RTREEMODEL_H_

#include <memory>
#include <string_view>
#include <vector>
#include <utility>

#include <atomic>

#include "RBranchModel.hxx"
#include "RTreeEntry.hxx"

namespace Toy {

class RTreeModel {
   using ModelId = unsigned;

   static std::atomic<ModelId> gModelId;

   ModelId fModelId;
   std::vector<std::unique_ptr<RBranchModel>> fBranches;
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
     // assert !frozen
     RBranchType branch_type = RBranchModel::MapType<T>();
     fBranches.push_back(std::make_unique<RBranchModel>(name, branch_type));
   }

   // Model can be cloned and as long as it stays frozen the model id
   // is the same
   void Freeze() { if (fModelId == 0) fModelId = ++gModelId; }
   bool IsFrozen() { return fModelId > 0; }
};

}  // namespace Toy

#endif  // RTREEMODEL_H_