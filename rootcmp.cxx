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

  if (!TClassTable::GetDict("Event")) {
    gSystem->Load("./libEvent.so");
  }

  TTree *tree = new TTree("DecayTree", "");

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

  tree->Branch("events", "Event", &event);

  for (unsigned i = 0; i < 8000000; ++i) {
    tree->Fill();
  }

  file->Write();
  file->Close();

  return 0;
}
