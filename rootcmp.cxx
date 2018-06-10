#include <TClass.h>
#include <TClassTable.h>
#include <TFile.h>
#include <TSystem.h>
#include <TTree.h>

#include <chrono>
#include <iostream>
#include <vector>

#include "event.h"

int main() {
  std::chrono::high_resolution_clock stopwatch;
  auto start_time = stopwatch.now();
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
  /*float myfloat = 1.0;
  tree->Branch("h1_py", &myfloat, "h1_py/F");*/

  for (unsigned i = 0; i < 8000000; ++i) {
    tree->Fill();
  }

  file->Write();
  file->Close();

  auto end_time = stopwatch.now();
  auto diff = end_time - start_time;
  auto milliseconds =
    std::chrono::duration_cast<std::chrono::milliseconds>(diff);
  std::cout << "finished writing, now reading, took "
            << milliseconds.count() << " milliseconds" << std::endl;

  float sum = 0.0;

  start_time = stopwatch.now();
  TFile roFile("/dev/shm/test.root");
  TTree* roTree = nullptr;
  roFile.GetObject("DecayTree", roTree);
  Event *roEvent = new Event(); //object must be created before
                               //setting the branch address
  /*float h1_py;
  auto branch = roTree->GetBranch("h1_py");
  roTree->SetBranchAddress("h1_py", &h1_py, &branch);
  auto nevent = roTree->GetEntries();
  std::cout << "found " << nevent << " events" << std::endl;
  for (Int_t i=0;i<nevent;i++) {
    branch->GetEntry(i);
    if (i % 1000000 == 0)
      std::cout << "event " << i << " value " << h1_py << std::endl;
    sum += h1_py;
  }
  end_time = stopwatch.now();
  diff = end_time - start_time;
  milliseconds =
    std::chrono::duration_cast<std::chrono::milliseconds>(diff);
  std::cout << "reading took " << milliseconds.count()
            << " milliseconds (sum " << sum << ")" << std::endl;*/

  return 0;
}
