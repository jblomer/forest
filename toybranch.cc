/**
 * Playground for possible TTree/TBranch interfaces.
 */

#include <iostream>
#include <memory>
#include <string_view>

#include "ROOT/TDirectory.hxx"
#include "TTree.h"


/**
 * In the Toy namespace, we create wrappers around the v6 classes.
 */
namespace Toy {

class TTree {
   using TDirectory = ROOT::Experimental::TDirectory;

   ::TTree *fClassicTree;

public:
   TTree() = delete;

   // FixMe01: Should the constructor become private?
   TTree(std::string_view name)
      : fClassicTree(new ::TTree(name.to_string().c_str(), ""))
   { };

   ~TTree() {
      delete fClassicTree;
   }

   void Print() { fClassicTree->Print(); }
   void Fill() { fClassicTree->Print(); }

   static std::shared_ptr<TTree> Create(std::string_view name, TDirectory &where) {
      auto t = std::make_shared<TTree>(name);
      where.Add(name, t);
      return t;
   }
};

}  // namespace Toy


int main() {
   using TDirectory = ROOT::Experimental::TDirectory;
   using TTree = Toy::TTree;

   // Constructing
   auto tree_persistent = TTree::Create("MyPersistentTree", TDirectory::Heap());
   auto tree_transient = std::make_unique<TTree>("MyTransientTree");
   // TTree::CreateFromFolder ?

   /*T->Branch("px",&px,"px/F");
   T->Branch("py",&py,"py/F");
   T->Branch("pz",&pz,"pz/F");
   T->Branch("random",&random,"random/D");
   T->Branch("i",&i,"i/s");
   T->SetCircular(20000);
   for (i = 0; i < 65000; i++) {
      r.Rannor(px,py);
      pz = px*px + py*py;
      random = r.Rndm();
      T->Fill();
   }*/
   for (unsigned i = 0; i < 8; ++i) {
      tree_transient->Fill();
   }
   tree_transient->Print();

   return 0;
}
