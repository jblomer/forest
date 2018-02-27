/**
 * Playground for possible TTree/TBranch interfaces.
 */

#include <cmath>
#include <functional>
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


// Implementation notes
//   - arbitrarily nested, split collections
//   - can we ensure sequential writing?
//   - can we recover partial file data if writing fails?
//   - besides random access by event entry, possibility for secondary index desirable (timestamp)

template <typename T>
class TBranch {
   std::string fName;

public:
   TBranch(std::string_view name) : fName(name) { }
   std::string GetName() { return fName; }


   // Bind could be the right place to look whether all branches are bound,
   // so that Fill() only needs to check one bool.

   // Equivalent to SetBranchAddress
   void Bind(T &data) {
     std::cout << "binding reference of type " << typeid(data).name() << std::endl;
     std::cout << "   ... current value " << data << std::endl;
   }

   // Write a constant value
   void Bind(const T &data) {
     std::cout << "binding constant of type " << typeid(data).name() << std::endl;
     std::cout << "   ... value " << data << std::endl;
   }

   // Evaluate a function on Fill
   void Bind(std::function<T()> fn) {
     std::cout << "binding lambda" << std::endl;
     std::cout << "   ... evaluating to " << fn() << std::endl;
   }
};


/*class TBranchCollection {
   std::vector<std::unique_ptr<TBranch>> fElements;
public:
   struct Iterator {
      const TBranchCollection *fCollection;
      unsigned fPos;
   public:
      Iterator(const TBranchCollection *collection, unsigned pos)
         : fCollection(collection), fPos(pos) { }
      bool operator != (const Iterator &other) const { return fPos != other.fPos; }
      TBranch *operator* () const { return fCollection->fElements[fPos].get(); }
      const Iterator &operator++ () { ++fPos; return *this; }
   };

   TBranch *Add(std::string_view name) {
      auto element = std::make_unique<TBranch>(name);
      TBranch *observer = element.get();
      fElements.emplace_back(std::move(element));
      return observer;
   }

   Iterator begin() const { return Iterator(this, 0); }
   Iterator end() const { return Iterator(this, fElements.size()); }
};*/


class TTree {
   using TDirectory = ROOT::Experimental::TDirectory;

   ::TTree *fClassicTree;
   // Should TTree own the TBranch objects?
   //TBranchCollection fBranches;

public:
   TTree() = delete;

   // FixMe01: Should the constructor become private?
   TTree(std::string_view name)
      : fClassicTree(new ::TTree(name.to_string().c_str(), ""))
   {
      //fBranches.Add("/");  // tree's root branch (stem)
   };

   ~TTree() {
      delete fClassicTree;
   }

   // Tamplates instead of "BranchFloat" because that allows us to add a user
   // provided event class branch with the same syntax
   template <typename T>
   TBranch<T> *Branch(std::string_view name) {
      TClass *type = TClass::GetClass(typeid(T));
      if (!type) {
         std::cout << "OOPS, unknown type" << std::endl;
         return NULL;
      }
      std::cout << "Building branches from class " << type->GetName() << std::endl;
      // Here we actually figured out the members of the Event class
      //fBranches.Add("/" + name.to_string() + "/fEnergy");
      //return fBranches.Add("/" + name.to_string());
      return new TBranch<T>("/" + name.to_string());
   }

   /*(const TBranchCollection &GetBranches() {
      return fBranches;
   }*/

   void Print() { fClassicTree->Print(); }
   void Fill() { }

   static std::shared_ptr<TTree> Create(std::string_view name, TDirectory &where) {
      auto t = std::make_shared<TTree>(name);
      where.Add(name, t);
      return t;
   }
};

}  // namespace Toy

// Has to be in the namespace of Float_t
template <>
Toy::TBranch<Float_t> *Toy::TTree::Branch<Float_t>(std::string_view name) {
   std::cout << "Adding float_t branch" << std::endl;
   //return fBranches.Add("/" + name.to_string());
   return new TBranch<Float_t>("/" + name.to_string());
}


////////////////////////////////////////////////////////////////////////////////


int main() {
   using TDirectory = ROOT::Experimental::TDirectory;
   using TTree = Toy::TTree;

   if (!TClassTable::GetDict("Event")) {
      gSystem->Load("./libEvent.so");
   }

   // Constructing
   auto tree_persistent = TTree::Create("MyPersistentTree", TDirectory::Heap());  // <-- shared_ptr
   auto tree_transient = std::make_unique<TTree>("MyTransientTree");
   // TTree::CreateFromFolder ?

   // Interface for writing into a tree, rationale:
   //    - Separate branch creation from SetBranchAddress
   //    - Type-safe binding
   //    - Allow to bind/write
   //      * lvalues (reference)
   //      * Values and literals (rvalue)
   //      * Callables
   //
   // Q05: Can we bind variables in such a way that we know they are not
   //      invalid when Fill() is called?  We probably do not want shared
   //      pointers here.
   //
   // Q03: Should we have "bulk filling"?  -->  vectorized fill interface should cover "bulk I/O"
   // Q06: How should addition of a branch work
   auto branch_px = tree_transient->Branch<Float_t>("px");  // <-- OK
   tree_transient->Branch<Event>("EventBranch");  // <-- OK, processed by cling
   tree_transient->Branch<Point>("oops");  // <-- no reflection info available

   Float_t chi2 = 1.0;
   tree_transient->Branch<Float_t>("chi2")->Bind(chi2);
   branch_px->Bind(0.0);
   branch_px->Bind(log10(100.0));
   branch_px->Bind([]() -> Float_t { return 42.0; });

   // branch_px->Bind("0.0");  <-- compiler error

   // Scalar filling
   for (auto i = 0; i < 100; i++) {
     tree_transient->Fill();
   }

   // Vector filling interface


   // Iterate over available branches in the tree
   // Q04: Should we be able to iterate over branches of a particular nesting
   // level only?

   /*for (auto branch : tree_transient->GetBranches()) {
      std::cout << "Listing branch " << branch->GetName() << std::endl;
   }*/

   // Other iterators
   //   - Clusters
   //   - Baskets
   //   - Leaves

   // Iterate through events
   // for (auto WhatTypeShouldIBe : tree_transient) {
   // }

   // Iterate through branches

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
