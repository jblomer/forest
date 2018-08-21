/**
 * Playground for possible TTree/TBranch interfaces.
 */

#include "ROOT/RTree.hxx"
#include "ROOT/RTreeModel.hxx"

#include "TBranch.h"
#include "TChain.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <utility>

int main(int argc, char **argv) {
   using RColumnSource = ROOT::Experimental::RColumnSource;
   using RTree = ROOT::Experimental::RTree;
   using RTreeModel = ROOT::Experimental::RTreeModel;
   using RColumnRange = ROOT::Experimental::RColumnRange;

   std::chrono::high_resolution_clock stopwatch;
   auto start_time = stopwatch.now();
   auto event_model = std::make_shared<RTreeModel>();
   RTree tree(event_model, RColumnSource::MakeSourceRaw(argv[1]));

   auto view_kaons = tree.GetViewCollection("kaons");
   auto view_h_is_muon = view_kaons.GetView<int>("h_is_muon");
   auto view_h_px =      view_kaons.GetView<double>("h_px");
   auto view_h_py =      view_kaons.GetView<double>("h_py");
   auto view_h_pz =      view_kaons.GetView<double>("h_pz");
   auto view_h_prob_k =  view_kaons.GetView<double>("h_prob_k");
   auto view_h_prob_pi = view_kaons.GetView<double>("h_prob_pi");
   auto view_h_charge =  view_kaons.GetView<int>("h_charge");

   double sum = 0.0;
   unsigned skipped = 0;

   for (auto e : tree.GetEntryRange(ROOT::Experimental::ERangeType::kLazy)) {
      RColumnRange kaon_range = view_kaons.GetRange(e);
      bool is_muon = false;
      for (auto k : kaon_range) {
         if (view_h_is_muon(k)) {
            is_muon = true;
            break;
         }
      }

      if (is_muon) {
         skipped++;
         continue;
      }

      for (auto k : kaon_range) {
         sum += view_h_px(k) +
            view_h_py(k) +
            view_h_pz(k) +
            view_h_prob_k(k) +
            view_h_prob_pi(k) +
            double(view_h_charge(k));
      }
   }

   auto end_time = stopwatch.now();
   auto diff = end_time - start_time;
   auto milliseconds =
    std::chrono::duration_cast<std::chrono::milliseconds>(diff);
   std::cout << "reading took " << milliseconds.count()
            << " milliseconds,    sum " << sum
            << "   skipped: " << skipped
            << std::endl;

   return 0;
}
