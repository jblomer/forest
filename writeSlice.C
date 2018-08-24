#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <string>

/*
To be ran with ACLIC: root writeSlice.C+

This allows ROOT to build the dictionaries for persistifying the objects. In order
to properly integrate the code in the prototype, one needs to add the headers
and select the RSlice class. The CMake infrastructure will do the rest :)
*/

// The size must be an int
// The comment tells root io that the memory pointed to by fBuffer is fBufferSize
// long and needs to be persistified.
struct RSlice {
   RSlice(){};
   RSlice(const int size, const char* buffer): fBufferSize(size), fBuffer(buffer) {};
   const int fBufferSize = 0;
   const char* fBuffer = nullptr; ///<[fBufferSize]
};

const auto filename = "out.root";
const auto datasetname = "myDataset";

void write()
{
   TFile f(filename, "RECREATE");
   auto datasetDir = f.mkdir(datasetname);
   auto counter = 0;
   for (const auto blob : {"foo", "bar", "Ueberfluessig", "footino"}) {
      const auto bloblen = (int)strlen(blob);
      RSlice slice(bloblen, blob);
      const auto countStr = std::to_string(counter++);
      datasetDir->WriteObject(&slice, countStr.c_str());
   }
}

void read()
{
   TFile f(filename);
   auto datasetDir = f.GetDirectory(datasetname);
   auto keysList = datasetDir->GetListOfKeys();
   for (const auto keyAsTObj : *keysList) {
      auto key = static_cast<TKey*>(keyAsTObj);
      const auto slice = key->ReadObject<RSlice>();
      std::cout << "Key name: " << key->GetName()
                << " - Blob lenght " << slice->fBufferSize
                << " - content ->" << slice->fBuffer << "<-" << std::endl;
   }

}

void writeSlice()
{
   write();
   read();
}

