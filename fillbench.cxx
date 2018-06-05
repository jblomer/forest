#include <TFile.h>
#include <TTree.h>

int main() {
  TFile *file = new TFile("/dev/shm/fillbench.root", "RECREATE");
  file->SetCompressionSettings(0);
  TTree *tree = new TTree("DecayTree", "");

  float h1_px = 0.0;
  float h1_py = 1.0;
  float h1_pz = 2.0;
  float h2_px = 3.0;
  float h2_py = 4.0;
  float h2_pz = 5.0;
  float h3_px = 6.0;
  float h3_py = 7.0;
  float h3_pz = 8.0;
  float h4_px = 0.0;
  float h4_py = 1.0;
  float h4_pz = 2.0;
  float h5_px = 3.0;
  float h5_py = 4.0;
  float h5_pz = 5.0;
  float h6_px = 6.0;
  float h6_py = 7.0;
  float h6_pz = 8.0;
  tree->Branch("h1_px", &h1_px, "h1_px");
  tree->Branch("h1_py", &h1_py, "h1_py");
  tree->Branch("h1_pz", &h1_pz, "h1_pz");
  tree->Branch("h2_px", &h2_px, "h2_px");
  tree->Branch("h2_py", &h2_py, "h2_py");
  tree->Branch("h2_pz", &h2_pz, "h2_pz");
  tree->Branch("h3_px", &h3_px, "h3_px");
  tree->Branch("h3_py", &h3_py, "h3_py");
  tree->Branch("h3_pz", &h3_pz, "h3_pz");
  tree->Branch("h4_px", &h4_px, "h4_px");
  tree->Branch("h4_py", &h4_py, "h4_py");
  tree->Branch("h4_pz", &h4_pz, "h4_pz");
  tree->Branch("h5_px", &h5_px, "h5_px");
  tree->Branch("h5_py", &h5_py, "h5_py");
  tree->Branch("h5_pz", &h5_pz, "h5_pz");
  tree->Branch("h6_px", &h6_px, "h6_px");
  tree->Branch("h6_py", &h6_py, "h6_py");
  tree->Branch("h6_pz", &h6_pz, "h6_pz");

  for (unsigned i = 0; i < 10000000; ++i) {
    tree->Fill();
  }

  file->Write();
  file->Close();

  return 0;
}
