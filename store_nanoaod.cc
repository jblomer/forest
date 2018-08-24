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

using RDataFrame = ROOT::RDataFrame;

int main(int argc, char **argv) {
   std::unique_ptr<RDataFrame> rdf;
   std::string inputFile;
   std::string outputFile;

   int c;
   while ((c = getopt(argc, argv, "i:o:")) != -1) {
      switch (c) {
      case 'i':
         inputFile = optarg;
         break;
      case 'o':
         outputFile = optarg;
         break;
      default:
         fprintf(stderr, "Unknown option: -%c\n", c);
         return 1;
      }
   }

   rdf = std::make_unique<ROOT::RDataFrame>("Events", inputFile);
   rdf->Snapshot("Events", outputFile, {"*Electron*"});

   return 0;
}