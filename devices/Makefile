
CC = gcc
LIBS  = -lndn-lite
CFLAGS = -O3 -I ../hardware-interface/
SRCS = $(wildcard *.c)
PROGS = $(patsubst %.c,%,$(SRCS))
TASK = senarial 1
all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) --include "configs/$(TASK)/$@"
clean: 
	rm -f $(PROGS)

