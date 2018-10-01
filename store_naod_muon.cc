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
using RTree = ROOT::Experimental::RTree;
using RTreeModel = ROOT::Experimental::RTreeModel;

void Write(TTree *inputTree, RColumnSink *sink, int maxN) {
   constexpr int kMaxMuon = 32;

   auto muon_model = std::make_shared<RTreeModel>();
   auto cargo_pt = muon_model->Branch<float>("pt");
   auto cargo_eta = muon_model->Branch<float>("eta");
   auto cargo_phi = muon_model->Branch<float>("phi");
   auto cargo_mass = muon_model->Branch<float>("mass");
   auto cargo_charge = muon_model->Branch<int>("charge");

   auto event_model = std::make_shared<RTreeModel>();
   auto cargo_muon = event_model->BranchCollection("muons", muon_model);

   RTree *forest = new RTree(event_model, std::unique_ptr<RColumnSink>(sink));

   Int_t nMuon;
   auto pt = new std::vector<float>();
   auto eta = new std::vector<float>();
   auto phi = new std::vector<float>();
   auto mass = new std::vector<float>();
   auto charge = new std::vector<int>();
   pt->resize(kMaxMuon);
   eta->resize(kMaxMuon);
   phi->resize(kMaxMuon);
   mass->resize(kMaxMuon);
   charge->resize(kMaxMuon);

   TBranch* br_nMuon;
   TBranch* br_pt;
   TBranch* br_eta;
   TBranch* br_phi;
   TBranch* br_mass;
   TBranch* br_charge;

   inputTree->SetBranchAddress("nMuon", &nMuon, &br_nMuon);
   inputTree->SetBranchAddress("Muon_pt", &pt, &br_pt);
   inputTree->SetBranchAddress("Muon_eta", &eta, &br_eta);
   inputTree->SetBranchAddress("Muon_phi", &phi, &br_phi);
   inputTree->SetBranchAddress("Muon_mass", &mass, &br_mass);
   inputTree->SetBranchAddress("Muon_charge", &charge, &br_charge);

   auto N = inputTree->GetEntries();
   for (int i = 0; i < N; ++i) {
      br_nMuon->GetEntry(i);
      R__ASSERT(nMuon < kMaxMuon);

      br_pt->GetEntry(i);
      br_eta->GetEntry(i);
      br_phi->GetEntry(i);
      br_mass->GetEntry(i);
      br_charge->GetEntry(i);

      for (int imuon = 0; imuon < nMuon; ++imuon) {
         *cargo_pt = (*pt)[imuon];
         *cargo_eta = (*eta)[imuon];
         *cargo_phi = (*phi)[imuon];
         *cargo_mass = (*mass)[imuon];
         *cargo_charge = (*charge)[imuon];
         cargo_muon->Fill();
      }
      forest->Fill();

      if (i % 1000000 == 0) {
         std::cout << "  ... processed " << i << " events" << std::endl;
      }
      if ((maxN > 0) && (i >= maxN)) break;
   }
   delete forest;
   std::cout << "Converted " << N << " entries" << std::endl;
}

int main(int argc, char** argv) {
   bool writeMem = false;
   bool writeZlib = false;
   bool writeFile = false;
   std::string inputFile;
   std::string outputFile;
   int maxN = 0;

   int c;
   while ((c = getopt(argc, argv, "mzfi:o:n:")) != -1) {
      switch (c) {
      case 'm':
         writeMem = true;
         break;
      case 'z':
         writeZlib = true;
         break;
      case 'f':
         writeFile = true;
         break;
      case 'i':
         inputFile = optarg;
         break;
      case 'o':
         outputFile = optarg;
         break;
      case 'n':
         maxN = atoi(optarg);
         break;
      default:
         fprintf(stderr, "Unknown option: -%c\n", c);
         return 1;
      }
   }

   TChain* chain = new TChain("Events");
   chain->Add(inputFile.c_str());

   if (writeMem) {
     RColumnRawSettings settings_raw(outputFile);
     //settings_raw.fCompressionSettings = 404;
     //settings.fEpochSize = 64 * 1024 * 1024;
     RColumnSinkRaw *sink_raw = new RColumnSinkRaw(settings_raw);
     Write(chain, sink_raw, maxN);
   }

   if (writeZlib) {
      RColumnRawSettings settings_raw(outputFile);
      settings_raw.fCompressionSettings = 209;
      RColumnSinkRaw *sink_raw = new RColumnSinkRaw(settings_raw);
      Write(chain, sink_raw, maxN);
   }

   if (writeFile) {
      TFile* file = new TFile(outputFile.c_str(), "RECREATE");
      file->SetCompressionSettings(0);
      RColumnFileSettings settings_file(file, "Events");
      RColumnSinkFile *sink_file = new RColumnSinkFile(settings_file);
      Write(chain, sink_file, maxN);
   }

   return 0;
}
