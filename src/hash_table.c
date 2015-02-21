#include "hash_table.h"

/*
 * Prototypes
 */

void free_redirect(struct hash_table * table);

/*
 * Function definition
 */

struct hash_table ** create_table(){
	struct hash_table **ret = NULL;
	int i, stable = 1<<16;
	if( NULL == (ret = calloc( 32, sizeof(struct hash_table *) ) ) ){
		printf("There was an error allocating memory for the main table, aborting");
		return NULL;
	}
	for(i=0;i<16;i++){
		if( NULL == (ret[i]=calloc( 1<<i+1, sizeof(struct hash_table) ))){
			printf("There was an error allocating memory for the %d hash table, aborting", i+1);
			return NULL;
		}
	}
	for(i=16;i<32;i++){
		if( NULL == (ret[i]=calloc( stable, sizeof(struct hash_table) ))){
			printf("There was an error allocating memory for the %d hash table, aborting", i+1);
			return NULL;
		}
	}
	return ret;
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
		free(aux);
		aux=aux2;
	}
	free(aux);
}





