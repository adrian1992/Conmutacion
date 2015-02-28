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

int create_table(struct hash_table *** table){
	int i, stable = 1<<16;
	if( (*table = (struct hash_table **)malloc( 33*sizeof(struct hash_table *) ) ) == NULL ){
		printf("There was an error allocating memory for the main table, aborting");
		return MEMORY_ALLOCATED_ERROR;
	}
	for(i=0;i<16;i++){
		if( NULL == ((*table)[i]=(struct hash_table *)calloc( 1<<i, sizeof(struct hash_table) ))){
			printf("There was an error allocating memory for the %d hash table, aborting", i+1);
			return MEMORY_ALLOCATED_ERROR;
		}
	}
	for(i=16;i<33;i++){
		if( NULL == ((*table)[i]=(struct hash_table *)calloc( stable, sizeof(struct hash_table) ))){
			printf("There was an error allocating memory for the %d hash table, aborting", i+1);
			return MEMORY_ALLOCATED_ERROR;
		}
	}
	return OK;
}

void free_table(struct hash_table ** table){
	int i;
	for (i=0;i<33;i++){
		free_redirect(table[i]);
		free(table[i]->first);
		free(table[i]);
	}
}

void free_redirect(struct hash_table *table){
	struct redirect * aux, * aux2;
	if((aux=table->first) == NULL)
		return;
	while((aux2=aux->next)!=NULL){
		free(aux->IPAddress);
		free(aux->iface);
		free(aux);
		aux=aux2;
	}
	free(aux);
}

int put( struct hash_table *** table ){
	int iface;
	int ret = OK;
	uint32_t prefix;
	int prefixLength;
	int sizeHashTable;
	if( ( ret = readFIBLine(&prefix, &prefixLength, &iface) ) == OK ){
	  
		if(prefixLength > 15)
			sizeHashTable=16;
		else
			sizeHashTable=prefixLength;
		ret = put_redirect(iface, prefix, prefixLength, hash((prefix & getNetmask(prefixLength)), sizeHashTable), table);
		return OK;
	}else{
		if(ret == REACHED_EOF)
			return REACHED_EOF;
		return MEMORY_ALLOCATED_ERROR;
	}
//	return /*REACHED_EOF;*/ret;
}

int put_redirect(int iface, uint32_t prefix, int prefixLength, int hashed, struct hash_table *** table){
	struct redirect *aux;
	if((aux=(*table)[prefixLength][hashed].first) == NULL){
 		if( NULL == ((*table)[prefixLength][hashed].first = malloc( sizeof(struct redirect) ))){
 			printf("There was an error allocating memory for the %d interface and %d IP address, aborting", iface, prefix);
 			return MEMORY_ALLOCATED_ERROR;
 		}
 		(*table)[prefixLength][hashed].first->IPAddress = (uint32_t *)malloc(sizeof(uint32_t));
		*(*table)[prefixLength][hashed].first->IPAddress = prefix;
 		(*table)[prefixLength][hashed].first->iface = (int *)malloc(sizeof(int));
		*(*table)[prefixLength][hashed].first->iface = iface;
		
				printf("iface %d prefix %d\n",*(*table)[prefixLength][hashed].first->iface,*(*table)[prefixLength][hashed].first->IPAddress );
	}else{
		while(aux->next != NULL)
			aux=aux->next;
		if( NULL == (aux = malloc( sizeof(struct redirect) ))){
			printf("There was an error allocating memory for the %d interface and %d IP address, aborting", iface, prefix);
			return MEMORY_ALLOCATED_ERROR;
		}
		(*table)[prefixLength][hashed].first->IPAddress= (uint32_t *)malloc(sizeof(uint32_t));
		*(*table)[prefixLength][hashed].first->IPAddress = prefix;
 		(*table)[prefixLength][hashed].first->iface = (int *)malloc(sizeof(int));
		*(*table)[prefixLength][hashed].first->iface = iface;
	}
	return OK;
}

int search(uint32_t IPaddress, struct hash_table ** table, int *hash_lookup){
	int i,  hashed, ret;
	uint32_t IPprefix;
	for (i=32;i>=0;i--){
		IPprefix = IPaddress & getNetmask(i);
		if(i>16)
			hashed=hash(IPprefix, 16);
		else
			hashed=hash(IPprefix, i);

		if( ( ret=search_redirect(IPprefix, table[i][hashed].first) ) != -1 ){
			*hash_lookup = 33-i+1;
			return ret;
		}
	}
	return ADDRESS_COULDNT_RESOLVE;
}

int search_redirect(uint32_t IPaddress, struct redirect *first){
	if(first != NULL){
		while( (first->next) != NULL && *(first->IPAddress) != IPaddress )
			first = first->next;
 		if( *(first->IPAddress) == IPaddress ){
			return *first->iface;
		}
	}
	return -1;
}
