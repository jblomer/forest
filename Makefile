CXXFLAGS_CUSTOM = -Wall -pthread -g -O3 -std=c++14
# -fopt-info-vec-missed
CXXFLAGS_ROOT = $(shell root-config --cflags)
CXXFLAGS = $(CXXFLAGS_CUSTOM) $(CXXFLAGS_ROOT)
LDFLAGS_ROOT = $(shell root-config --libs)
LDFLAGS_CUSTOM = -lROOTForest
LDFLAGS = $(LDFLAGS_ROOT) $(LDFLAGS_CUSTOM)

all: forest \
  dsforest \
	dslhcb \
	open_forest \
	convert \
	store_naod_muon \
  store_lhcbopendata \
	store_nanoaod \
	read_lhcbopendata \
	read_lhcbopendata~deep \
	root_lhcbopendata \
	store_lhcbopendata~deep \
	nanoaod_dimuon_spectrum \
	nanoaod_dimuon_spectrum.orig

.PHONY = clean

event.cxx: event.h event_linkdef.h
	rootcling -f $@ event.h event_linkdef.h

libEvent.so: event.cxx
	g++ -shared -fPIC -o$@ $(CXXFLAGS) event.cxx $(LDFLAGS)

forest: forest.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

dsforest: dsforest.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

dslhcb: dslhcb.cc libEvent.so
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

open_forest: open_forest.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

convert: convert.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

store_nanoaod: store_nanoaod.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

store_naod_muon: store_naod_muon.cc
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
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS_ROOT)

nanoaod_dimuon_spectrum: nanoaod_dimuon_spectrum.cxx
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

nanoaod_dimuon_spectrum.orig: nanoaod_dimuon_spectrum.orig.cxx
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f forest \
	  event.cxx \
	  libEvent.so \
	  dsforest \
		dslhcb \
		convert \
		open_forest \
		store_nanoaod \
	  store_naod_muon \
		store_lhcbopendata \
		store_lhcbopendata~deep \
		read_lhcbopendata \
		read_lhcbopendata~deep \
		root_lhcbopendata \
		nanoaod_dimuon_spectrum nanoaod_dimuon_spectrum.orig
