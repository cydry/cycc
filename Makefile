CC = gcc
CFLAGS = -g -std=c11 -Wall -static
LD = gcc
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

cycc: $(OBJS)
	$(LD) -o $@ $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

test: cycc
	./test.sh ${ARGS}

build: cycc
	./sbuild.sh

clean:
	rm -f cycc *.o tmp* *.s
	rm -f sbuild/cycc sbuild/*.o sbuild/*.s sbuild/*.c sbuild/*.h

.PHONY:
	clean test build

