/**
 * Playground for possible TTree/TBranch interfaces.
 */

#include "ROOT/RColumnStorageFile.hxx"
#include "ROOT/RTree.hxx"
#include "ROOT/RTreeModel.hxx"

#include "TBranch.h"
#include "TChain.h"
#include "TError.h"
#include "TFile.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <utility>

#include <unistd.h>


using RColumnFileSettings = ROOT::Experimental::RColumnFileSettings;
using RColumnRawSettings = ROOT::Experimental::RColumnRawSettings;
using RColumnSink = ROOT::Experimental::RColumnSink;
using RColumnSinkRaw = ROOT::Experimental::RColumnSinkRaw;
using RColumnSinkFile = ROOT::Experimental::RColumnSinkFile;
using RColumnSource = ROOT::Experimental::RColumnSource;
using RColumnSourceFile = ROOT::Experimental::RColumnSourceFile;
using RTree = ROOT::Experimental::RTree;
using RTreeModel = ROOT::Experimental::RTreeModel;


int main(int argc, char** argv) {
   bool isFile = false;
   std::string inputFile;

   int c;
   while ((c = getopt(argc, argv, "fi:")) != -1) {
      switch (c) {
      case 'f':
         isFile = true;
         break;
      case 'i':
         inputFile = optarg;
         break;
      default:
         fprintf(stderr, "Unknown option: -%c\n", c);
         return 1;
      }
   }

   RTree *tree = nullptr;
   auto event_model = std::make_shared<RTreeModel>();
   if (isFile) {
      TFile* file = new TFile(inputFile.c_str(), "READ");
      RColumnSourceFile *source = new RColumnSourceFile(file);
      tree = new RTree(event_model, std::unique_ptr<RColumnSourceFile>(source));
   } else {
      tree = new RTree(event_model, RColumnSource::MakeSourceRaw(inputFile));
   }



   return 0;
}
