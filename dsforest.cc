/**
 * Playground for possible TTree/TBranch interfaces.
 */

#include "ROOT/RColumnStorageFile.hxx"
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RForestDS.hxx"
#include "ROOT/RTree.hxx"
#include "ROOT/RTreeModel.hxx"

#include "TFile.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <utility>

void Write() {
   using RColumnRawSettings = ROOT::Experimental::RColumnRawSettings;
   using RColumnFileSettings = ROOT::Experimental::RColumnFileSettings;
   using RColumnSink = ROOT::Experimental::RColumnSink;
   using RTreeModel = ROOT::Experimental::RTreeModel;
   using RTree = ROOT::Experimental::RTree;

   auto event_model = std::make_shared<RTreeModel>();
   auto h1_px = event_model->Branch<float>("h1_px");

   //RColumnRawSettings settings("toy.forest");
   //settings.fCompressionSettings = 104;
   //RTree tree(event_model, RColumnSink::MakeSinkRaw(settings));
   TFile* file = new TFile("toy.tfile.forest", "RECREATE");
   RColumnFileSettings settings(file, "Forest");
   RTree tree(event_model, RColumnSink::MakeSinkFile(settings));

   *h1_px = 1.0;
   tree.Fill();
   *h1_px = 2.0;
   tree.Fill();
   *h1_px = 3.0;
   tree.Fill();
}

int main() {
   using RColumnSource = ROOT::Experimental::RColumnSource;

   Write();

   ROOT::EnableImplicitMT();

   std::chrono::high_resolution_clock stopwatch;
   auto start_time = stopwatch.now();


  float sum = 0.0;
  //float sum_e = 0.0;
  float sum_x = 0.0;
  //unsigned n_energy_sum_op = 0;
  start_time = stopwatch.now();

  auto source = RColumnSource::MakeSourceRaw("toy.forest");
  std::cout << "OK" << std::endl;
  auto rdf = ROOT::RDataFrame(std::make_unique<ROOT::RDF::RForestDS>(source.get()));
  std::cout << "RDF Ready" << std::endl;

  std::cout << *rdf.Max("h1_px") << std::endl;

  auto end_time = stopwatch.now();
  auto diff = end_time - start_time;
  auto milliseconds =
    std::chrono::duration_cast<std::chrono::milliseconds>(diff);
  std::cout << "reading took " << milliseconds.count()
            << " milliseconds,    sum " << sum
            << ",    sum_x " << sum_x
            //<< "   [n_energy_sum_op " << n_energy_sum_op << "]"
            << std::endl;

   return 0;
}
