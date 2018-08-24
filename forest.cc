/**
 * Playground for possible TTree/TBranch interfaces.
 */

#include "ROOT/RTree.hxx"
#include "ROOT/RTreeModel.hxx"

#include <chrono>
#include <memory>
#include <utility>

int main() {
   using ERangeType = ROOT::Experimental::ERangeType;
   using RColumnSink = ROOT::Experimental::RColumnSink;
   using RColumnSource = ROOT::Experimental::RColumnSource;
   using RColumnRange = ROOT::Experimental::RColumnRange;
   using RTree = ROOT::Experimental::RTree;
   using RTreeModel = ROOT::Experimental::RTreeModel;
   using RColumnRawSettings = ROOT::Experimental::RColumnRawSettings;

   std::chrono::high_resolution_clock stopwatch;
   auto start_time = stopwatch.now();

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

   //auto jets = event_model->Branch<std::vector<float>>("jets");

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
    RColumnRawSettings settings("/dev/shm/test.toy");
    //settings.fCompressionSettings = 104;  // ZLIB, level 4
    RTree tree(event_model, RColumnSink::MakeSinkRaw(settings));

    // TODO: value semantics
    for (unsigned i = 0; i < 800; ++i) {
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
  float sum_energy = 0.0;
  //unsigned n_energy_sum_op = 0;
  start_time = stopwatch.now();
  {
    // event_model unused so far
    RTree tree(event_model, RColumnSource::MakeSourceRaw("/dev/shm/test.toy"));

    auto view_h1_py = tree.GetView<float>("h1_py");
    auto view_tracks = tree.GetViewCollection("tracks");
    //auto view_energy = view_tracks.GetView<float>("energy");
    auto view_energy = tree.GetView<std::vector<float>>("tracks/energy");
    auto view_hits = view_tracks.GetViewCollection("hits");
    auto view_hit_x = view_hits.GetView<float>("x");

    // The non-lazy option: the iteration fills automatically an REntry
    for (auto e : tree.GetEntryRange(ERangeType::kLazy)) {
      //std::cout << "Entry range pointer " << e.GetIndex() << std::endl;

      for (auto energy : view_energy(e)) {
        sum_energy += energy;
      }

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
            << ",    sum_energy " << sum_energy
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

   return 0;
}
