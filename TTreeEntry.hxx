#ifndef TTREEENTRY_H_
#define TTREEENTRY_H_

#include <memory>
#include <vector>
#include <utility>

#include "TSprout.hxx"

namespace Toy {

class TTreeModel;

/**
 * Collection of Sprouts: how guarantuee consistency with treemodel?
 */
class TTreeEntry {
   TTreeModel *fModel;
   std::vector<std::unique_ptr<TSproutBase>> fSprouts;

public:
   TTreeEntry(TTreeModel *model) : fModel(model) { }

   template <typename T, typename... ArgsT>
   std::shared_ptr<T> MakeSprout(ArgsT&&... args) {
     auto sprout = std::make_unique<TSprout<T>>(std::forward<ArgsT>(args)...);
     auto value_ptr = sprout->Get();
     fSprouts.emplace_back(std::move(sprout));
     return value_ptr;
   }
};

}  // namespace Toy

#endif  // TTREEENTYR_H_