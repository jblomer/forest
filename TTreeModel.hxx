#ifndef TTREEMODEL_H_
#define TTREEMODEL_H_

#include <memory>
#include <string_view>
#include <vector>
#include <utility>

#include "TBranch.hxx"
#include "TTreeEntry.hxx"

namespace Toy {

class TTreeModel {
   bool fFrozen;
   std::vector<std::unique_ptr<TBranch>> fBranches;
   TTreeEntry fDefaultEntry;

public:
   TTreeModel() : fFrozen(false), fDefaultEntry(this) { }

   /**
    * Convenience wrapper around MakeBranch->GetSprout
    */
   template <typename T, typename... ArgsT>
   std::shared_ptr<T> Branch(std::string_view name, ArgsT&&... args) {
     MakeBranch<T>(name);
     return fDefaultEntry.MakeSprout<T>(std::forward<ArgsT>(args)...);
   }

   // TODO: Error handling in ROOT?
   template <typename T>
   void MakeBranch(std::string_view name) {
     // assert !frozen
     TBranchType branch_type = TBranch::MapType<T>();
     fBranches.push_back(std::make_unique<TBranch>(name, branch_type));
   }

   void Freeze() { fFrozen = true; }
};

}  // namespace Toy

#endif  // TTREEMODEL_H_