/*
 *  Copyright (C) 2015 Alberdi Ainciburu, Adrián
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include "io.h"
#include "aux.h"

#ifndef _HASH_TABLE
#define _HASH_TABLE

#define HASH_LENGTH 32


struct redirect{
	uint32_t *IPAddress;
	int *iface;
	struct redirect *next;
};

struct hash_table{
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

int create_table(struct hash_table *** table);

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
 *			-3003 REACHED_EOF 
 * 			-3007 MEMORY_ALLOCATED_ERROR
 * 			0 OK
 */

int put( struct hash_table *** table );

/*
 * Searches the output interface for an IP address.
 *
 * Parameters:
 * 		uint32_t IPaddress: Ip address to search in the tables.
 * 		struct hash_table ** table: Pointer to the location in memory where the file system was built
 * Return:
 * 		int *: error or interface number.
 * 			-3008 ADDRESS_COULDNT_RESOLVE
 * 			iface
 */

int search(uint32_t IPaddress, struct hash_table ** table, int *hash_lookup);

#endif //_HASH_TABLE
