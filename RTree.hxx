#ifndef RTREE_H_
#define RTREE_H_

#include <memory>

namespace Toy {

class RTreeEntry;
class RTreeModel;

class RTree {
   std::shared_ptr<RTreeModel> fModel;

   unsigned fNentries;

public:
   RTree(std::shared_ptr<RTreeModel> model);

   unsigned GetNentries() { return fNentries; }

   void Fill(RTreeEntry *entry = NULL);
};

}  // namespace Toy

#endif  // RTREE_H_
