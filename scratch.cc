int main() {
  /*if (!TClassTable::GetDict("FlatEvent")) {
    gSystem->Load("./libEvent.so");
    std::cout << "Library libEvent.so loaded" << std::endl;
  }*/

  //ROOT::Experimental::TFilePtr file_ptr =
  //  ROOT::Experimental::TFile::Open("/dev/null");
  ROOT::Experimental::TFilePtr file_ptr =
    ROOT::Experimental::TFile::Recreate("testfile.root");

  //TFile file("testoldfile.root", "RECREATE");
  TH1F histogram("bla", "blubb", 100, 1, 1);
  //histogram.Write();
  //file.Close();

  file_ptr->Write("New Histogram", histogram);
  file_ptr->Close();

  return 0;
}
