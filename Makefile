LDLIBS=-lqrencode -lcurl -lpng -lcrypto
LDPATHS=-L/usr/local/lib
LDFLAGS=$(LDPATHS) 
CCPATHS=-I/usr/local/include
DEBUG=-g -Wall
COMPILER=clang++
GTEST_DIR=gtest-1.7.0/

all: libblytz.so

test: blytz-test.o

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

blytz-debug.o: blytz-debug.cpp blytz-debug.h
	$(COMPILER) $(DEBUG) -fPIC $(CCPATHS) -c blytz-debug.cpp -o blytz-debug.o

blytz-test.o: blytz-test.cpp libblytz.so
	$(COMPILER) $(DEBUG) $(CCPATHS) blytz-test.cpp  -L. -lblytz -lgtest $(LDFLAGS) -pthread -o blytz-test && ./blytz-test

clean:
	rm -rf *~ .*~ *.o *.so
