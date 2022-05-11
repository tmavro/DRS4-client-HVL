########################################################
#
#  Makefile for drs4client
#  executable under linux
#
########################################################

CFLAGS        = -g -O2 -Wall -Wuninitialized -fno-strict-aliasing -Iinclude -I/usr/local/include -DHAVE_USB -DHAVE_LIBUSB10 
LIBS          = -lpthread -lutil -lusb-1.0

CPP_OBJ       = DRS.o averager.o
OBJECTS       = musbstd.o mxml.o strlcpy.o

all: drs4client

drs4client: $(OBJECTS) $(CPP_OBJ) drs4client.o
	$(CXX) $(CFLAGS) $(OBJECTS) $(CPP_OBJ) drs4client.o -o drs4client $(LIBS)

drs4client.o: src/drs4client.cpp include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) -c $<

$(CPP_OBJ): %.o: src/%.cpp include/%.h include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) -c $<

$(OBJECTS): %.o: src/%.c include/mxml.h include/DRS.h
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f *.o
