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

rebuild: build
	./sbuild/rebuild.sh

clean:
	rm -f cycc *.o tmp* *.s
	rm -f sbuild/cycc sbuild/cycc1 sbuild/cycc2 sbuild/*.o sbuild/*.s sbuild/*.c

.PHONY:
	clean test build rebuild

