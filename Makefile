LDLIBS=-lqrencode -lcurl -lpng -lcrypto
LDPATHS=-L/usr/local/lib
LDFLAGS=$(LDPATHS) 
CCPATHS=-I/usr/local/include
DEBUG=-g -Wall
COMPILER=g++

all: libblytz.so

install:
	sudo cp libblytz.so /usr/local/lib/

libblytz.so: blytz-api.o blytz-rest.o blytz-qr.o \
	blytz-enc.o blytz-base64.o
	$(COMPILER) $(DEBUG) --shared -o libblytz.so \
		blytz-rest.o blytz-api.o blytz-qr.o blytz-enc.o \
		blytz-base64.o $(LDFLAGS) $(LDLIBS)

blytz-api.o: blytz-api.cpp
	$(COMPILER) $(DEBUG) -fPIC $(CCPATHS) -c blytz-api.cpp -o blytz-api.o

blytz-rest.o: blytz-rest.cpp
	$(COMPILER) $(DEBUG) -fPIC $(CCPATHS) -c blytz-rest.cpp -o blytz-rest.o

blytz-qr.o: blytz-qr.cpp
	$(COMPILER) $(DEBUG) -fPIC $(CCPATHS) -c blytz-qr.cpp -o blytz-qr.o

blytz-enc.o: blytz-enc.cpp
	$(COMPILER) $(DEBUG) -fPIC $(CCPATHS) -c blytz-enc.cpp -o blytz-enc.o

blytz-base64.o: blytz-base64.cpp
	$(COMPILER) $(DEBUG) -fPIC $(CCPATHS) -c blytz-base64.cpp -o blytz-base64.o

clean:
	rm -rf *~ .*~ *.o *.so
