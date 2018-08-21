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

   std::cout << *(rdf->Max("h1_px")) << std::endl;

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
