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

/*
 * Builds the hash tables tree that is the base for the algorithm. This method is recursive.
 * Parameters:
 * 		struct binary_tree **tree: Pointer to the location in memory where the tree will be stored.
 * 		int id: Id of the tree node.
 * Return:
 * 		int: error or succed code.
 * 			-3007 MEMORY_ALLOCATED_ERROR
 * 			0 OK
 */

int build_tree(struct binary_tree **tree, int id);

/*
 * Looks for the leaves on the tree where to put the given prefix. It calls put_redirect to put it
 * inside the hash table. This method is recursive.
 *
 * Parameters:
 * 		int iface: Interface information about this FIB
 * 		uint32_t prefix: IP address information about this FIB
 * 		int length: Length of the netmask. used to find the position in the main table
 * 		struct binary_tree *tree: Pointer to the tree struct where the given FIB is going to be stored.
 * 	Return:
 * 		int: error or succed code.
 * 			-3007 MEMORY_ALLOCATED_ERROR
 * 			0 OK
 */

int put_in_tree( int iface, uint32_t prefix, int length, struct binary_tree *tree );

/*
 * Creates the file system for the program. The main table to which all the hash tables are linked.
 *
 * Parameters:
 * 		int iface: Pointer to the location in memory where the interface information about this FIB
 * 				is stored
 * 		uint32_t prefix: Pointer to the location in memory where the IP address information about this
 * 				FIB	is stored
 * 		int prefixLength: Length of the netmask. used to find the position in the main table
 * 		int sizeHashTable: Size of the hash table.
 * 		struct hash_table ** table: Pointer to the location in memory where the file system was built
 * Return:
 * 		int: error or succed code.
 * 			-3007 MEMORY_ALLOCATED_ERROR
 * 			0 OK
 */

int put_redirect(int iface, uint32_t prefix, int prefixLength, int sizeHashTable, struct hash_table ** table);

/*
 * Searches the output interface for an IP address between the possible collisions.
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
 * Frees all the memory consumed at creation.
 *
 * Parameters:
 * 		struct hash_table ** table: Pointer to the location in memory where the file system was built
 * Return:
 * 		void
 */

void free_table(struct hash_table * table);

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
 * Function definition
 */

int create_tree(struct binary_tree ** tree){
	if( ((*tree) = (struct binary_tree *)malloc( sizeof(struct binary_tree) ) ) == NULL ){
		printf("There was an error allocating memory for the main table, aborting");
		return MEMORY_ALLOCATED_ERROR;
	}
	return build_tree(tree, HALF_IP);
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

int search(uint32_t IPaddress, struct binary_tree * tree){
	int iface = ADDRESS_COULDNT_RESOLVE;
	int aux;
	int hash = hash(getNetmask(tree->prefix) & IPaddress);
	if ((iface = search_redirect(hash, tree->table[hash].first)) != ADDRESS_COULDNT_RESOLVE){
		if((aux = search(IPaddress, tree->rigth)) != ADDRESS_COULDNT_RESOLVE)
			return aux;
		else
			return iface;
	}else{
		if((aux = search(IPaddress, tree->left)) != ADDRESS_COULDNT_RESOLVE)
			return aux;
	}
	return iface;
}

void free_tree(struct binary_tree * tree){
	if(tree->left != NULL)
		free_tree(tree->left);
	if(tree->rigth != NULL)
		free_tree(tree->rigth);
	free_table(tree->table);
	free(tree);
}

/*
 * Private functions
 */

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
	if(first == NULL)
		return ADDRESS_COULDNT_RESOLVE;
	else
	{
		struct redirect * aux = first;
		while(aux->IPAddress != IPaddress && aux->next != NULL)
			aux = aux->next;
		if(aux->IPAddress != IPaddress)
			return ADDRESS_COULDNT_RESOLVE;
		else
			return aux->iface;
	}
}

void free_table(struct hash_table *table, int size){
	int i ;
	for (i=0;i< size;i++)
		free_redirect(table[i].first);
	free(table);
}

void free_redirect(struct redirect *first){
	if(first == NULL)
		return;
	if(first->next != NULL)
		free_redirect(first->next);
	free(first);
}
