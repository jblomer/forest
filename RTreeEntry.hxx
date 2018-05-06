#ifndef RTREEENTRY_H_
#define RTREEENTRY_H_

#include <memory>
#include <vector>
#include <utility>

#include "RLeaf.hxx"

namespace Toy {

class RTreeModel;

class RTreeEntry {
   friend class RTree;

   using LeafCollection = std::vector<std::unique_ptr<RLeafBase>>;

   RTreeModel *fModel;
   LeafCollection fLeafs;

   bool HasCompatibleModelId(RTreeModel *model);

   LeafCollection& GetLeafsRef() { return fLeafs; }

public:
   RTreeEntry(RTreeModel *model) : fModel(model) { }

   template <typename T, typename... ArgsT>
   std::shared_ptr<T> AddLeaf(ArgsT&&... args) {
     auto leaf = std::make_unique<RLeaf<T>>(std::forward<ArgsT>(args)...);
     auto value_ptr = leaf->Get();
     fLeafs.emplace_back(std::move(leaf));
     return value_ptr;
   }

   template <typename T, typename... ArgsT>
   void AddLeafCaptured(ArgsT&&... args) {
     auto leaf =
       std::make_unique<RLeafCaptured<T>>(std::forward<ArgsT>(args)...);
     fLeafs.emplace_back(std::move(leaf));
   }

   bool IsCompatibleWith(RTreeModel *model) {
     return (model == fModel) || HasCompatibleModelId(model);
   }
};

}  // namespace Toy

#endif  // TTREEENTYR_H_