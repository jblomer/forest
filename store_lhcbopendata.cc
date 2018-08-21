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


using RColumnRawSettings = ROOT::Experimental::RColumnRawSettings;

void Write(RColumnRawSettings settings) {
   using RColumnSink = ROOT::Experimental::RColumnSink;
   using RTree = ROOT::Experimental::RTree;
   using RTreeModel = ROOT::Experimental::RTreeModel;

   TChain* chain = new TChain("DecayTree");
   chain->Add("data/B2HHH.root");

   auto event_model = std::make_shared<RTreeModel>();
   auto cargo_b_flight_distance = event_model->Branch<double>("B_FlightDistance");
   auto cargo_b_vertex_chi2 = event_model->Branch<double>("B_VertexChi2");
   auto cargo_h1_px = event_model->Branch<double>("H1_PX");
   auto cargo_h1_py = event_model->Branch<double>("H1_PY");
   auto cargo_h1_pz = event_model->Branch<double>("H1_PZ");
   auto cargo_h1_prob_k = event_model->Branch<double>("H1_ProbK");
   auto cargo_h1_prob_pi = event_model->Branch<double>("H1_ProbPi");
   auto cargo_h1_charge = event_model->Branch<int>("H1_Charge");
   auto cargo_h1_is_muon = event_model->Branch<int>("H1_isMuon");
   auto cargo_h1_ip_chi2 = event_model->Branch<double>("H1_IpChi2");
   auto cargo_h2_px = event_model->Branch<double>("H2_PX");
   auto cargo_h2_py = event_model->Branch<double>("H2_PY");
   auto cargo_h2_pz = event_model->Branch<double>("H2_PZ");
   auto cargo_h2_prob_k = event_model->Branch<double>("H2_ProbK");
   auto cargo_h2_prob_pi = event_model->Branch<double>("H2_ProbPi");
   auto cargo_h2_charge = event_model->Branch<int>("H2_Charge");
   auto cargo_h2_is_muon = event_model->Branch<int>("H2_isMuon");
   auto cargo_h2_ip_chi2 = event_model->Branch<double>("H2_IpChi2");
   auto cargo_h3_px = event_model->Branch<double>("H3_PX");
   auto cargo_h3_py = event_model->Branch<double>("H3_PY");
   auto cargo_h3_pz = event_model->Branch<double>("H3_PZ");
   auto cargo_h3_prob_k = event_model->Branch<double>("H3_ProbK");
   auto cargo_h3_prob_pi = event_model->Branch<double>("H3_ProbPi");
   auto cargo_h3_charge = event_model->Branch<int>("H3_Charge");
   auto cargo_h3_is_muon = event_model->Branch<int>("H3_isMuon");
   auto cargo_h3_ip_chi2 = event_model->Branch<double>("H3_IpChi2");

   RTree forest(event_model, RColumnSink::MakeSinkRaw(settings));

   double b_flight_distance;
   double b_vertex_chi2;
   double h1_px;
   double h1_py;
   double h1_pz;
   double h1_prob_k;
   double h1_prob_pi;
   int h1_charge;
   int h1_is_muon;
   double h1_ip_chi2;
   double h2_px;
   double h2_py;
   double h2_pz;
   double h2_prob_k;
   double h2_prob_pi;
   int h2_charge;
   int h2_is_muon;
   double h2_ip_chi2;
   double h3_px;
   double h3_py;
   double h3_pz;
   double h3_prob_k;
   double h3_prob_pi;
   int h3_charge;
   int h3_is_muon;
   double h3_ip_chi2;

   TBranch* br_b_flight_distance;
   TBranch* br_b_vertex_chi2;
   TBranch* br_h1_px;
   TBranch* br_h1_py;
   TBranch* br_h1_pz;
   TBranch* br_h1_prob_k;
   TBranch* br_h1_prob_pi;
   TBranch* br_h1_charge;
   TBranch* br_h1_is_muon;
   TBranch* br_h1_ip_chi2;
   TBranch* br_h2_px;
   TBranch* br_h2_py;
   TBranch* br_h2_pz;
   TBranch* br_h2_prob_k;
   TBranch* br_h2_prob_pi;
   TBranch* br_h2_charge;
   TBranch* br_h2_is_muon;
   TBranch* br_h2_ip_chi2;
   TBranch* br_h3_px;
   TBranch* br_h3_py;
   TBranch* br_h3_pz;
   TBranch* br_h3_prob_k;
   TBranch* br_h3_prob_pi;
   TBranch* br_h3_charge;
   TBranch* br_h3_is_muon;
   TBranch* br_h3_ip_chi2;

   chain->SetBranchAddress("B_FlightDistance", &b_flight_distance, &br_b_flight_distance);
   chain->SetBranchAddress("B_VertexChi2", &b_vertex_chi2, &br_b_vertex_chi2);
   chain->SetBranchAddress("H1_PX",     &h1_px,      &br_h1_px);
   chain->SetBranchAddress("H1_PY",     &h1_py,      &br_h1_py);
   chain->SetBranchAddress("H1_PZ",     &h1_pz,      &br_h1_pz);
   chain->SetBranchAddress("H1_ProbK",  &h1_prob_k,  &br_h1_prob_k);
   chain->SetBranchAddress("H1_ProbPi", &h1_prob_pi, &br_h1_prob_pi);
   chain->SetBranchAddress("H1_Charge", &h1_charge,  &br_h1_charge);
   chain->SetBranchAddress("H1_isMuon", &h1_is_muon, &br_h1_is_muon);
   chain->SetBranchAddress("H1_IpChi2", &h1_ip_chi2, &br_h1_ip_chi2);
   chain->SetBranchAddress("H2_PX",     &h2_px,      &br_h2_px);
   chain->SetBranchAddress("H2_PY",     &h2_py,      &br_h2_py);
   chain->SetBranchAddress("H2_PZ",     &h2_pz,      &br_h2_pz);
   chain->SetBranchAddress("H2_ProbK",  &h2_prob_k,  &br_h2_prob_k);
   chain->SetBranchAddress("H2_ProbPi", &h2_prob_pi, &br_h2_prob_pi);
   chain->SetBranchAddress("H2_Charge", &h2_charge,  &br_h2_charge);
   chain->SetBranchAddress("H2_isMuon", &h2_is_muon, &br_h2_is_muon);
   chain->SetBranchAddress("H2_IpChi2", &h2_ip_chi2, &br_h2_ip_chi2);
   chain->SetBranchAddress("H3_PX",     &h3_px,      &br_h3_px);
   chain->SetBranchAddress("H3_PY",     &h3_py,      &br_h3_py);
   chain->SetBranchAddress("H3_PZ",     &h3_pz,      &br_h3_pz);
   chain->SetBranchAddress("H3_ProbK",  &h3_prob_k,  &br_h3_prob_k);
   chain->SetBranchAddress("H3_ProbPi", &h3_prob_pi, &br_h3_prob_pi);
   chain->SetBranchAddress("H3_Charge", &h3_charge,  &br_h3_charge);
   chain->SetBranchAddress("H3_isMuon", &h3_is_muon, &br_h3_is_muon);
   chain->SetBranchAddress("H3_IpChi2", &h3_ip_chi2, &br_h3_ip_chi2);

   double sum = 0.0;
   unsigned skipped = 0;
   auto num_events = chain->GetEntries();
   std::cout << num_events << std::endl;
   for (int i = 0; i < num_events; ++i) {
      br_b_flight_distance->GetEntry(i);
      br_b_vertex_chi2->GetEntry(i);
      br_h1_px->GetEntry(i);
      br_h1_py->GetEntry(i);
      br_h1_pz->GetEntry(i);
      br_h1_prob_k->GetEntry(i);
      br_h1_prob_pi->GetEntry(i);
      br_h1_charge->GetEntry(i);
      br_h1_is_muon->GetEntry(i);
      br_h1_ip_chi2->GetEntry(i);
      br_h2_px->GetEntry(i);
      br_h2_py->GetEntry(i);
      br_h2_pz->GetEntry(i);
      br_h2_prob_k->GetEntry(i);
      br_h2_prob_pi->GetEntry(i);
      br_h2_charge->GetEntry(i);
      br_h2_is_muon->GetEntry(i);
      br_h2_ip_chi2->GetEntry(i);
      br_h3_px->GetEntry(i);
      br_h3_py->GetEntry(i);
      br_h3_pz->GetEntry(i);
      br_h3_prob_k->GetEntry(i);
      br_h3_prob_pi->GetEntry(i);
      br_h3_charge->GetEntry(i);
      br_h3_is_muon->GetEntry(i);
      br_h3_ip_chi2->GetEntry(i);

      *cargo_b_flight_distance = b_flight_distance;
      *cargo_b_vertex_chi2 = b_vertex_chi2;
      *cargo_h1_px       = h1_px;
      *cargo_h1_py       = h1_py;
      *cargo_h1_pz       = h1_pz;
      *cargo_h1_prob_k   = h1_prob_k;
      *cargo_h1_prob_pi  = h1_prob_pi;
      *cargo_h1_charge   = h1_charge;
      *cargo_h1_is_muon  = h1_is_muon;
      *cargo_h1_ip_chi2  = h1_ip_chi2;
      *cargo_h2_px       = h2_px;
      *cargo_h2_py       = h2_py;
      *cargo_h2_pz       = h2_pz;
      *cargo_h2_prob_k   = h2_prob_k;
      *cargo_h2_prob_pi  = h2_prob_pi;
      *cargo_h2_charge   = h2_charge;
      *cargo_h2_is_muon  = h2_is_muon;
      *cargo_h2_ip_chi2  = h2_ip_chi2;
      *cargo_h3_px       = h3_px;
      *cargo_h3_py       = h3_py;
      *cargo_h3_pz       = h3_pz;
      *cargo_h3_prob_k   = h3_prob_k;
      *cargo_h3_prob_pi  = h3_prob_pi;
      *cargo_h3_charge   = h3_charge;
      *cargo_h3_is_muon  = h3_is_muon;
      *cargo_h3_ip_chi2  = h3_ip_chi2;

      forest.Fill();

      if (!h1_is_muon && !h2_is_muon && !h3_is_muon) {
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
      } else {
         skipped++;
      }
   }

   std::cout << "Checksums: sum " << sum << "   skipped " << skipped << std::endl;
}

int main() {
   RColumnRawSettings settings("data/B2HHH.forest-mem~flat");
   //settings.fCompressionSettings = 104;  // ZLIB, level 4
   //settings.fEpochSize = 64 * 1024 * 1024;
   Write(settings);

   settings.fPath = "data/B2HHH.forest-zlib~flat";
   settings.fCompressionSettings = 104;
   Write(settings);

   return 0;
}
