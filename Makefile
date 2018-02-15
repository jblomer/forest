CXXFLAGS_CUSTOM = -std=c++14 -Wall -pthread -g -O2
CXXFLAGS_ROOT = $(shell root-config --cflags)
LDFLAGS_ROOT = $(shell root-config --libs)
CXXFLAGS = $(CXXFLAGS_CUSTOM) $(CXXFLAGS_ROOT)
LDFLAGS = $(LDFLAGS_CUSTOM) $(LDFLAGS_ROOT)

all: libEvent.so toybranch

.PHONY = clean

event.cxx: event.h event_linkdef.h
	rootcling -f $@ event.h event_linkdef.h

libEvent.so: event.cxx
	g++ -shared -fPIC -o$@ $(CXXFLAGS) event.cxx $(LDFLAGS)

#lhcb_opendata: lhcb_opendata.cc lhcb_opendata.h util.h util.o lhcb_opendata.pb.cc event.h
#	g++ $(CXXFLAGS) -o lhcb_opendata lhcb_opendata.cc lhcb_opendata.pb.cc util.o \
#		-lhdf5 -lhdf5_hl -lsqlite3 -lavro -lprotobuf $(LDFLAGS) -lz $(LIBS_PARQUET)

#toybranch: toybranch.cc event.h libEvent.so
#	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

toybranch: toybranch.cc
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f event.cxx libEvent.so toybranch
