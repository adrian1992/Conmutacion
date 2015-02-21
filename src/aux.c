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

#include "aux.h"

/********************************************************************
 * Generate a netmask of length prefixLength
 ********************************************************************/
void getNetmask (int prefixLength, int *netmask){

	*netmask = (0xFFFFFFFF << (IP_ADDRESS_LENGTH - prefixLength)); 

}

/********************************************************************
 * Example of a very simple hash function using the modulus operator
 * For more info: https://gist.github.com/cpq/8598442
 ********************************************************************/
int hash(uint32_t *IPAddress, int sizeHashTable){

//	//Map the key (IPAddress) to the appropriate index of the hash table
//  int index = IPAddress % sizeHashTable;
//  return (index);
	
	/*
	 * lookup3 hash function
	 */

	uint16_t a, b, c;
	const uint8_t *k;
	int length=32;
	a = b = c = 0xdeadbeef + (((uint16_t) 32) << 2);
	while (length > 6) {
		a += *(IPAddress++);
		b += *(IPAddress++);
		c += *(IPAddress++);
		mix(a, b, c);
		length -= 6;
	}

	k = (const uint8_t *) IPAddress;
	b += ((uint16_t) k[3]) << 8;
	b += k[2];
	a += ((uint16_t) k[1]) << 8;
	a += k[0];
	final(a, b, c);
	return (c & (sizeHashTable - 1));
}
