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

#include "hash_table.h"

/*
 * Prototypes
 */

int build_tree(struct binary_tree **tree, int id);

/*
 * Frees the redirect list and its pointers to other memory locations.
 *
 * Parameters:
 * 		struct hash_table  table: parent of the redirect list.
 * Return:
 * 		void
 */

void free_redirect(struct hash_table  *table);

/*
 * Creates the file system for the program. The main table to which all the hash tables are linked.
 *
 * Parameters:
 * 		int * iface: Pointer to the location in memory where the interface information about this FIB
 * 				is stored
 * 		uint32_t *prefix: Pointer to the location in memory where the IP address information about this
 * 				FIB	is stored
 * 		int prefixLength: Length of the netmask. used to find the position in the main table
 * 		int hash: Indes of the data given inside the hash table.
 * 		struct hash_table ** table: Pointer to the location in memory where the file system was built
 * Return:
 * 		int: error or succed code.
 * 			-3007 MEMORY_ALLOCATED_ERROR
 * 			0 OK
 */

int put_redirect(int iface, uint32_t prefix, int prefixLength, int hashed, struct hash_table *** table);

/*
 * Searches the output interface for an IP address. between the possible collisions.
 *
 * Parameters:
 * 		uint32_t IPaddress: Ip address to search in the tables.
 * 		struct redirect *first: first element of the list containing all the FIB collisions for a hash
 * Return:
 * 		int *: error or interface number.
 * 			-3008 ADDRESS_COULDNT_RESOLVE
 * 			iface
 */

int search_redirect(uint32_t IPaddress, struct redirect *first);

/*
 * Function definition
 */

int create_tree(struct binary_tree ** tree){
	if( ((*tree) = (struct binary_tree *)malloc( sizeof(struct binary_tree) ) ) == NULL ){
		printf("There was an error allocating memory for the main table, aborting");
		return MEMORY_ALLOCATED_ERROR;
	}
	return build_tree(tree, HALF_IP);
}

void free_tree(struct hash_table ** table){

}

int put( struct binary_tree **tree ){
	uint32_t prefix = 0;
	int prefixLength, outInterface, error;
	if( (error = readFIBLine(&prefix, &prefixLength, &outInterface)) == OK ){
		put_in_tree(outInterface, prefix, prefixLength, *tree);
		return OK;
	}
	return error;
}

int search(uint32_t IPaddress, struct hash_table ** table, int *hash_lookup){

}

int build_tree(struct binary_tree **tree, int id){
	(*tree)->prefix=id;
	if( ((*tree)->table=(struct hash_table *)malloc( sizeof(struct hash_table) ) ) == NULL ){
		printf("There was an error allocating memory for the main table, aborting");
		return MEMORY_ALLOCATED_ERROR;
	}
	// Odd nodes
	if( (id % 2) == 1 ){
		(*tree)->rigth=NULL;
		(*tree)->left=NULL;
		if(id == 1){
			if( ((*tree)->left=(struct binary_tree *)malloc( sizeof(struct binary_tree) ) ) == NULL ){
				printf("There was an error allocating memory for the main table, aborting");
				return MEMORY_ALLOCATED_ERROR;
			}else{
				build_tree(&((*tree)->left),0);
			}
		}
		if(id == 31){
			if( ((*tree)->rigth=(struct binary_tree *)malloc( sizeof(struct binary_tree) ) ) == NULL ){
				printf("There was an error allocating memory for the main table, aborting");
				return MEMORY_ALLOCATED_ERROR;
			}else{
				build_tree(&((*tree)->rigth),32);
			}
		}
	}// End odd nodes
	else{
		// Even nodes
		// Bottom node
		if( id==0 || id==32 ){
			(*tree)->rigth=NULL;
			(*tree)->left=NULL;
			return OK;
		}// End bottom node
		// Superior nodes
		if(id%4 == 0){
			if( ((*tree)->left=(struct binary_tree *)malloc( sizeof(struct binary_tree) ) ) == NULL ){
				printf("There was an error allocating memory for the main table, aborting");
				return MEMORY_ALLOCATED_ERROR;
			}else{
				build_tree(&((*tree)->left), id-id/2);
			}
			if( ((*tree)->rigth=(struct binary_tree *)malloc( sizeof(struct binary_tree) ) ) == NULL ){
				printf("There was an error allocating memory for the main table, aborting");
				return MEMORY_ALLOCATED_ERROR;
			}else{
				build_tree(&((*tree)->rigth), id+id/2);
			}
			// End superior nodes
		}else{
			// Middle nodes
			if( ((*tree)->left=(struct binary_tree *)malloc( sizeof(struct binary_tree) ) ) == NULL ){
				printf("There was an error allocating memory for the main table, aborting");
				return MEMORY_ALLOCATED_ERROR;
			}else{
				build_tree(&((*tree)->left), id-1);
			}
			if( ((*tree)->rigth=(struct binary_tree *)malloc( sizeof(struct binary_tree) ) ) == NULL ){
				printf("There was an error allocating memory for the main table, aborting");
				return MEMORY_ALLOCATED_ERROR;
			}else{
				build_tree(&((*tree)->rigth), id+1);
			}
			// End middle nodes
		}
	}
	return OK;
}

void free_redirect(struct hash_table *table){

}

int put_in_tree( int iface, uint32_t prefix, int length, struct binary_tree *tree ){
	int size;
	if(prefix > HALF_IP){
		size = 1 << HALF_IP;
	}else{
		size = 1 << tree->prefix;
	}
	if(prefix == tree->prefix){
		return put_redirect(iface, prefix, length, size, &(tree->table));
	}
	if( prefix > tree->prefix ){
		if(put_redirect(iface, prefix, length, size, &(tree->table)) == OK)
			return put_in_tree(iface, prefix, length, tree->rigth);
		else
			return MEMORY_ALLOCATED_ERROR;
	}else
		return put_in_tree(iface, prefix, length, tree->left);
}

int put_redirect(int iface, uint32_t prefix, int prefixLength, int sizeHashTable, struct hash_table ** table){
	int hash = hash(prefix, sizeHashTable);
	if( (*table)[hash].first == NULL ){
		if( ((*table)[hash].first = (struct redirect *)calloc(1, sizeof(struct redirect))) == NULL ){
			printf("There was an error allocating memory for the main table, aborting");
			return MEMORY_ALLOCATED_ERROR;
		}
		(*table)[hash].first->IPAddress=prefix;
		(*table)[hash].first->iface=iface;
		return OK;
	}else{
		struct redirect * aux = (*table)[hash].first;
		while(aux->next != NULL)
			aux = aux->next;
		if( (aux->next = (struct redirect *)calloc(1, sizeof(struct redirect))) == NULL ){
			printf("There was an error allocating memory for the main table, aborting");
			return MEMORY_ALLOCATED_ERROR;
		}
		aux->next->IPAddress=prefix;
		aux->next->iface=iface;
		return OK;
	}
}

int search_redirect(uint32_t IPaddress, struct redirect *first){

}
