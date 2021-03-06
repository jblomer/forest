/**
 * Playground for possible TTree/TBranch interfaces.
 */

#include <array>
#include <chrono>
#include <cmath>
#include <experimental/filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <typeinfo>
#include <vector>

/*#include "ROOT/TDirectory.hxx"
#include "TClass.h"
#include "TClassTable.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTree.h"

#include "event.h"*/

#include "RColumnRange.hxx"
#include "RColumnStorage.hxx"
#include "RTree.hxx"
#include "RTreeModel.hxx"


struct Point {
   double x;
   double y;
};


/**
 * In the Toy namespace, we create wrappers around the v6 classes.
 */
/*namespace Toy {


// Implementation notes
//   - arbitrarily nested, split collections
//   - can we ensure sequential writing?
//   - can we recover partial file data if writing fails?
//   - besides random access by event entry, possibility for secondary index desirable (timestamp)

class TDynamicEntry {
};

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

   std::shared_ptr<T> GetTip() {
     return fSprout;
   }

   T& GetTipRef() {
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


class TTreeModel {
public:
   // Templates instead of "BranchFloat" because that allows us to add a user
   // provided event class branch with the same syntax
   template <typename T>
   std::shared_ptr<TBranch<T>> Branch(std::string_view name) {
      TClass *type = TClass::GetClass(typeid(T));
      if (!type) {
         std::cout << "OOPS, unknown type" << std::endl;
         return NULL;
      }
      std::cout << "Building branches from class " << type->GetName() << std::endl;
      // Here we actually figured out the members of the Event class
      //fBranches.Add("/" + name.to_string() + "/fEnergy");
      //return fBranches.Add("/" + name.to_string());
      return std::make_shared<TBranch<T>>("/" + std::string(name.data(), name.size()));
   }

   template <typename T, typename... ArgsT>
   std::shared_ptr<TBranch<T>> MakeBranch(std::string_view name, ArgsT&&... args) {
      auto branch = Branch<T>(name);
      branch->MakeSprout(std::forward<ArgsT>(args)...);
      return branch;
   }

   std::shared_ptr<TBranch<TDynamicEntry>> MakeDynamicBranch(std::string_view name, std::string_view type) {
     auto branch = Branch<TDynamicEntry>(name);
     return branch;
   }
};


class TTree {
   using TDirectory = ROOT::Experimental::TDirectory;

   std::string fName;
   std::shared_ptr<TTreeModel> fModel;
   std::unique_ptr<TTreeMedium> fMedium;
public:
   static const UShort_t kMaxVectorSize = 0;

   TTree() = delete;
   TTree(const TTree &other) = default;

   // FixMe01: Should the constructor become private?
   TTree(std::string_view name, std::shared_ptr<TTreeModel> model, std::unique_ptr<TTreeMedium> &&medium)
     : fName(name), fModel(model), fMedium(std::move(medium))
   {
   }

   TTree(std::string_view name, std::unique_ptr<TTreeMedium> &&medium)
     : fName(name), fMedium(std::move(medium))
   {
   }

   ~TTree() {
   }

   void Fill(UShort_t N = kMaxVectorSize) { }

   static std::shared_ptr<TTree> Make(std::string_view name,
                                      std::shared_ptr<TTreeModel> model,
                                      std::unique_ptr<TTreeMedium> &&medium)
   {
      auto t = std::make_shared<TTree>(name, model, std::move(medium));
      //where.Add(name, t);
      return t;
   }

   static std::shared_ptr<TTree> Open(std::string_view name,
                                      std::unique_ptr<TTreeMedium> &&medium)
   {
      auto t = std::make_shared<TTree>(name, std::move(medium));
      //where.Add(name, t);
      return t;
   }

   static std::shared_ptr<TTree> Open(std::string_view name,
                                      std::shared_ptr<TTreeModel> model,
                                      std::unique_ptr<TTreeMedium> &&medium)
   {
      auto t = std::make_shared<TTree>(name, model, std::move(medium));
      //where.Add(name, t);
      return t;
   }
};

}  // namespace Toy

// Has to be in the namespace of Float_t
template <>
std::shared_ptr<Toy::TBranch<Float_t>> Toy::TTreeModel::Branch<Float_t>(std::string_view name) {
   std::cout << "Adding float_t branch" << std::endl;
   return std::make_shared<Toy::TBranch<Float_t>>("/" + std::string(name.data(), name.size()));
}*/


////////////////////////////////////////////////////////////////////////////////


int main() {
   std::chrono::high_resolution_clock stopwatch;
   auto start_time = stopwatch.now();
   using RColumnRange = Toy::RColumnRange;
   using RRangeType = Toy::RRangeType;
   using RTreeModel = Toy::RTreeModel;
   using RColumnSink = Toy::RColumnSink;
   using RColumnSource = Toy::RColumnSource;
   using RTree = Toy::RTree;
   //using RTreeEntry = Toy::RTreeEntry;

   auto event_model = std::make_shared<RTreeModel>();

   auto h1_px = event_model->Branch<float>("h1_px", 0.0);
   auto h1_py = event_model->Branch<float>("h1_py", 1.0);
   auto h1_pz = event_model->Branch<float>("h1_pz", 2.0);
   auto h2_px = event_model->Branch<float>("h2_px", 3.0);
   auto h2_py = event_model->Branch<float>("h2_py", 4.0);
   auto h2_pz = event_model->Branch<float>("h2_pz", 5.0);
   auto h3_px = event_model->Branch<float>("h3_px", 6.0);
   auto h3_py = event_model->Branch<float>("h3_py", 7.0);
   auto h3_pz = event_model->Branch<float>("h3_pz", 8.0);

   auto jets = event_model->Branch<std::vector<float>>("jets");

   float unsafe;
   event_model->BranchDynamic("unsafe", "float", &unsafe);

   auto hit_model = std::make_shared<RTreeModel>();
   auto hit_x = hit_model->Branch<float>("x", 0.0);
   auto hit_y = hit_model->Branch<float>("y", 0.0);

   auto track_model = std::make_shared<RTreeModel>();
   auto track_energy = track_model->Branch<float>("energy", 0.0);
   auto hits = track_model->BranchCollection("hits", hit_model);
   auto tracks = event_model->BranchCollection("tracks", track_model);

   //auto tracks = tree_model->Branch<std::vector<float>>("tracks");

  {
    RTree tree(event_model, RColumnSink::MakeSinkRaw("/dev/shm/test.toy"));

    // TODO: value semantics
    for (unsigned i = 0; i < 8000000; ++i) {
      for (unsigned t = 0; t < 3; ++t) {
        for (unsigned h = 0; h < 3; ++h) {
          *hit_x = 4.2;
          *hit_y = 8.4;
          hits->Fill();
        }
        *track_energy = 1.2;
        tracks->Fill();
      }
      tree.Fill();
    }
  } // Writing

  auto end_time = stopwatch.now();
  auto diff = end_time - start_time;
  auto milliseconds =
    std::chrono::duration_cast<std::chrono::milliseconds>(diff);
  std::cout << "Tree written, now reading it back, tool "
            << milliseconds.count() << " milliseconds" << std::endl;

  float sum = 0.0;
  //float sum_e = 0.0;
  float sum_x = 0.0;
  //unsigned n_energy_sum_op = 0;
  start_time = stopwatch.now();
  {
    // event_model unused so far
    RTree tree(event_model, RColumnSource::MakeSourceRaw("/dev/shm/test.toy"));

    auto view_h1_py = tree.GetView<float>("h1_py");
    auto view_tracks = tree.GetViewCollection("tracks");
    // TODO: add tracks/ prefix internally
    //auto view_energy = view_tracks.GetView<float>("energy");
    auto view_hits = view_tracks.GetViewCollection("hits");
    auto view_hit_x = view_hits.GetView<float>("x");

    // The non-lazy option: the iteration fills automatically an REntry
    for (auto e : tree.GetEntryRange(RRangeType::kLazy)) {
      //std::cout << "Entry range pointer " << e.GetIndex() << std::endl;
      float v_h1_py = view_h1_py(e);
      RColumnRange track_range = view_tracks.GetRange(e);
      unsigned ntracks = track_range.GetSize();
      sum += v_h1_py;
      if ((e.GetIndex() % 1000000) == 0) {
        std::cout << "entry " << e.GetIndex()
                  << " value " << v_h1_py
                  << ", number of tracks " << ntracks << std::endl;
      }

      for (auto t : track_range) {
        //std::cout << "Track range pointer " << t.GetIndex() << std::endl;
        //sum_e += view_energy(t);
        RColumnRange hit_range = view_hits.GetRange(t);
        for (auto h : hit_range) {
          //std::cout << "Hit range pointer " << h.GetIndex() << std::endl;
          sum_x += view_hit_x(h);
        }
      }
    }

    // The bulk read option
    /*unsigned N = tree.GetNentries();
    std::cout << "Tree has " << N << " entries" << std::endl;
    constexpr unsigned bulksize = 10;
    unsigned limit = N/bulksize;
    float cache[bulksize];
    for (unsigned i = 0; i < limit; ++i) {
      view_h1_py.ReadBulk(i * bulksize, bulksize, cache);

      for (unsigned j = 0; j < bulksize; ++j) {
        sum += cache[j];
      }

      if ((i % (1000000/bulksize)) == 0) {
        std::cout << "entry " << i * bulksize
                  << " value " << cache[0] << std::endl;
      }
    }*/
  }
  end_time = stopwatch.now();
  diff = end_time - start_time;
  milliseconds =
    std::chrono::duration_cast<std::chrono::milliseconds>(diff);
  std::cout << "reading took " << milliseconds.count()
            << " milliseconds,    sum " << sum
            << ",    sum_x " << sum_x
            //<< "   [n_energy_sum_op " << n_energy_sum_op << "]"
            << std::endl;

  // Read a single deeply nested branch independently of entry structure
  sum_x = 0.0;
  start_time = stopwatch.now();
  {
    // event_model unused so far
    RTree tree(event_model, RColumnSource::MakeSourceRaw("/dev/shm/test.toy"));
    auto view_hit_x = tree.GetView<float>("tracks/hits/x");

    // The non-lazy option: the iteration fills automatically an REntry
    for (auto x : view_hit_x) {
      sum_x += x;
    }
  }
  end_time = stopwatch.now();
  diff = end_time - start_time;
  milliseconds =
    std::chrono::duration_cast<std::chrono::milliseconds>(diff);
  std::cout << "independent branch reading took " << milliseconds.count()
            << ",    sum_x " << sum_x
            << std::endl;

   //std::vector<RTreeEntry*> entries{
   //  event_model->GetDefaultEntry(),
   //  event_model->GetDefaultEntry(),
   //  event_model->GetDefaultEntry(),
   //  event_model->GetDefaultEntry(),
   //  event_model->GetDefaultEntry(),
   //  event_model->GetDefaultEntry(),
   //  event_model->GetDefaultEntry(),
   //  event_model->GetDefaultEntry()
   //};
   //for (unsigned i = 0; i < 1000000; ++i) {
   //  tree.FillV(entries.data(), 8);
   //}

   //auto py = tree_model->Branch<int>("py", 0);

   // Implicit call to tree_model->Freeze();
   //TTree tree(tree_model, std::make_unique<TreeSinkRaw>(""));

   //tree.Fill();

   // TODO: make branch of simple type
   // make sub branch
   // make collection branch


   //auto px = tree_model->Fork<Float_t>("px");  /* == MakeBranch<Float_t>("px")
   //auto px = tree_model->MakeBranch<Float_t>("px")->GetLeaf();

   //auto event = /* shared pointer to Event */
   //   tree_model->MakeBranch<Event>("my_event" /*, { constructor arguments for Event }*/)->GetTip();
   //auto& px = tree_model->MakeBranch<Float_t>("px")->GetTipRef();
   //px = 0.42;  // <-- can assign later to px without dereferencing
//
   //tree_model->MakeBranch<Float_t>("chi2", log10(100.0));  // initialized with constant value
//
   //// tree_model->MakeBranch<Float_t>("oops", "0.0");  // type-safe, compile error
//
   //// calculate on fill from other values
   //tree_model->MakeBranch<Float_t>("is_exotic")->Bind(
   //  [event = event]() -> Float_t { return (event->fEnergy < 0) ? 0.9 : 0.1; });

   // Capture shared pointer
   /*auto calibration = std::make_shared<TCalibration>();
   tree_model->MakeBranch<TCalibration>("calibration")->Bind(calibration);*

   // Support decoupled writer modules that don't have the types available at compile time
   auto branch_dynamic = tree_model->MakeDynamicBranch("custom", "TUserClass");
   // Can then be bound to a pointer + size

   // Constructing as a writable tree
   auto tree = TTree::Make("my_tree", tree_model, TTreeMedium::MakeFileSink("/a/b/c"));
   // Possible to use other sinks, e.g. TTreeMedium::CreateCephSink
   // We can reuse the tree model but not the tree medium

   // Scalar filling
   for (auto i = 0; i < 100; i++) {
     // Tips of branches must not be touched during Fill()
     tree->Fill();
     // Serialization done

     // For few branches and single Fill() call, one could also think of an explicit
     // tree->Fill({"mu", mu}, {"e", e});

     // Allow user to flush the TreeMedium if wanted
   }

   // Advanced: vector filling
   // Bind to array views?  Would lose the shared pointer semantics (array can be removed before fill is called)
   // Fill({list of array views?})

   // Interface for writing into a tree, rationale:
   //    - Separate branch creation from SetBranchAddress
   //    - Acknowledge shared management of memory locations that hold the tip of branches
   //    - Type-safe binding
   //    - Allow to bind/write
   //      * lvalues (reference)
   //      * Values and literals (rvalue)
   //      * Callables (thread-safety)
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
   // Q13: Are templated functions preventing good Python bindings?

   // Vectorized: array_view
   // Separate Reader/Writer classes?
   // Fill({"mu", mu},
   //      {"e", e})
   // Folders anbieten für verschiedene Module, die unabhängig füllen
   // Dynamische tree Komposition fürs lesen
   // Trenne lesen und schreiben interface, unterschiedliches mem management

   // Abstrahiere TFile zum rausschreiben

   // MT: Objekte kopieren und dann serialiesern?
   // Nach Fill ist serialisierung fertig, eventuell parallele serialisierung

   // Tree consturctor nimmt TreeModel (array von branches)

   // File Chains: iterierung über mehrere dateien?  über mehrere cluster?
   // IO Layer zwischen tree und file

   //for (auto branch : tree_transient->GetBranches()) {
   //   std::cout << "Listing branch " << branch->GetName() << std::endl;
   //}

   // Other iterators
   //   - Clusters
   //   - Baskets
   //   - Leaves

   // Iterate through events
   //auto view_px = tree->GetView<int>("px");
   //  // Gib shared pointer raus
   //for (auto entry : tree) {
   //  std::cout << view_px() << std::endl;
   //}

   // Iterate through branches

   // TreeMedium provides file chaining functionality
   tree = TTree::Open("my_tree", tree_model, TTreeMedium::MakeFileSource({"/tree1", "/tree2"}));

   for (e : TTree::MakeIterator(tree, kIterEntries)) {
     // Write into storage location given by tree model
     std::cout << px << std::endl;
     std::cout << event->fEnergy << std::endl
   }

   // Hierarchical iteration
   for (cluster : TTree::MakeIterator(tree, kIterClusters)) {
     for (entries : TTree::MakeIterator(c, kIterEntries)) {
     }
   }

   // MakeIterator could possibly create more sophisticated ranges

   // Open without model, allow for lazy views
   tree = TTree::Open("my_tree", TTreeMedium::MakeFileSource({"/tree1", "/tree2"}));

   auto view_px = tree->GetView<int>("px");
   auto view_chi2 = tree->GetView<int>("chi2");

   for (e : TTree::MakeIterator(tree, kIterEntries)) {
     if (view_px() > 1.0) {
       std::cout << view_chi2() << std::endl;
     }
   }*/

   return 0;
}
