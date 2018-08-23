#include "ROOT/RDataFrame.hxx"
#include "ROOT/RForestDS.hxx"
#include "ROOT/RRootDS.hxx"
#include "ROOT/RTree.hxx"
#include "ROOT/RTreeModel.hxx"
#include "ROOT/RVec.hxx"

#include "TClassTable.h"
#include "TSystem.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <utility>

#include <unistd.h>

struct Result {
   Result() : sum(0.0), skipped(0) { }
   double sum;
   unsigned skipped;
};


Result ReadFlat(ROOT::RDataFrame *rdf) {
   Result result;

   // TODO: make thread safe
   auto lCatMuon = [&result](int h1_isMuon, int h2_isMuon, int h3_isMuon)
      { //std::cout << "GOT: h1_isMuon " << h1_isMuon << " H2 " << h2_isMuon << " H3 " << h3_isMuon << std::endl;
        bool isBad = h1_isMuon || h2_isMuon || h3_isMuon;
        if (isBad) result.skipped++;
        return !isBad;
      };

   auto lSum = [](
      double h1_px,
      double h1_py,
      double h1_pz,
      double h1_prob_k,
      double h1_prob_pi,
      int h1_charge,
      double h2_px,
      double h2_py,
      double h2_pz,
      double h2_prob_k,
      double h2_prob_pi,
      int h2_charge,
      double h3_px,
      double h3_py,
      double h3_pz,
      double h3_prob_k,
      double h3_prob_pi,
      int h3_charge) {
         return
            h1_px +
            h1_py +
            h1_pz +
            h1_prob_k +
            h1_prob_pi +
            double(h1_charge) +
            h2_px +
            h2_py +
            h2_pz +
            h2_prob_k +
            h2_prob_pi +
            double(h2_charge) +
            h3_px +
            h3_py +
            h3_pz +
            h3_prob_k +
            h3_prob_pi +
            double(h3_charge);
      };

   result.sum = rdf->Filter(lCatMuon, {"H1_isMuon", "H2_isMuon", "H3_isMuon"}).Define("H_SUM", lSum,
      {
         "H1_PX",
         "H1_PY",
         "H1_PZ",
         "H1_ProbK",
         "H1_ProbPi",
         "H1_Charge",
         "H2_PX",
         "H2_PY",
         "H2_PZ",
         "H2_ProbK",
         "H2_ProbPi",
         "H2_Charge",
         "H3_PX",
         "H3_PY",
         "H3_PZ",
         "H3_ProbK",
         "H3_ProbPi",
         "H3_Charge",
      }).Sum("H_SUM").GetValue();

   return result;
}


Result ReadDeep(ROOT::RDataFrame *rdf) {
   Result result;
   auto fnCatMuon = [](ROOT::VecOps::RVec<int> h_isMuon)
      {
         bool isBad = h_isMuon[0] || h_isMuon[1] || h_isMuon[2];
         //if (isBad) result.skipped++;
         return !isBad;
      };

   auto fnSum = [](
      ROOT::VecOps::RVec<double> h_px,
      ROOT::VecOps::RVec<double> h_py,
      ROOT::VecOps::RVec<double> h_pz,
      ROOT::VecOps::RVec<double> h_prob_k,
      ROOT::VecOps::RVec<double> h_prob_pi,
      ROOT::VecOps::RVec<int> h_charge)
      {
         auto vec_sum =
            h_px +
            h_py +
            h_pz +
            h_prob_k +
            h_prob_pi;
         return vec_sum[0] + vec_sum[1] + vec_sum[2] +
           double(h_charge[0]) + double(h_charge[1]) + double(h_charge[2]);
      };

   result.sum =
      rdf->Filter(fnCatMuon, {"kaon_candidates.h_is_muon"}).Define("H_SUM", fnSum,
      {
         "kaon_candidates.h_px",
         "kaon_candidates.h_py",
         "kaon_candidates.h_pz",
         "kaon_candidates.h_prob_k",
         "kaon_candidates.h_prob_pi",
         "kaon_candidates.h_charge"
      }).Sum("H_SUM").GetValue();
   return result;
}


Result ReadCsplit(ROOT::RDataFrame *rdf) {
   Result result;
   auto lCatMuon = [&result](ROOT::VecOps::RVec<int> h_isMuon)
      { bool isBad = h_isMuon[0] || h_isMuon[1] || h_isMuon[2];
        if (isBad) result.skipped++;
        return !isBad;
      };

   auto lSum = [](
      ROOT::VecOps::RVec<double> h_px,
      ROOT::VecOps::RVec<double> h_py,
      ROOT::VecOps::RVec<double> h_pz,
      ROOT::VecOps::RVec<double> h_prob_k,
      ROOT::VecOps::RVec<double> h_prob_pi,
      ROOT::VecOps::RVec<int> h_charge) {
         auto vec_sum =
            h_px +
            h_py +
            h_pz +
            h_prob_k +
            h_prob_pi;
         return vec_sum[0] + vec_sum[1] + vec_sum[2] +
           double(h_charge[0]) + double(h_charge[1]) + double(h_charge[2]);
      };

   result.sum =
      rdf->Filter(lCatMuon, {"h_is_muon"}).Define("H_SUM", lSum,
      {
         "h_px",
         "h_py",
         "h_pz",
         "h_prob_k",
         "h_prob_pi",
         "h_charge"
      }).Sum("H_SUM").GetValue();

   return result;
}


int main(int argc, char **argv) {
   gSystem->Load("./libEvent.so");

   using RColumnSource = ROOT::Experimental::RColumnSource;
   using RDataFrame = ROOT::RDataFrame;

   std::string inputFile;
   bool useRRootDS = false;
   bool isDeep = false;
   bool isCsplit = false;

   int c;
   while ((c = getopt(argc, argv, "pri:")) != -1) {
      switch (c) {
      case 'p':
         ROOT::EnableImplicitMT();
         break;
      case 'r':
         useRRootDS = true;
         break;
      case 'i':
         inputFile = optarg;
         break;
      default:
         fprintf(stderr, "Unknown option: -%c\n", c);
         return 1;
      }
   }


   std::chrono::high_resolution_clock stopwatch;
   auto start_time_init = stopwatch.now();

   std::unique_ptr<RDataFrame> rdf;
   std::unique_ptr<RColumnSource> source;

   if (inputFile.find("forest") != std::string::npos) {
      source = RColumnSource::MakeSourceRaw(inputFile);
      rdf = std::make_unique<ROOT::RDataFrame>(std::make_unique<ROOT::RDF::RForestDS>(source.get()));
   } else {
      if (useRRootDS)
         rdf = std::make_unique<ROOT::RDataFrame>(ROOT::RDF::MakeRootDataFrame("DecayTree", inputFile));
      else
         rdf = std::make_unique<ROOT::RDataFrame>("DecayTree", inputFile);
   }

   if (inputFile.find("deep") != std::string::npos) {
      isDeep = true;
   }
   if (inputFile.find("csplit") != std::string::npos) {
      isCsplit = true;
   }

   auto start_time_proc = stopwatch.now();

   Result result;
   if (isDeep) {
      result = ReadDeep(rdf.get());
   } else if (isCsplit) {
      result = ReadCsplit(rdf.get());
   } else {
      result = ReadFlat(rdf.get());
   }

   auto end_time = stopwatch.now();
   auto diff_init = start_time_proc - start_time_init;
   auto diff_proc = end_time - start_time_proc;
   auto ms_init =
      std::chrono::duration_cast<std::chrono::milliseconds>(diff_init);
   auto ms_proc =
      std::chrono::duration_cast<std::chrono::milliseconds>(diff_proc);
   std::cout << "init took " << ms_init.count()
             << "ms,    reading took " << ms_proc.count()
             << "ms,    sum " << result.sum
             << "    skipped " << result.skipped
             << std::endl;

   return 0;
}
