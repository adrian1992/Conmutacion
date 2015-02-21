SRC = linearSearch.c linearSearch.h io.c io.h hashtable.c hashtable.h
CFLAGS = -Wall


all: linearSearch


linearSearch:linearSearch.c linearSearch.h io.c io.h hashtable.c hashtable.h
	gcc $(CFLAGS) $(SRC) -o linearSearch -lm

.PHONY: clean
clean:
	rm -f linearSearch

