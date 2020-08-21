CC = gcc
LIBS  = -lndn-lite
CFLAGS = -O3 -I ../hardware-interface/
SRCS = $(wildcard *.c)
PROGS = $(patsubst %.c,%,$(SRCS))
all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) 
clean: 
	rm -f $(PROGS)
