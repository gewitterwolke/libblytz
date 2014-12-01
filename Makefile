LIBS=-lqrencode -lcurl -lpng -lcrypto
LDPATHS=-L/usr/local/lib
LDFLAGS=$(LDPATHS) $(LIBS)
CFLAGS?=-g -Wall -fPIC -I/usr/local/include
CXX?=clang++
GTEST_DIR=gtest-1.7.0/
LIB_SRC=blytz-api.cpp blytz-rest.cpp blytz-qr.cpp blytz-enc.cpp blytz-base64.cpp
LIB_OBJ=$(patsubst %.cpp,%.o,$(LIB_SRC))

all: libblytz.so

test: blytz-test.o

install:
	sudo cp libblytz.so /usr/local/lib/

libblytz.so: $(LIB_OBJ)
	$(CXX) $(DEBUG) --shared -o $@ $(LIB_OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

blytz-test.o: blytz-test.cpp libblytz.so install
	$(CXX) $(CFLAGS) blytz-test.cpp  -L. -lblytz -lgtest $(LDFLAGS) -pthread -o blytz-test && ./blytz-test

clean:
	rm -rf *~ .*~ *.o *.so *.core blytz-test
