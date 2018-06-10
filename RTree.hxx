#ifndef RTREE_H_
#define RTREE_H_

#include <memory>
#include <string_view>
#include <vector>

#include "RTreeColumn.hxx"
#include "RTreeModel.hxx"
#include "RTreeView.hxx"

namespace Toy {

class REntryRange;
class RTree;
class RTreeEntry;
class RTreeSource;

// TODO: ROTree and RITree

enum class RRangeType {
  kLazy,
  kSync,
};

class RTree {
   std::unique_ptr<RTreeSink> fSink;
   std::unique_ptr<RTreeSource> fSource;
   std::shared_ptr<RTreeModel> fModel;
   RTreeColumnCollection fColumns;

   unsigned fNentries;

public:
  RTree(std::shared_ptr<RTreeModel> model,
        std::unique_ptr<RTreeSink> sink);
  RTree(std::shared_ptr<RTreeModel> model,
        std::unique_ptr<RTreeSource> source);
  ~RTree();

  unsigned GetNentries() const { return fNentries; }
  REntryRange GetEntryRange(RRangeType type, RTreeEntry* entry = nullptr);

  template <typename T>
  RTreeView<T> GetView(std::string_view name) {
    auto branch = new RBranch<T>(name);  // TODO not with raw pointer
    branch->GenerateColumns(fSource.get(), nullptr);
    return RTreeView<T>(branch);
  }

  void Fill() { Fill(&(fModel->fDefaultEntry)); }
  void Fill(RTreeEntry *entry);
  void FillV(RTreeEntry **entry, unsigned size);
};  // RTree


}  // namespace Toy

#endif  // RTREE_H_
