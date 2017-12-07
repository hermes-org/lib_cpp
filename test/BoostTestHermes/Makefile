PROGRAM = BoostTestHermes

INCLUDEDIRS = -I. -I../../src/include


LIBDIRS = -L/usr/lib -L../../Hermes/.libs

LIBS = -lhermes -lpthread

CXXSOURCES = CApiTest.cpp ConfigurationTest.cpp DownstreamTest.cpp HermesDataTest.cpp SerializerTest.cpp TestMain.cpp Trace.cpp UpstreamTest.cpp


DBGFLAGS = -O2 -g0 -DNDEBUG
ifeq ($(DEBUG),1)
DBGFLAGS = -O0 -g3 -D_DEBUG
endif

CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXFLAGS = -std=c++17 -Wall -DESRI_UNIX $(INCLUDEDIRS) $(DBGFLAGS)
CXX = g++

LDFLAGS = $(LIBDIRS) $(LIBS)

all: $(PROGRAM)

$(PROGRAM): $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS) $(LDFLAGS)

clean:
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)

run:
	./$(PROGRAM)


