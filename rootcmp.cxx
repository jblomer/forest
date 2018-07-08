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
    track.energy = 1.2;
    for (unsigned j = 0; j < 3; ++j) {
      Hit hit;
      hit.x = 4.2;
      hit.y = 8.4;
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
  float sum_x = 0.0;

  start_time = stopwatch.now();
  TFile file_read("/dev/shm/test.root");
  TTree* tree_read = nullptr;
  file_read.GetObject("DecayTree", tree_read);
  Event *event_read = new Event(); //object must be created before
                                   //setting the branch address

  tree_read->SetBranchAddress("events", &event_read);
  //tree_read->SetBranchStatus("*", 0);
  //tree_read->SetBranchStatus("h1_py", 1);

  TBranch* br_h1_py = tree_read->GetBranch("h1_py");
  //TBranch* br_energy = tree_read->GetBranch("tracks.energy");
  // Note: GetBranch("tracks.hits.x") doesn't work
  TBranch* br_hits = tree_read->GetBranch("tracks.hits");

  auto nevent = tree_read->GetEntries();
  std::cout << "found " << nevent << " events" << std::endl;
  for (Int_t i = 0; i < nevent; i++) {
    //tree_read->GetEntry(i);
    br_h1_py->GetEntry(i);
    br_hits->GetEntry(i);
    if (i % 1000000 == 0)
      std::cout << "event " << i << " value " << event_read->h1_py
                << " (" << event_read->tracks.size() << ")"
                << ",  (" << event_read->tracks[0].hits.size() << ")"
                << std::endl;
    sum += event_read->h1_py;
    for (auto t : event_read->tracks) {
      for (auto h : t.hits) {
        sum_x += h.x;
      }
    }
  }
  end_time = stopwatch.now();
  diff = end_time - start_time;
  milliseconds =
    std::chrono::duration_cast<std::chrono::milliseconds>(diff);
  std::cout << "reading took " << milliseconds.count()
            << " milliseconds (sum " << sum << ", sum_x " << sum_x << ")"
            << std::endl;

  return 0;
}
