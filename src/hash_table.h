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

/*
 * Creates the file system for the program. The main table to which all the hash tables are linked.
 *
 * Parameters:
 * 		struct hash_table ** table: Pointer to the location in memory where the file system was built
 * Return:
 * 		int: error or succed code.
 * 			-3007 MEMORY_ALLOCATED_ERROR
 * 			0 OK
 */

int create_table(struct hash_table ** table);

/*
 * Frees all the memory consumed at creation.
 *
 * Parameters:
 * 		struct hash_table ** table: Pointer to the location in memory where the file system was built
 * Return:
 * 		void
 */

void free_table(struct hash_table ** table);

/*
 * Puts a new FIB in the file system. The initializeIO(char *routingTableName, char *inputFileName) function
 * Should have been used before this method is called.
 *
 * Parameters:
 * 		struct hash_table ** table: Pointer to the location in memory where the file system was built
 * Return:
 * 		int: error or succed code.
 * 			-3007 MEMORY_ALLOCATED_ERROR
 * 			0 OK
 */

int put( struct hash_table ** table );
