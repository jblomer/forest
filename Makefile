CXXFLAGS_CUSTOM = -Wall -pthread -g -O3 -std=c++14
# -fopt-info-vec-missed
CXXFLAGS_ROOT = $(shell root-config --cflags)
CXXFLAGS = $(CXXFLAGS_CUSTOM) $(CXXFLAGS_ROOT)
LDFLAGS_ROOT = $(shell root-config --libs)
LDFLAGS_CUSTOM = -lROOTForest
LDFLAGS = $(LDFLAGS_ROOT) $(LDFLAGS_CUSTOM)

all: forest \
  dsforest \
  store_lhcbopendata \
	read_lhcbopendata \
	read_lhcbopendata~deep \
	root_lhcbopendata \
	store_lhcbopendata~deep

.PHONY = clean

forest: forest.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

dsforest: dsforest.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

store_lhcbopendata: store_lhcbopendata.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

store_lhcbopendata~deep: store_lhcbopendata~deep.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

read_lhcbopendata: read_lhcbopendata.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

read_lhcbopendata~deep: read_lhcbopendata~deep.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

root_lhcbopendata: root_lhcbopendata.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f forest \
	  dsforest \
	  store_lhcbopendata \
		store_lhcbopendata~deep \
		read_lhcbopendata \
		read_lhcbopendata~deep \
		root_lhcbopendata
