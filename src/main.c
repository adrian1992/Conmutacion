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

#include "io.h"
#include "hash_table.h"
#include "aux.h"
#include <time.h>


void print_tree(struct binary_tree * tree){
	if(tree->rigth!=NULL)
		print_tree(tree->rigth);
	printf("tree prefix %d\n", tree->prefix);
	if(tree->left!=NULL)
		print_tree(tree->left);
}

int initialize(struct binary_tree ** tree){
	int error = MEMORY_ALLOCATED_ERROR;	
// Started building file system
	if( ( error = create_tree(tree) ) == OK ){
		while( ( error = put( tree ) ) == OK);
		if(error == REACHED_EOF)
			return OK;
	}else{
		printErrorExplanation(error);
		return error;
	}
// Finished building file system	
	return error;
}

int main(int n_args, char *args[]){
	int error,busqueda;
	int hash_lookup = 0;
	uint32_t ipAddress;
	struct timeval tic, toc;
	double tac;
	if(n_args<4){
		if( ( error = initializeIO(args[1], args[2]) ) == OK ){
			// Initialize
			printf("inicializado");
			struct binary_tree * tree = NULL;
			if((error = initialize( &tree )) == OK ){
				print_tree(tree);
				// Search and route
				while(readInputPacketFileLine(&ipAddress)!=REACHED_EOF){
					gettimeofday (&tic, NULL);
					if((busqueda=search(ipAddress, tree, &hash_lookup)) != ADDRESS_COULDNT_RESOLVE){
						gettimeofday (&toc, NULL);
						tac = toc.tv_usec - tic.tv_usec;
						printf("ip %d, busqueda %d,\n",ipAddress, busqueda);
						printOutputLine(ipAddress, busqueda, &tic, &toc, &tac, hash_lookup);
					}
				}
				// End search and route
				/*freeIO();
				free_tree( tree);*/
			}else{
				/*freeIO();
				printErrorExplanation(error);
				return error;*/
			}
			// End initialize
		}else{
			printErrorExplanation(error);
			return error;
		}
	}
	return 0;
	
}

