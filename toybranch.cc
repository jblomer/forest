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
   std::shared_ptr<T> fSprout;

public:
   TBranch(std::string_view name) : fName(name) { }
   std::string GetName() { return fName; }

   template <typename... ArgsT>
   std::shared_ptr<T> MakeSprout(ArgsT&&... args) {
     fSprout = std::make_shared<T>(std::forward<ArgsT>(args)...);
     return fSprout;
   }

   T& GetSproutRef() {
     if (!fSprout) fSprout = std::make_shared<T>();
     return *fSprout;
   }

   TBranch<T>* SetSprout(const T&& value) {
     if (!fSprout) fSprout = std::make_shared<T>(value);
     else *fSprout = value;
     std::cout << "Fixed sprout to " << *fSprout << std::endl;
     return this;
   }


   // Evaluate a function on Fill
   void Bind(std::function<T()> fn) {
     std::cout << "binding lambda" << std::endl;
     std::cout << "   ... evaluating to " << fn() << std::endl;
   }

   // Write a vector of values
   void Bind(const T* data_v, unsigned sz) {
     std::cout << "binding vector of size " << sz << std::endl;
   }
};


class TTree {
   using TDirectory = ROOT::Experimental::TDirectory;

   ::TTree *fClassicTree;
   // Should TTree own the TBranch objects?

public:
   static const UShort_t kMaxVectorSize = 0;

   TTree() = delete;
   TTree(const TTree &other) = default;

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
   void Fill(UShort_t N = kMaxVectorSize) { }

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
   // Is TTree::CreateFromFolder needed/used?

   // Interface for writing into a tree, rationale:
   //    - Separate branch creation from SetBranchAddress
   //    - Acknowledge shared management of memory locations that hold the tip of branches
   //    - Type-safe binding
   //    - Allow to bind/write
   //      * lvalues (reference)
   //      * Values and literals (rvalue)
   //      * Callables
   //
   // Q05: Can we bind variables in such a way that we know they are not
   //      invalid when Fill() is called?  Is there a reason for not using shared pointers?
   //
   // Q03: Should we have "bulk filling"?  -->  vectorized fill interface should cover "bulk I/O"
   // Q06: How should addition of a branch work?  Is it the only use case of friend trees?
   // Q07: Should we have UUIDs for branches and trees?
   // Q08: What does "multi-threaded filling" mean?  Do we need it?  Which thread model applies?
   // Q09: Do we need branch-wise (row-wise) filling?  Perhaps overkill, requires a lot of checks
   // Q10: Can we provide type-safe filling in experiment frameworks?  Is the current type system good enough for it?
   // Q11: Do we need to construct TTree with a TFile to make sure auto-saving works?
   // Q12: Are we happy with the naming Tree, Branch, Basket, etc.?  Should we use Row, Column, DataSet, etc.?

   auto tree = std::move(tree_transient);

   auto event = /* shared pointer to Event */
      tree->Branch<Event>("EventBranch")->MakeSprout(/* constructor arguments for Event */);
   // We probably also want to be able to acquire an existing shared pointer

   tree->Branch<Point>("oops");  // <-- no reflection info available

   auto& px = tree->Branch<Float_t>("px")->GetSproutRef();
   px = 0.42;  // <-- can assign to px without dereferencing

   auto branch_chi2 = tree->Branch<Float_t>("chi2")->SetSprout(1.2);
   branch_chi2->SetSprout(log10(100.0));
   // branch_chi2->SetSprout("0.0");   <-- Compile error
   branch_chi2->Bind([]() -> Float_t { return 42.0; });

   // Scalar filling
   for (auto i = 0; i < 100; i++) {
     tree->Fill();  // <-- implicit tree->Fill(TTree:kVecMax)
   }

   // Vector filling
   /*for (auto i = 0; i < 10; i++) {
     tree_transient->Fill(10);  // <-- Fails at runtime because there is only a scalar chi2
   }
   std::vector<Float_t> chi2_v = {0.0, 0.1, 0.2, 0.3};
   branch_px->Bind(chi2_v.data(), chi2_v.size());
   for (auto i = 0; i < 10; i++) {
      tree_transient->Fill(10);  // <-- Fails at runtime because there is only a scalar chi2
   }*/

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
   auto view_px = tree->GetView<int>("px");
   for (auto entry : tree) {
     std::cout << view_px() << std::endl;
   }

   // Iterate through branches

   tree->Print();

   return 0;
}
