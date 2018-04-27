#ifndef TTREE_H_
#define TTREE_H_

#include <memory>

namespace Toy {

class TTreeModel;

class TTree {
   std::shared_ptr<TTreeModel> fModel;

   unsigned fNentries;

public:
   TTree(std::shared_ptr<TTreeModel> model);

   unsigned GetNentries() { return fNentries; }

   void Fill();
};

}  // namespace Toy

#endif  // TTREE_H_
