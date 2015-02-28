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

#ifndef _AUX
#define _AUX

#include <stdint.h>
#include <stdlib.h>

/********************************************************************
 * Constant definitions 
 ********************************************************************/
#define IP_ADDRESS_LENGTH 32

/********************************************************************
 * Helpfull macros
 ********************************************************************/

#define rot(x,k) (((x)<<(k)) | ((x)>>(16-(k))))

#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  c += b; \
  b -= a;  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; \
  c ^= b; \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; \
}

#define getNetmask(x) 0xFFFFFFFF << (IP_ADDRESS_LENGTH - x)

/********************************************************************
 * Simple lookup3 hash function implementation.
 ********************************************************************/

int hash(uint32_t IPAddress, int sizeHashTable);

#endif
