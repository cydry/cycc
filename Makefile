CC = gcc
CFLAGS = -g -std=c11 -Wall -static
LD = gcc
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

main: $(OBJS)
	$(LD) -o $@ $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

test: main
	./test.sh ${ARGS}

clean:
	rm -f main *.o tmp*

.PHONY:
	clean test

