CXXFLAGS_CUSTOM = -Wall -pthread -g -O2 -std=c++14
# -fopt-info-vec-missed
CXXFLAGS_ROOT = $(shell root-config --cflags)
CXXFLAGS = $(CXXFLAGS_CUSTOM) $(CXXFLAGS_ROOT)
LDFLAGS_ROOT = $(shell root-config --libs)
LDFLAGS_CUSTOM = -lstdc++fs
LDFLAGS = $(LDFLAGS_ROOT) $(LDFLAGS_CUSTOM)

UNITS = \
  RColumn.o \
  RColumnElement.o \
  RColumnModel.o \
  RColumnRange.o \
  RColumnSlice.o \
  RColumnStorage.o \
  RCargo.o \
  RBranch.o \
  RTreeModel.o \
  RTreeEntry.o \
  RTree.o \
  RTreeView.o

#  RDataObject.o
#  RDataMapper.o
#  RDataModel.o
#  RDataEntry.o
#  RDataSet.o
#  RDataView.o

all: libEvent.so compress toybranch streamer rootcmp protobufcmp fillbench

.PHONY = clean

event.cxx: event.h event_linkdef.h
	rootcling -f $@ event.h event_linkdef.h

libEvent.so: event.cxx event.cc
	g++ -shared -fPIC -o$@ $(CXXFLAGS) $< event.cc $(LDFLAGS)

%.o: %.cxx $(wildcard *.hxx)
	g++ -c $(CXXFLAGS_CUSTOM) -std=c++2a $< $(LDFLAGS_CUSTOM)

toybranch: toybranch.cc $(UNITS)
	g++ $(CXXFLAGS_CUSTOM) -std=c++2a -o $@ $< $(LDFLAGS_CUSTOM) $(UNITS)

rootcmp: rootcmp.cxx
	echo "*** USING: $$ROOTSYS ***"
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

fillbench: fillbench.cxx
	echo "*** USING: $$ROOTSYS ***"
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
