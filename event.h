#ifndef EVENT_H_
#define EVENT_H_

class Event {
public:
   Event() : fEnergy(0.0) { }
   Event(double e) : fEnergy(e) { }
   double fEnergy;
};

#endif  // EVENT_H_
