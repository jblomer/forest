CXXFLAGS_CUSTOM = -Wall -pthread -g -O2
CXXFLAGS_ROOT = $(shell root-config --cflags)
LDFLAGS_ROOT = $(shell root-config --libs)
CXXFLAGS = $(CXXFLAGS_CUSTOM) $(CXXFLAGS_ROOT)
LDFLAGS = $(LDFLAGS_CUSTOM) $(LDFLAGS_ROOT)

all: libEvent.so toybranch

.PHONY = clean

event.cxx: event.h event_linkdef.h
	rootcling -f $@ event.h event_linkdef.h

libEvent.so: event.cxx
	g++ -shared -fPIC -o$@ $(CXXFLAGS) $< $(LDFLAGS)

toybranch: toybranch.cc libEvent.so
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f event.cxx libEvent.so toybranch