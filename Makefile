appname := redisTool

CXX := /opt/rh/devtoolset-2/root/usr/bin/g++
CXXFLAGS := -g -I. --std=c++11 -I/usr/local/include

LDFLAGS := -lhiredis -lrt

#srcfiles := $(shell find . -maxdepth 1 -name "*.cpp")
srcfiles := RedisInstrumentCache.cpp RedisInstrument.cpp RedisConnection.cpp main.cpp
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(objects) $(LDLIBS)

depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects) $(appname)

dist-clean: clean
	rm -f *~ .depend

include .depend
