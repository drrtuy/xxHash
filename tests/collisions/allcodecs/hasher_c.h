/* Copyright (C) 2014 InfiniDB, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 2 of
   the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA. */

/******************************************************************************
 * $Id: hasher.h 3843 2013-05-31 13:46:24Z pleblanc $
 *
 *****************************************************************************/

/** @file
 * class Hasher interface
 */

#ifndef UTILS_HASHER_H
#define UTILS_HASHER_H

inline uint32_t rotl32(uint32_t x, int8_t r)
{
    return (x << r) | (x >> (32 - r));
}

inline uint32_t mm3_fmix(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}
/*
inline uint64_t mm3_fmix(uint64_t k)
{
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdULL;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53ULL;
    k ^= k >> 33;

    return k;
}

inline uint64_t rotl64(uint64_t x, int8_t r)
{
    return (x << r) | (x >> (64 - r));
}
*/
uint64_t mm3_hash(const char* data, uint64_t len, uint32_t seed)
{
    const int nblocks = len / 4;

    uint32_t h1 = seed;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    //----------
    // body

    const uint32_t* blocks = (const uint32_t*) (data + nblocks * 4);

    for (int i = -nblocks; i; i++)
    {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = rotl32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = rotl32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    //----------
    // tail

    const uint8_t* tail = (const uint8_t*) (data + nblocks * 4);

    uint32_t k1 = 0;

    switch (len & 3)
    {
        case 3:
            k1 ^= tail[2] << 16;
    /* FALLTHRU */

        case 2:
            k1 ^= tail[1] << 8;
    /* FALLTHRU */

        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = rotl32(k1, 15);
            k1 *= c2;
            h1 ^= k1;
    };

    return (uint64_t)h1;
}

uint32_t finalize(uint32_t seed, uint32_t len)
{
    seed ^= len;
    seed = mm3_fmix(seed);
    return seed;
}
#define MY_HASH_ADD(A, B, value) \
    do { A^= (((A & 63)+B)*((value)))+ (A << 8); B+=3; } while(0)

size_t my_hash_sort_bin(const unsigned char *key, size_t len, uint64_t nr1, uint64_t nr2)
{
  const unsigned char *end = key + len;
  uint64_t tmp1= nr1;
  uint64_t tmp2= nr2;

  for (; key < end ; key++)
  {
    MY_HASH_ADD(tmp1, tmp2, (uint32_t) *key);
  }
 
   return tmp1;
}


#endif  // UTILS_HASHER_H
