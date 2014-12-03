LIBS=-lqrencode -lcurl -lpng -lcrypto
LDPATHS?=-L/usr/local/lib
LDFLAGS=$(LDPATHS) $(LIBS)
CFLAGS?=-g -Wall -fPIC -I/usr/local/include
CXX?=clang++
LIB_SRC=blytz-api.cpp blytz-rest.cpp blytz-qr.cpp blytz-enc.cpp blytz-base64.cpp
LIB_OBJ=$(patsubst %.cpp,%.o,$(LIB_SRC))

GTEST_DIR?=/usr/src/gtest
GTEST_SRC=$(GTEST_DIR)/src/gtest-all.cc
GTEST_OBJ=gtest-all.o
GTEST_CFLAGS=-pthread -I$(GTEST_DIR)

CFLAGS+=$(GTEST_CFLAGS)

.PHONY: all clean test

all: libblytz.so

test: blytz-test
	@./blytz-test

install:
	sudo cp libblytz.so /usr/local/lib/

libblytz.so: $(LIB_OBJ)
	$(CXX) --shared -o $@ $(LIB_OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(GTEST_OBJ): $(GTEST_SRC)
	$(CXX) $(CFLAGS) -c $< -o $(shell basename $@)

blytz-test: blytz-test.cpp libblytz.so $(GTEST_OBJ)
	$(CXX) $(CFLAGS) blytz-test.cpp $(GTEST_OBJ) -L. -lblytz $(LDFLAGS) -o blytz-test

clean:
	rm -rf *~ .*~ *.o *.so *.core blytz-test
