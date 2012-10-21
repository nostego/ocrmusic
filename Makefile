CFLAGS=-W -Wall -Wextra -Wunused-function -Werror -O3
DEFINE=
IFLAGS=
LINK=-lcv -lhighgui -lcxcore
CC=g++
TARGET=ocrsheet

all: ${TARGET}

include Makefile.inc

Makefile.inc:
	./configure

${TARGET}: ${OBJS}
	g++ ${OBJS} ${LINK} -o $@

clean:
	rm -rf src/*.o *.o *.a *.so a.out libsound.so
	rm -f `find . -name '*.o'`
	rm -f `find . -name '*~'`
	rm -f `find . -name '*#'`
	rm -f Makefile.inc ${TARGET}
