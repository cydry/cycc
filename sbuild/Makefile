CC = ./cycc
AS = gcc
LD = gcc
SRCS = $(wildcard *.c)
ASMS = $(SRCS:.c=.s)
OBJS = $(ASMS:.s=.o)

cycc1: $(OBJS) $(ASMS)
	$(LD) -o $@ $(OBJS)

%.o: %.s
	$(AS) -c $<

%.s: %.c
	$(CC) $< > $@

clean:
	rm -f cycc1 *.o tmp* *.s

.PHONY:
	clean

