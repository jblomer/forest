#ifndef RTREE_H_
#define RTREE_H_

#include <memory>
#include <vector>

#include "RBranch.hxx"
#include "RTreeModel.hxx"

namespace Toy {

class RTreeEntry;
class RTreeSink;

class RTree {
   using BranchCollection = std::vector<std::unique_ptr<RBranch>>;

   std::unique_ptr<RTreeSink> fSink;
   std::shared_ptr<RTreeModel> fModel;
   BranchCollection fBranches;

   unsigned fNentries;

public:
   RTree(std::shared_ptr<RTreeModel> model,
         std::unique_ptr<RTreeSink> sink);

   unsigned GetNentries() { return fNentries; }

   void Fill() { Fill(&(fModel->fDefaultEntry)); }
   void Fill(RTreeEntry *entry);
};

}  // namespace Toy

#endif  // RTREE_H_