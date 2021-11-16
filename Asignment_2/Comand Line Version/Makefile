TARGETS=client server

CC=gcc
CCOPTS=-Wall -g3 -fsanitize=address -pthread

.PHONY: all clean pristine

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

pristine: clean

%: %.c
	$(CC) $(CCOPTS) -o $@ $<
