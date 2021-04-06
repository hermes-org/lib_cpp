DBGFLAGS = -O2 -g0 -DNDEBUG
ifeq ($(DEBUG),1)
DBGFLAGS = -O0 -g3 -D_DEBUG
endif

VERSION= 3:1
CFLAGS = -std=c++17
LFLAGS = -lpugixml

CCOMPILER=libtool --mode=compile g++ -Wall -Wno-unused -fPIC -shared $(DBGFLAGS) -D_REENTRANT -DENV_LINUX -I.  -I../include $(CFLAGS)
# -fvisibility=hidden

SYSLIBRARIES=

LIBS =

EXECUTABLE=libhermes.la

LINK=libtool --mode=link g++ -shared -rpath /usr/lib64 -version-info $(VERSION)
#-fvisibility=hidden-export-symbols-regex 'Hermes'

OBJECTS = ConfigurationClient.lo ConfigurationService.lo ConfigurationServiceSerializer.lo ConfigurationServiceSession.lo \
	Downstream.lo DownstreamSerializer.lo DownstreamSession.lo DownstreamStateMachine.lo Serialization.lo Upstream.lo \
	UpstreamSerializer.lo UpstreamSession.lo UpstreamStateMachine.lo AsioServer.lo AsioClient.lo DeserializationHelper.lo \
	GeneratedHermesGuid.lo SenderEnvelope.lo


default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK) $(SYSLDFLAGS) $(LDFLAGS) -o $(EXECUTABLE) $(OBJECTS) -Wl,-whole-archive $(LIBS) -Wl,-no-whole-archive

%.lo: %.cpp
	$(CCOMPILER) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)



