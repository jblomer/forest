#include <TClass.h>
#include <TClassTable.h>
#include <TFile.h>
#include <TSystem.h>
#include <TTree.h>

#include <vector>

#include "event.h"

int main() {
  TFile *file = new TFile("/dev/shm/test.root", "RECREATE");
  file->SetCompressionSettings(0);
  TTree *tree = new TTree("DecayTree", "");

  /*if (!TClassTable::GetDict("Event")) {
    gSystem->Load("./libEvent.so");
  }

  Event event;
  event.h1_px = 0.0;
  event.h1_py = 1.0;
  event.h1_pz = 2.0;
  event.h2_px = 3.0;
  event.h2_py = 4.0;
  event.h2_pz = 5.0;
  event.h3_px = 6.0;
  event.h3_py = 7.0;
  event.h3_pz = 8.0;
  event.unsafe = 9.0;

  for (unsigned i = 0; i < 3; ++i) {
    Track track;
    track.energy = 10123421342121.0;
    for (unsigned j = 0; j < 3; ++j) {
      Hit hit;
      hit.x = 111234231234.0;
      hit.y = 1212342134213423.0;
      track.hits.push_back(hit);
    }
    event.tracks.push_back(track);
  }

  tree->Branch("events", &event);*/

  float h1_px = 0.0;
  float h1_py = 1.0;
  float h1_pz = 2.0;
  float h2_px = 3.0;
  float h2_py = 4.0;
  float h2_pz = 5.0;
  float h3_px = 6.0;
  float h3_py = 7.0;
  float h3_pz = 8.0;
  float unsafe = 9.0;
  tree->Branch("h1_px", &h1_px, "h1_px");
  tree->Branch("h1_py", &h1_py, "h1_py");
  tree->Branch("h1_pz", &h1_pz, "h1_pz");
  tree->Branch("h2_px", &h2_px, "h2_px");
  tree->Branch("h2_py", &h2_py, "h2_py");
  tree->Branch("h2_pz", &h2_pz, "h2_pz");
  tree->Branch("h3_px", &h3_px, "h3_px");
  tree->Branch("h3_py", &h3_py, "h3_py");
  tree->Branch("h3_pz", &h3_pz, "h3_pz");
  tree->Branch("unsafe", &unsafe, "unsafe");

  for (unsigned i = 0; i < 8000000; ++i) {
    tree->Fill();
  }

  file->Write();
  file->Close();

  return 0;
}
