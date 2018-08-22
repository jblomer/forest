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

#include <unistd.h>

using RTree = ROOT::Experimental::RTree;

struct Result {
   Result() : sum(0.0), skipped(0) { }
   double sum;
   unsigned skipped;
};


Result ReadScalar(RTree* tree) {
   auto view_h1_is_muon = tree->GetView<int>("H1_isMuon");
   auto view_h2_is_muon = tree->GetView<int>("H2_isMuon");
   auto view_h3_is_muon = tree->GetView<int>("H3_isMuon");
   auto view_h1_px = tree->GetView<double>("H1_PX");
   auto view_h1_py = tree->GetView<double>("H1_PY");
   auto view_h1_pz = tree->GetView<double>("H1_PZ");
   auto view_h1_prob_k = tree->GetView<double>("H1_ProbK");
   auto view_h1_prob_pi = tree->GetView<double>("H1_ProbPi");
   auto view_h1_charge = tree->GetView<int>("H1_Charge");
   auto view_h2_px = tree->GetView<double>("H2_PX");
   auto view_h2_py = tree->GetView<double>("H2_PY");
   auto view_h2_pz = tree->GetView<double>("H2_PZ");
   auto view_h2_prob_k = tree->GetView<double>("H2_ProbK");
   auto view_h2_prob_pi = tree->GetView<double>("H2_ProbPi");
   auto view_h2_charge = tree->GetView<int>("H2_Charge");
   auto view_h3_px = tree->GetView<double>("H3_PX");
   auto view_h3_py = tree->GetView<double>("H3_PY");
   auto view_h3_pz = tree->GetView<double>("H3_PZ");
   auto view_h3_prob_k = tree->GetView<double>("H3_ProbK");
   auto view_h3_prob_pi = tree->GetView<double>("H3_ProbPi");
   auto view_h3_charge = tree->GetView<int>("H3_Charge");

   Result result;

   for (auto e : tree->GetEntryRange(ROOT::Experimental::ERangeType::kLazy)) {
      if (view_h1_is_muon(e) || view_h2_is_muon(e) || view_h3_is_muon(e)) {
         result.skipped++;
         continue;
      }

      result.sum +=
         view_h1_px(e) +
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

   return result;
}


Result ReadVectorized(RTree* tree) {
   auto view_h1_is_muon = tree->GetView<int>("H1_isMuon");
   auto view_h2_is_muon = tree->GetView<int>("H2_isMuon");
   auto view_h3_is_muon = tree->GetView<int>("H3_isMuon");
   auto view_h1_px = tree->GetView<double>("H1_PX");
   auto view_h1_py = tree->GetView<double>("H1_PY");
   auto view_h1_pz = tree->GetView<double>("H1_PZ");
   auto view_h1_prob_k = tree->GetView<double>("H1_ProbK");
   auto view_h1_prob_pi = tree->GetView<double>("H1_ProbPi");
   auto view_h1_charge = tree->GetView<int>("H1_Charge");
   auto view_h2_px = tree->GetView<double>("H2_PX");
   auto view_h2_py = tree->GetView<double>("H2_PY");
   auto view_h2_pz = tree->GetView<double>("H2_PZ");
   auto view_h2_prob_k = tree->GetView<double>("H2_ProbK");
   auto view_h2_prob_pi = tree->GetView<double>("H2_ProbPi");
   auto view_h2_charge = tree->GetView<int>("H2_Charge");
   auto view_h3_px = tree->GetView<double>("H3_PX");
   auto view_h3_py = tree->GetView<double>("H3_PY");
   auto view_h3_pz = tree->GetView<double>("H3_PZ");
   auto view_h3_prob_k = tree->GetView<double>("H3_ProbK");
   auto view_h3_prob_pi = tree->GetView<double>("H3_ProbPi");
   auto view_h3_charge = tree->GetView<int>("H3_Charge");

   Result result;

   unsigned N = tree->GetNentries();
   unsigned blksize = 10;
   unsigned limit = N / blksize;
   unsigned last_blksize = N % blksize;
   if (last_blksize != 0) {
      limit++;
   }

   int cache_h1_is_muon[blksize];
   int cache_h2_is_muon[blksize];
   int cache_h3_is_muon[blksize];
   double cache_h1_px[blksize];
   double cache_h1_py[blksize];
   double cache_h1_pz[blksize];
   double cache_h1_prob_k[blksize];
   double cache_h1_prob_pi[blksize];
   int cache_h1_charge[blksize];
   double cache_h2_px[blksize];
   double cache_h2_py[blksize];
   double cache_h2_pz[blksize];
   double cache_h2_prob_k[blksize];
   double cache_h2_prob_pi[blksize];
   int cache_h2_charge[blksize];
   double cache_h3_px[blksize];
   double cache_h3_py[blksize];
   double cache_h3_pz[blksize];
   double cache_h3_prob_k[blksize];
   double cache_h3_prob_pi[blksize];
   int cache_h3_charge[blksize];

   for (unsigned i = 0; i < limit; ++i) {
      if ((i == limit-1) && (last_blksize != 0)) blksize = last_blksize;

      view_h1_is_muon.ReadBulk(i * blksize, blksize, cache_h1_is_muon);
      view_h2_is_muon.ReadBulk(i * blksize, blksize, cache_h2_is_muon);
      view_h3_is_muon.ReadBulk(i * blksize, blksize, cache_h3_is_muon);

      view_h1_px.ReadBulk(i * blksize, blksize, cache_h1_px);
      view_h1_py.ReadBulk(i * blksize, blksize, cache_h1_py);
      view_h1_pz.ReadBulk(i * blksize, blksize, cache_h1_pz);
      view_h1_prob_k.ReadBulk(i * blksize, blksize, cache_h1_prob_k);
      view_h1_prob_pi.ReadBulk(i * blksize, blksize, cache_h1_prob_pi);
      view_h1_charge.ReadBulk(i * blksize, blksize, cache_h1_charge);
      view_h2_px.ReadBulk(i * blksize, blksize, cache_h2_px);
      view_h2_py.ReadBulk(i * blksize, blksize, cache_h2_py);
      view_h2_pz.ReadBulk(i * blksize, blksize, cache_h2_pz);
      view_h2_prob_k.ReadBulk(i * blksize, blksize, cache_h2_prob_k);
      view_h2_prob_pi.ReadBulk(i * blksize, blksize, cache_h2_prob_pi);
      view_h2_charge.ReadBulk(i * blksize, blksize, cache_h2_charge);
      view_h3_px.ReadBulk(i * blksize, blksize, cache_h3_px);
      view_h3_py.ReadBulk(i * blksize, blksize, cache_h3_py);
      view_h3_pz.ReadBulk(i * blksize, blksize, cache_h3_pz);
      view_h3_prob_k.ReadBulk(i * blksize, blksize, cache_h3_prob_k);
      view_h3_prob_pi.ReadBulk(i * blksize, blksize, cache_h3_prob_pi);
      view_h3_charge.ReadBulk(i * blksize, blksize, cache_h3_charge);

      for (unsigned j = 0; j < blksize; ++j) {
         //std::cout << "HE IS MUON: " << cache_h1_is_muon[j] << "  " << cache_h2_is_muon[j] << std::endl;

         if (cache_h1_is_muon[j] || cache_h2_is_muon[j] || cache_h3_is_muon[j]) {
            result.skipped++;
            continue;
         }

         result.sum +=
            cache_h1_px[j] +
            cache_h1_py[j] +
            cache_h1_pz[j] +
            cache_h1_prob_k[j] +
            cache_h1_prob_pi[j] +
            double(cache_h1_charge[j]) +
            cache_h2_px[j] +
            cache_h2_py[j] +
            cache_h2_pz[j] +
            cache_h2_prob_k[j] +
            cache_h2_prob_pi[j] +
            double(cache_h2_charge[j]) +
            cache_h3_px[j] +
            cache_h3_py[j] +
            cache_h3_pz[j] +
            cache_h3_prob_k[j] +
            cache_h3_prob_pi[j] +
            double(cache_h3_charge[j]);
      }
   }

   return result;
}


int main(int argc, char **argv) {
   using RColumnSource = ROOT::Experimental::RColumnSource;
   using RTreeModel = ROOT::Experimental::RTreeModel;

   std::chrono::high_resolution_clock stopwatch;
   auto start_time = stopwatch.now();
   auto event_model = std::make_shared<RTreeModel>();
   RTree tree(event_model, RColumnSource::MakeSourceRaw(argv[1]));

   Result result;
   result = ReadVectorized(&tree);

   auto end_time = stopwatch.now();
   auto diff = end_time - start_time;
   auto milliseconds =
    std::chrono::duration_cast<std::chrono::milliseconds>(diff);
   std::cout << "reading took " << milliseconds.count()
            << " milliseconds,    sum " << result.sum
            << "   skipped: " << result.skipped
            << std::endl;

   return 0;
}
