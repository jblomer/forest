CXXFLAGS_CUSTOM = -Wall -pthread -g -O3 -std=c++14
# -fopt-info-vec-missed
CXXFLAGS_ROOT = $(shell root-config --cflags)
CXXFLAGS = $(CXXFLAGS_CUSTOM) $(CXXFLAGS_ROOT)
LDFLAGS_ROOT = $(shell root-config --libs)
LDFLAGS_CUSTOM = -lROOTForest
LDFLAGS = $(LDFLAGS_ROOT) $(LDFLAGS_CUSTOM)

all: forest store_lhcbopendata

.PHONY = clean

forest: forest.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

store_lhcbopendata: store_lhcbopendata.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f forest \
	  store_lhcbopendata
