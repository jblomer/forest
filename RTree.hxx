#ifndef RTREE_H_
#define RTREE_H_

#include <memory>
#include <vector>

#include "RTreeColumn.hxx"
#include "RTreeModel.hxx"

namespace Toy {

class RTreeEntry;
class RTreeSource;

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

   unsigned GetNentries() { return fNentries; }

   void Fill() { Fill(&(fModel->fDefaultEntry)); }
   void Fill(RTreeEntry *entry);
   void FillV(RTreeEntry **entry, unsigned size);
};

}  // namespace Toy

#endif  // RTREE_H_
