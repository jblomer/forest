#ifndef TTREE_H_
#define TTREE_H_

#include <memory>

namespace Toy {

class TTreeModel;

class TTree {
   std::shared_ptr<TTreeModel> fModel;
public:
   TTree(std::shared_ptr<TTreeModel> model);
};

}  // namespace Toy

#endif  // TTREE_H_
