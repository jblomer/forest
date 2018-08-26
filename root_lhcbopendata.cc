/**
 * Playground for possible TTree/TBranch interfaces.
 */


#include "TBranch.h"
#include "TChain.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <utility>

#include <unistd.h>

int main(int argc, char** argv) {
   std::string inputFile;

   int c;
   while ((c = getopt(argc, argv, "i:")) != -1) {
      switch (c) {
      case 'i':
         inputFile = optarg;
         break;
      default:
         fprintf(stderr, "Unknown option: -%c\n", c);
         return 1;
      }
   }

   std::chrono::high_resolution_clock stopwatch;
   auto start_time = stopwatch.now();

   TChain* chain = new TChain("DecayTree");
   chain->Add(inputFile.c_str());

   double h1_px;
   double h1_py;
   double h1_pz;
   double h1_prob_k;
   double h1_prob_pi;
   int h1_charge;
   int h1_is_muon;
   double h2_px;
   double h2_py;
   double h2_pz;
   double h2_prob_k;
   double h2_prob_pi;
   int h2_charge;
   int h2_is_muon;
   double h3_px;
   double h3_py;
   double h3_pz;
   double h3_prob_k;
   double h3_prob_pi;
   int h3_charge;
   int h3_is_muon;

   TBranch* br_h1_px;
   TBranch* br_h1_py;
   TBranch* br_h1_pz;
   TBranch* br_h1_prob_k;
   TBranch* br_h1_prob_pi;
   TBranch* br_h1_charge;
   TBranch* br_h1_is_muon;
   TBranch* br_h2_px;
   TBranch* br_h2_py;
   TBranch* br_h2_pz;
   TBranch* br_h2_prob_k;
   TBranch* br_h2_prob_pi;
   TBranch* br_h2_charge;
   TBranch* br_h2_is_muon;
   TBranch* br_h3_px;
   TBranch* br_h3_py;
   TBranch* br_h3_pz;
   TBranch* br_h3_prob_k;
   TBranch* br_h3_prob_pi;
   TBranch* br_h3_charge;
   TBranch* br_h3_is_muon;

   chain->SetBranchAddress("H1_PX",     &h1_px,      &br_h1_px);
   chain->SetBranchAddress("H1_PY",     &h1_py,      &br_h1_py);
   chain->SetBranchAddress("H1_PZ",     &h1_pz,      &br_h1_pz);
   chain->SetBranchAddress("H1_ProbK",  &h1_prob_k,  &br_h1_prob_k);
   chain->SetBranchAddress("H1_ProbPi", &h1_prob_pi, &br_h1_prob_pi);
   chain->SetBranchAddress("H1_Charge", &h1_charge,  &br_h1_charge);
   chain->SetBranchAddress("H1_isMuon", &h1_is_muon, &br_h1_is_muon);
   chain->SetBranchAddress("H2_PX",     &h2_px,      &br_h2_px);
   chain->SetBranchAddress("H2_PY",     &h2_py,      &br_h2_py);
   chain->SetBranchAddress("H2_PZ",     &h2_pz,      &br_h2_pz);
   chain->SetBranchAddress("H2_ProbK",  &h2_prob_k,  &br_h2_prob_k);
   chain->SetBranchAddress("H2_ProbPi", &h2_prob_pi, &br_h2_prob_pi);
   chain->SetBranchAddress("H2_Charge", &h2_charge,  &br_h2_charge);
   chain->SetBranchAddress("H2_isMuon", &h2_is_muon, &br_h2_is_muon);
   chain->SetBranchAddress("H3_PX",     &h3_px,      &br_h3_px);
   chain->SetBranchAddress("H3_PY",     &h3_py,      &br_h3_py);
   chain->SetBranchAddress("H3_PZ",     &h3_pz,      &br_h3_pz);
   chain->SetBranchAddress("H3_ProbK",  &h3_prob_k,  &br_h3_prob_k);
   chain->SetBranchAddress("H3_ProbPi", &h3_prob_pi, &br_h3_prob_pi);
   chain->SetBranchAddress("H3_Charge", &h3_charge,  &br_h3_charge);
   chain->SetBranchAddress("H3_isMuon", &h3_is_muon, &br_h3_is_muon);

   double sum = 0.0;
   unsigned skipped = 0;
   auto num_events = chain->GetEntries();
   std::cout << num_events << std::endl;
   for (int i = 0; i < num_events; ++i) {
      br_h1_is_muon->GetEntry(i);
      if (h1_is_muon) {
         skipped++;
         continue;
      }
      br_h2_is_muon->GetEntry(i);
      if (h2_is_muon) {
         skipped++;
         continue;
      }
      br_h3_is_muon->GetEntry(i);
      if (h3_is_muon) {
         skipped++;
         continue;
      }

      br_h1_px->GetEntry(i);
      br_h1_py->GetEntry(i);
      br_h1_pz->GetEntry(i);
      br_h1_prob_k->GetEntry(i);
      br_h1_prob_pi->GetEntry(i);
      br_h1_charge->GetEntry(i);
      br_h2_px->GetEntry(i);
      br_h2_py->GetEntry(i);
      br_h2_pz->GetEntry(i);
      br_h2_prob_k->GetEntry(i);
      br_h2_prob_pi->GetEntry(i);
      br_h2_charge->GetEntry(i);
      br_h3_px->GetEntry(i);
      br_h3_py->GetEntry(i);
      br_h3_pz->GetEntry(i);
      br_h3_prob_k->GetEntry(i);
      br_h3_prob_pi->GetEntry(i);
      br_h3_charge->GetEntry(i);

      sum +=
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
   }

   auto end_time = stopwatch.now();
   auto diff = end_time - start_time;
   auto milliseconds =
    std::chrono::duration_cast<std::chrono::milliseconds>(diff);

   std::cout << "sum " << sum << "   skipped " << skipped << std::endl;
   std::cout << "Took " << milliseconds.count() << " ms" << std::endl;

   return 0;
}
