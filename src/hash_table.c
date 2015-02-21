#include "hash_table.h"

/*
 * Prototypes
 */

void free_redirect(struct hash_table * table);

/*
 * Function definition
 */

int create_table(struct hash_table ** table){
	int i, stable = 1<<16;
	if( NULL == (table = malloc( 32, sizeof(struct hash_table *) ) ) ){
		printf("There was an error allocating memory for the main table, aborting");
		return MEMORY_ALLOCATED_ERROR;
	}
	for(i=0;i<16;i++){
		if( NULL == (table[i]=calloc( 1<<i+1, sizeof(struct hash_table) ))){
			printf("There was an error allocating memory for the %d hash table, aborting", i+1);
			return MEMORY_ALLOCATED_ERROR;
		}
	}
	for(i=16;i<32;i++){
		if( NULL == (table[i]=calloc( stable, sizeof(struct hash_table) ))){
			printf("There was an error allocating memory for the %d hash table, aborting", i+1);
			return MEMORY_ALLOCATED_ERROR;
		}
	}
	return OK;
}

void free_table(struct hash_table ** table){
	int i;
	for (i=0;i<32;i++){
		free_redirect(table[i]);
		free(table[i]->first);
		free(table[i]);
	}
	free(table);
}

void free_redirect(struct hash_table * table){
	struct redirect * aux, * aux2;
	if((aux=table.first) == NULL)
		return;
	while((aux2=aux->next)!=NULL){
		free(aux->IPAddress);
		free(aux->iface);
		free(aux);
		aux=aux2;
	}
	free(aux);
}

int put( struct hash_table ** table){
	int *iface, ret = OK;
	uint32_t *prefix;
	int *prefixLength;
	int sizeHashTable;
	if( readFIBLine(prefix, prefixLength, iface) == OK ){
		if(*prefixLength > 16)
			sizeHashTable=16;
		else
			sizeHashTable=*prefixLength;
		ret = put(iface, prefix, *prefixLength, hash(*prefix, sizeHashTable), table);
	}
	free(prefixLength);
	return ret;
}

int put(int * iface, uint32_t *prefix, int prefixLength, int hash, struct hash_table ** table){
	struct redirect *aux;
	if((aux=table[prefixLength][hash].first) == NULL){
		if( NULL == (table[prefixLength][hash].first = malloc( sizeof(struct redirect) ))){
			printf("There was an error allocating memory for the %d interface and %d IP address, aborting", *iface, *prefix);
			return MEMORY_ALLOCATED_ERROR;
		}
		table[prefixLength][hash].first->IPAddress=prefix;
		table[prefixLength][hash].first->iface=iface;
	}else{
		while(aux->next != NULL)
			aux=aux.next;
		if( NULL == (aux = malloc( sizeof(struct redirect) ))){
			printf("There was an error allocating memory for the %d interface and %d IP address, aborting", *iface, *prefix);
			return MEMORY_ALLOCATED_ERROR;
		}
		aux->IPAddress=prefix;
		aux->iface=iface;
	}
	return OK;
}





