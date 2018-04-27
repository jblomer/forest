#ifndef EVENT_H_
#define EVENT_H_

#include "TBuffer.h"
#include "TObject.h"

class Event : public TObject {
public:
   Event() : fEnergy(0.0) { }
   virtual ~Event() { }
   Event(double e) : fEnergy(e) { }
   double fEnergy;

   //void Streamer(TBuffer &b);

   ClassDef(Event,1)
};

#endif  // EVENT_H_
