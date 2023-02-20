CC = gcc

CFLAGS += -Wall -Ofast

DFLAGS +=

LIBS += -lpthread

LDFLAGS += ${LIBS}

CDFLAGS += ${CFLAGS} ${DFLAGS}

TARGET = \
					main \

OBJECT = \
					${TARGET:=.o} \
					thread.o \

.PHONY: all clean

all: ${TARGET}

${TARGET}: ${OBJECT}
	${CC} -o $@ $^ ${LDFLAGS}

.c.o:
	@${CC} -c ${CDFLAGS} $<

clean:
	rm -rf *.o ${TARGET}