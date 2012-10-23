CFLAGS=-W -Wall -Wextra -Wunused-function -O3
DEFINE=
IFLAGS=-I src
LINK=-lcv -lhighgui -lcxcore
CC=g++
TARGET=ocrsheet

all: ${TARGET}

include Makefile.inc

Makefile.inc:
	./configure

${TARGET}: ${OBJS} src/main.o
	g++ ${OBJS} src/main.o ${LINK} -o $@

check: ./tests/test
	(cd tests && ./check.sh)

./tests/test: ${OBJS} ./tests/main.o
	g++ ${OBJS} ./tests/main.o ${LINK} -o $@

%.o: %.cc
	${CC} -c $^ ${DEFINE} ${CFLAGS} ${IFLAGS} -o $@

clean:
	rm -rf src/*.o *.o *.a *.so a.out libsound.so
	rm -f `find . -name '*.o'`
	rm -f `find . -name '*~'`
	rm -f `find . -name '*#'`
	rm -f Makefile.inc ${TARGET} ./tests/test output.png
