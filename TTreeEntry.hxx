#ifndef TTREEENTRY_H_
#define TTREEENTRY_H_

#include <memory>
#include <vector>
#include <utility>

#include "RLeaf.hxx"

namespace Toy {

class TTreeModel;

/**
 * Collection of Sprouts: how guarantuee consistency with treemodel?
 */
class TTreeEntry {
   TTreeModel *fModel;
   std::vector<std::unique_ptr<RLeafBase>> fLeafs;

public:
   TTreeEntry(TTreeModel *model) : fModel(model) { }

   template <typename T, typename... ArgsT>
   std::shared_ptr<T> AddLeaf(ArgsT&&... args) {
     auto leaf = std::make_unique<RLeaf<T>>(std::forward<ArgsT>(args)...);
     auto value_ptr = leaf->Get();
     fLeafs.emplace_back(std::move(leaf));
     return value_ptr;
   }
};

}  // namespace Toy

#endif  // TTREEENTYR_H_