#include "ROOT/RDataFrame.hxx"
#include "ROOT/RForestDS.hxx"
#include "ROOT/RTree.hxx"
#include "ROOT/RTreeModel.hxx"

#include <chrono>
#include <iostream>
#include <memory>
#include <utility>

#include <unistd.h>

int main(int argc, char **argv) {
   using RColumnSource = ROOT::Experimental::RColumnSource;
   using RDataFrame = ROOT::RDataFrame;

   std::string inputFile;

   int c;
   while ((c = getopt(argc, argv, "pi:")) != -1) {
      switch (c) {
      case 'p':
         ROOT::EnableImplicitMT();
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

   double sum = 0.0;
   unsigned skipped = 0;

   std::unique_ptr<RDataFrame> rdf;
   std::unique_ptr<RColumnSource> source;

   if (inputFile.find("forest") != std::string::npos) {
      source = RColumnSource::MakeSourceRaw(inputFile);
      rdf = std::make_unique<ROOT::RDataFrame>(std::make_unique<ROOT::RDF::RForestDS>(source.get()));
   } else {
      rdf = std::make_unique<ROOT::RDataFrame>("DecayTree", inputFile);
   }

   auto start_time_proc = stopwatch.now();

   auto lCatMuon = [&skipped](int h1_isMuon, int h2_isMuon, int h3_isMuon)
      { //std::cout << "GOT: h1_isMuon " << h1_isMuon << " H2 " << h2_isMuon << " H3 " << h3_isMuon << std::endl;
        bool isBad = h1_isMuon || h2_isMuon || h3_isMuon;
        if (isBad) skipped++;
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

   sum = rdf->Filter(lCatMuon, {"H1_isMuon", "H2_isMuon", "H3_isMuon"}).Define("H_SUM", lSum,
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

   auto end_time = stopwatch.now();
   auto diff_init = start_time_proc - start_time_init;
   auto diff_proc = end_time - start_time_proc;
   auto ms_init =
      std::chrono::duration_cast<std::chrono::milliseconds>(diff_init);
   auto ms_proc =
      std::chrono::duration_cast<std::chrono::milliseconds>(diff_proc);
   std::cout << "init took " << ms_init.count()
             << "ms,    reading took " << ms_proc.count()
             << "ms,    sum " << sum
             << "    skipped " << skipped
             << std::endl;

   return 0;
}
