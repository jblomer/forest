CXXFLAGS_CUSTOM = -Wall -pthread -g -O3 -std=c++1z
CXXFLAGS_ROOT = $(shell root-config --cflags)
CXXFLAGS = $(CXXFLAGS_CUSTOM) $(CXXFLAGS_ROOT)
LDFLAGS_ROOT = $(shell root-config --libs)
LDFLAGS_CUSTOM = -lstdc++fs
LDFLAGS = $(LDFLAGS_ROOT) $(LDFLAGS_CUSTOM)

UNITS = RBasket.o \
  RTreeMedium.o \
  RTreeModel.o \
  RTreeEntry.o \
	RBranch.o \
	RBranchModel.o \
	RLeaf.o \
	RTree.o

all: libEvent.so compress toybranch streamer rootcmp protobufcmp

.PHONY = clean

event.cxx: event.h event_linkdef.h
	rootcling -f $@ event.h event_linkdef.h

libEvent.so: event.cxx event.cc
	g++ -shared -fPIC -o$@ $(CXXFLAGS) $< event.cc $(LDFLAGS)

%.o: %.cxx %.hxx
	g++ -c $(CXXFLAGS_CUSTOM) $< $(LDFLAGS_CUSTOM)

toybranch: toybranch.cc $(UNITS)
	g++ $(CXXFLAGS_CUSTOM) -o $@ $< $(LDFLAGS_CUSTOM) $(UNITS)

rootcmp: rootcmp.cxx
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

protobufcmp.pb.cc: protobufcmp.proto
	protoc --cpp_out=. protobufcmp.proto

protobufcmp: protobufcmp.cxx protobufcmp.pb.cc
	g++ $(CXXFLAGS_CUSTOM) -o $@ $< protobufcmp.pb.cc $(LDFLAGS_CUSTOM) -lprotobuf

compress: compress.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

streamer: streamer.cxx event.cxx
	g++ $(CXXFLAGS) -o $@ $< event.cxx $(LDFLAGS)

clean:
	rm -f event.cxx libEvent.so toybranch \
	  $(UNITS) \
		rootcmp protobufcmp protobufcmp.pb.cc protobufcmp.pb.h
