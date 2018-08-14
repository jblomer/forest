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

int main() {
   using RColumnSource = ROOT::Experimental::RColumnSource;
   using RTree = ROOT::Experimental::RTree;
   using RTreeModel = ROOT::Experimental::RTreeModel;

   std::chrono::high_resolution_clock stopwatch;
   auto start_time = stopwatch.now();
   auto event_model = std::make_shared<RTreeModel>();
   RTree tree(event_model, RColumnSource::MakeSourceRaw("B2HHH.forest"));

   auto view_h1_is_muon = tree.GetView<int>("h1_is_muon");
   auto view_h2_is_muon = tree.GetView<int>("h2_is_muon");
   auto view_h3_is_muon = tree.GetView<int>("h3_is_muon");
   auto view_h1_px = tree.GetView<double>("h1_px");
   auto view_h1_py = tree.GetView<double>("h1_py");
   auto view_h1_pz = tree.GetView<double>("h1_pz");
   auto view_h1_prob_k = tree.GetView<double>("h1_prob_k");
   auto view_h1_prob_pi = tree.GetView<double>("h1_prob_pi");
   auto view_h1_charge = tree.GetView<int>("h1_charge");
   auto view_h2_px = tree.GetView<double>("h2_px");
   auto view_h2_py = tree.GetView<double>("h2_py");
   auto view_h2_pz = tree.GetView<double>("h2_pz");
   auto view_h2_prob_k = tree.GetView<double>("h2_prob_k");
   auto view_h2_prob_pi = tree.GetView<double>("h2_prob_pi");
   auto view_h2_charge = tree.GetView<int>("h2_charge");
   auto view_h3_px = tree.GetView<double>("h3_px");
   auto view_h3_py = tree.GetView<double>("h3_py");
   auto view_h3_pz = tree.GetView<double>("h3_pz");
   auto view_h3_prob_k = tree.GetView<double>("h3_prob_k");
   auto view_h3_prob_pi = tree.GetView<double>("h3_prob_pi");
   auto view_h3_charge = tree.GetView<int>("h3_charge");

   double sum = 0.0;
   unsigned skipped = 0;

   for (auto e : tree.GetEntryRange(ROOT::Experimental::ERangeType::kLazy)) {
      if (view_h1_is_muon(e) || view_h2_is_muon(e) || view_h3_is_muon(e)) {
         skipped++;
         continue;
      }

      sum += view_h1_px(e) +
         view_h1_py(e) +
         view_h1_pz(e) +
         view_h1_prob_k(e) +
         view_h1_prob_pi(e) +
         double(view_h1_charge(e)) +
         view_h2_px(e) +
         view_h2_py(e) +
         view_h2_pz(e) +
         view_h2_prob_k(e) +
         view_h2_prob_pi(e) +
         double(view_h3_charge(e)) +
         view_h3_px(e) +
         view_h3_py(e) +
         view_h3_pz(e) +
         view_h3_prob_k(e) +
         view_h3_prob_pi(e) +
         double(view_h3_charge(e));
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
