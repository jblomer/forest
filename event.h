#ifndef EVENT_H_
#define EVENT_H_

#include <vector>

#include "TBuffer.h"
#include "TObject.h"

struct Hit {
  float x;
  float y;
};

struct Track {
  float energy;
  std::vector<Hit> hits;
};

struct Event {
  float h1_px;
  float h1_py;
  float h1_pz;
  float h2_px;
  float h2_py;
  float h2_pz;
  float h3_px;
  float h3_py;
  float h3_pz;
  float unsafe;
  std::vector<Track> tracks;
   //void Streamer(TBuffer &b);

   //ClassDef(Event,1)
};

#endif  // EVENT_H_
