/**
 * Playground for possible TTree/TBranch interfaces.
 */

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <typeinfo>
#include <vector>

#include "ROOT/TDirectory.hxx"
#include "TClass.h"
#include "TClassTable.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTree.h"

#include "event.h"


struct Point {
   double x;
   double y;
};


/**
 * In the Toy namespace, we create wrappers around the v6 classes.
 */
namespace Toy {

class TBranch {
   std::string fName;

public:
   TBranch(std::string_view name) : fName(name) { }
   std::string GetName() { return fName; }
};


class TBranchCollection {
   std::vector<TBranch> fElements;
public:
   void Add(const TBranch &branch) { fElements.emplace_back(branch); }

   std::vector<TBranch>::const_iterator begin() const {
      return fElements.begin();
   }
   std::vector<TBranch>::const_iterator end() const {
      return fElements.end();
   }
};


class TTree {
   using TDirectory = ROOT::Experimental::TDirectory;

   ::TTree *fClassicTree;
   // Should TTree own the TBranch objects?
   TBranchCollection fBranches;

public:
   TTree() = delete;

   // FixMe01: Should the constructor become private?
   TTree(std::string_view name)
      : fClassicTree(new ::TTree(name.to_string().c_str(), ""))
   {
      fBranches.Add(TBranch("/"));  // tree's root branch (stem)
   };

   ~TTree() {
      delete fClassicTree;
   }

   // Tamplates instead of "BranchFloat" because that allows us to add a user
   // provided event class branch with the same syntax
   template <typename T>
   void Branch(std::string_view name) {
      TClass *type = TClass::GetClass(typeid(T));
      if (!type) {
         std::cout << "OOPS, unknown type" << std::endl;
         return;
      }
      std::cout << "Building branches from class " << type->GetName() << std::endl;
      // Here we actually figured out the members of the Event class
      fBranches.Add(TBranch("/" + name.to_string()));
      fBranches.Add(TBranch("/" + name.to_string() + "/fEnergy"));
   }

   const TBranchCollection &GetBranches() {
      return fBranches;
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

// Has to be in the namespace of Float_t
template <>
void Toy::TTree::Branch<Float_t>(std::string_view name) {
   std::cout << "Adding float_t branch" << std::endl;
   fBranches.Add(TBranch("/" + name.to_string()));
}


int main() {
   using TDirectory = ROOT::Experimental::TDirectory;
   using TTree = Toy::TTree;

   if (!TClassTable::GetDict("FlatEvent")) {
      gSystem->Load("./libEvent.so");
   }

   // Constructing
   auto tree_persistent = TTree::Create("MyPersistentTree", TDirectory::Heap());
   auto tree_transient = std::make_unique<TTree>("MyTransientTree");
   // TTree::CreateFromFolder ?

   // Q02: Should we split createion of the branches (schema) from binding
   // variables to them for filling?
   // Q03: Should we have "bulk filling"?
   tree_transient->Branch<int>("oops");  // <-- no specialization for int
   tree_transient->Branch<Float_t>("px");  // <-- OK
   tree_transient->Branch<Event>("EventBranch");  // <-- OK, processed by cling
   tree_transient->Branch<Point>("oops");  // <-- no reflection info available

   for (auto branch : tree_transient->GetBranches()) {
      std::cout << "Listing branch " << branch.GetName() << std::endl;
   }

   // Iterate through events
   // for (auto WhatTypeShouldIBe : tree_transient) {
   // }

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

   /*for (unsigned i = 0; i < 8; ++i) {
      tree_transient->Fill();
   }*/
   tree_transient->Print();

   return 0;
}
