#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

#include <cassert>
#include <iostream>
#include <string>

class RUInt16BE {
private:
   std::uint16_t fValBE = 0;
public:
   operator std::uint16_t() const {
      return (fValBE & 0x00FF) << 8 | (fValBE & 0xFF00) >> 8;
   }
};

class RUInt32BE {
private:
   std::uint32_t fValBE = 0;
public:
   operator std::uint32_t() const {
      auto x = (fValBE & 0x0000FFFF) << 16 | (fValBE & 0xFFFF0000) >> 16;
      return (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
   }
};

class RUInt64BE {
private:
   std::uint64_t fValBE = 0;
public:
   operator std::uint64_t() const {
      auto x = (fValBE & 0x00000000FFFFFFFF) << 32 | (fValBE & 0xFFFFFFFF00000000) >> 32;
      x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
      return (x & 0x00FF00FF00FF00FF) << 8  | (x & 0xFF00FF00FF00FF00) >> 8;
   }
};

#pragma pack(push, 1)
struct RFileHeader {
   char fMagic[4];
   RUInt32BE fVersion;
   RUInt32BE fBEGIN;

   union {
      struct {
         RUInt32BE fEND;
         RUInt32BE fSeekFree;
         RUInt32BE fNbytesFree;
         RUInt32BE fNfree;
         RUInt32BE fNbytesName;
         unsigned char fUnits;
         RUInt32BE fCompress;
         char fUUID[16];
      } fInfoShort;

      struct {
         RUInt64BE fEND;
         RUInt64BE fSeekFree;
         RUInt32BE fNbytesFree;
         RUInt32BE fNfree;
         RUInt32BE fNbytesName;
         unsigned char fUnits;
         RUInt32BE fCompress;
         char fUUID[16];
      } fInfoLong;
   };

   RFileHeader() : fInfoLong() {}

   void Print() {
      std::cout << std::string(fMagic) << "  " << fVersion << " " << fBEGIN << " " << fInfoShort.fEND << " " <<
         "fSeekFree:" << fInfoShort.fSeekFree << " fNbytesFree:" << fInfoShort.fNbytesFree <<
         " fNfree:" << fInfoShort.fNfree << " fUnits:" << +fInfoShort.fUnits << " fCompress:" << fInfoShort.fCompress << std::endl;
   }
};

struct RKeyHeader {
   RUInt32BE fNbytes;
   RUInt16BE fVersion;
   RUInt32BE fObjLen;
   RUInt32BE fDatetime;
   RUInt16BE fKeyLen;
   RUInt16BE fCycle;

   union {
      struct {
         RUInt32BE fSeekPdir;
         RUInt32BE fSeekKey;
         char fNameBuffer[771];
      } fInfoShort;

      struct {
         RUInt64BE fSeekPdir;
         RUInt64BE fSeekKey;
         char fNameBuffer[771];
      } fInfoLong;
   };

   RKeyHeader() : fInfoLong() {}

   std::string GetClass() const {
      unsigned char lname = fInfoShort.fNameBuffer[0];
      return std::string(&fInfoShort.fNameBuffer[1], lname);
   }

   std::string GetName() const {
      unsigned int offset = 1 + fInfoShort.fNameBuffer[0];
      unsigned char lname = fInfoShort.fNameBuffer[offset];
      return std::string(&fInfoShort.fNameBuffer[1 + offset], lname);
   }

   std::string GetTitle() const {
      unsigned int offset = 1 + fInfoShort.fNameBuffer[0];
      offset += fInfoShort.fNameBuffer[offset] + 1;
      unsigned char lname = fInfoShort.fNameBuffer[offset];
      return std::string(&fInfoShort.fNameBuffer[1 + offset], lname);
   }

   void Print() {
      std::cout << "Version: " << fVersion << " Size:" << fNbytes << "/" << fObjLen << " seek:" << fInfoShort.fSeekKey
         << " Dir:" << fInfoShort.fSeekPdir << " Cycle:" << fCycle << " KeyLen: " << fKeyLen
         << " Class:" << GetClass() << " Object:" << GetName() << " Title:" << GetTitle() << std::endl;
   }

   std::uint32_t GetSize() const { return fNbytes; }
};
#pragma pack(pop)

int main(int argc, char** argv) {
   int fd = open(argv[1], O_RDONLY);

   RFileHeader fh;
   read(fd, &fh, sizeof(fh));
   fh.Print();

   std::uint64_t pos = fh.fBEGIN;
   while (pos < fh.fInfoShort.fEND) {
      RKeyHeader kh;
      pread(fd, &kh, sizeof(kh), pos);
      kh.Print();
      if (kh.fInfoShort.fSeekKey > 0) {
         //std::cout << "   POS " << pos << " SEEK:" << kh.fInfoShort.fSeekKey << std::endl;
         if (kh.fInfoShort.fSeekKey != pos) {
            std::cout << "   OUPPS:   POS " << pos << " SEEK:" << kh.fInfoShort.fSeekKey << std::endl;
         }
      }
      pos += kh.GetSize();
   }

   return 0;
}