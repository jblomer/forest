#include "Riostream.h"
#include "TROOT.h"
#include "TFile.h"
#include "TClass.h"

#include "event.h"

int main() {
  TFile *f = new TFile("streamer.root", "RECREATE");
  //Event *event = new Event();
  //event->SetName("name");
  //event->SetTitle("title");
  //event->Write();

  f->Close();

  return 0;
}
