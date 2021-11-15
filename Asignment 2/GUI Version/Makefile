CC=gcc
CCOPTS=-Wall -g3 -fsanitize=address -pthread

.PHONY: all clean GUIclient server

TARGETS=GUIclient server

GUIclient: GUIclient.c
	gcc -Wno-format -o GUIclient-bin GUIclient.c -Wno-deprecated-declarations -Wno-format-security -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

server: server.c  #the output file should go here, followed by dependencies. 
	$(CC) $(CCOPTS) -o $@ $<

all: $(TARGETS)

clean:
	rm -f $(TARGETS)
