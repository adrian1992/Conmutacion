#include <stdio.h>
#include "io.h"

typedef struct redirect{
	uint32_t *IPAddress;
	int *iface;
	struct redirect *next;
};

typedef struct hash_table{
	int iface;
	struct redirect *first;
};


struct hash_table ** create_table();

void free_table(struct hash_table ** table);
