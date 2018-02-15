#ifndef EVENT_H_
#define EVENT_H_

#include <string>
#include <vector>

class KaonCandidate {
 public:
  KaonCandidate()
    : h_px(0.0), h_py(0.0), h_pz(0.0)
    , h_prob_k(0.0), h_prob_pi(0.0)
    , h_is_muon(0)
    , h_ip_chi2(0.0)
  { }

  double h_px, h_py, h_pz;
  double h_prob_k, h_prob_pi;
  int h_charge;
  int h_is_muon;
  double h_ip_chi2;
};

struct Event {
  double b_flight_distance;
  double b_vertex_chi2;
  std::vector<KaonCandidate> kaon_candidates;
};



/*** FROM ROOT Course */
struct Header;
struct Track {
  double vertex; // more complex type
  double position;  // more complex type
};
struct Particle {
  double pt;
  double energy;
  int charge;
  Track track;
};

/*struct Event {
  Header header;
  int type;
  std::vector<Particle> particles;

};*/


//-----------------------
/*
struct Hadron {
  int id;
};

struct Jet {
  double px;
  double py;
  double pz;
  std::vector<Hadron> hadrons;
};

struct Record {
  int id;
  std::vector<std::string> tags;
  std::vector<Jet> jets;
};


// ID
// @TAGS
// TAGS
// @JETS
// JETS.PY

BASKET
  py_1  (Ev 1)
  py_2  (Ev 1)
  py_3  (Ev 2)
  py_4  (Ev 2)
  py_5  (Ev 2)
....
  offsets.
  0 (byte offset)
  2

// JETS.PZ
// JETS.PX
// JETS.HADRONS


// @JETS.HADRONS
// JETS.HADRONS.ID
*/
#endif  // EVENT_H_
