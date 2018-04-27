CXXFLAGS_CUSTOM = -Wall -pthread -g -O2
CXXFLAGS_ROOT = $(shell root-config --cflags)
LDFLAGS_ROOT = $(shell root-config --libs)
CXXFLAGS = $(CXXFLAGS_CUSTOM) $(CXXFLAGS_ROOT)
LDFLAGS = $(LDFLAGS_CUSTOM) $(LDFLAGS_ROOT) -lstdc++fs

UNITS = TTreeMedium.o \
  RTreeModel.o \
  RTreeEntry.o \
	TBranch.o \
	RBranchModel.o \
	RLeaf.o \
	RTree.o

all: libEvent.so compress toybranch streamer

.PHONY = clean

event.cxx: event.h event_linkdef.h
	rootcling -f $@ event.h event_linkdef.h

libEvent.so: event.cxx event.cc
	g++ -shared -fPIC -o$@ $(CXXFLAGS) $< event.cc $(LDFLAGS)

%.o: %.cxx %.hxx
	g++ -c $(CXXFLAGS) $< $(LDFLAGS)

toybranch: toybranch.cc libEvent.so $(UNITS)
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS) $(UNITS)

compress: compress.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

streamer: streamer.cxx event.cxx
	g++ $(CXXFLAGS) -o $@ $< event.cxx $(LDFLAGS)

clean:
	rm -f event.cxx libEvent.so toybranch \
	  $(UNITS)
