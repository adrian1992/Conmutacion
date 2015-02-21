SRC = io.c io.h hashtable.c hashtable.h
CFLAGS = -Wall


all: io.c io.h hashtable.c hashtable.h
	gcc $(CFLAGS) $(SRC) -o my_route_lookup -lm

.PHONY: clean
clean:
	rm -f my_route_lookup

