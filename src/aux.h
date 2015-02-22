#include <stdint.h>
#include <stdlib.h>

/********************************************************************
 * Constant definitions 
 ********************************************************************/
#define IP_ADDRESS_LENGTH 32

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

/********************************************************************
 * Generate a netmask of length prefixLength
 ********************************************************************/
int getNetmask (int prefixLength);

/********************************************************************
 * Example of a very simple hash function using the modulus operator
 * For more info: https://gist.github.com/cpq/8598442
 ********************************************************************/
int hash(uint32_t IPAddress, int sizeHashTable);

