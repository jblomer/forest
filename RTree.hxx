#ifndef RTREE_H_
#define RTREE_H_

#include <memory>
#include <vector>

#include "RBranch.hxx"

namespace Toy {

class RTreeEntry;
class RTreeModel;

class RTree {
   using BranchCollection = std::vector<RBranch>;

   std::shared_ptr<RTreeModel> fModel;
   BranchCollection fBranches;

   unsigned fNentries;

public:
   RTree(std::shared_ptr<RTreeModel> model);

   unsigned GetNentries() { return fNentries; }

   void Fill(RTreeEntry *entry = NULL);
};

}  // namespace Toy

#endif  // RTREE_H_
