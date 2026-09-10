/*
 * rapidhash V3 - Very fast, high quality, platform-independent hashing
 * algorithm.
 *
 * Based on 'wyhash', by Wang Yi <godspeed_china@yeah.net>
 *
 * Copyright (C) 2025 Nicolas De Carli
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * You can contact the author at:
 *   - rapidhash source repository: https://github.com/Nicoshev/rapidhash
 */

#ifndef NAO_CLIHH_EXTRAS_NAO_RAPIDHASH_H
#define NAO_CLIHH_EXTRAS_NAO_RAPIDHASH_H

/*
 *  Includes.
 */
#include <stdint.h>
#include <string.h>
#if defined(_MSC_VER)
#  include <intrin.h>
#  if defined(_M_X64) && !defined(_M_ARM64EC)
#    pragma intrinsic(_umul128)
#  endif
#endif

/*
 *  C/C++ macros.
 */

#ifdef _MSC_VER
#  define NAO_RAPIDHASH_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
#  define NAO_RAPIDHASH_ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
#  define NAO_RAPIDHASH_ALWAYS_INLINE inline
#endif

#ifdef __cplusplus
#  define NAO_RAPIDHASH_NOEXCEPT noexcept
#  define NAO_RAPIDHASH_CONSTEXPR constexpr
#  ifndef NAO_RAPIDHASH_INLINE
#    define NAO_RAPIDHASH_INLINE NAO_RAPIDHASH_ALWAYS_INLINE
#  endif
#  if __cplusplus >= 201402L && !defined(_MSC_VER)
#    define NAO_RAPIDHASH_INLINE_CONSTEXPR NAO_RAPIDHASH_ALWAYS_INLINE constexpr
#  else
#    define NAO_RAPIDHASH_INLINE_CONSTEXPR NAO_RAPIDHASH_ALWAYS_INLINE
#  endif
#else
#  define NAO_RAPIDHASH_NOEXCEPT
#  define NAO_RAPIDHASH_CONSTEXPR static const
#  ifndef NAO_RAPIDHASH_INLINE
#    define NAO_RAPIDHASH_INLINE static NAO_RAPIDHASH_ALWAYS_INLINE
#  endif
#  define NAO_RAPIDHASH_INLINE_CONSTEXPR NAO_RAPIDHASH_INLINE
#endif

/*
 *  Unrolled macro.
 *  Improves large input speed, but increases code size and worsens small input
 * speed.
 *
 *  NAO_RAPIDHASH_COMPACT: Normal behavior.
 *  NAO_RAPIDHASH_UNROLLED:
 *
 */
#ifndef NAO_RAPIDHASH_UNROLLED
#  define NAO_RAPIDHASH_COMPACT
#elif defined(NAO_RAPIDHASH_COMPACT)
#  error                                                                       \
      "cannot define NAO_RAPIDHASH_COMPACT and NAO_RAPIDHASH_UNROLLED simultaneously."
#endif

/*
 *  Protection macro, alters behaviour of rapid_mum multiplication function.
 *
 *  NAO_RAPIDHASH_FAST: Normal behavior, max speed.
 *  NAO_RAPIDHASH_PROTECTED: Extra protection against entropy loss.
 */
#ifndef NAO_RAPIDHASH_PROTECTED
#  define NAO_RAPIDHASH_FAST
#elif defined(NAO_RAPIDHASH_FAST)
#  error                                                                       \
      "cannot define NAO_RAPIDHASH_PROTECTED and NAO_RAPIDHASH_FAST simultaneously."
#endif

/*
 *  Likely and unlikely macros.
 */
#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
#  define _likely_(x) __builtin_expect(x, 1)
#  define _unlikely_(x) __builtin_expect(x, 0)
#else
#  define _likely_(x) (x)
#  define _unlikely_(x) (x)
#endif

/*
 *  Endianness macros.
 */
#ifndef NAO_RAPIDHASH_LITTLE_ENDIAN
#  if defined(_WIN32) || defined(__LITTLE_ENDIAN__) ||                         \
      (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#    define NAO_RAPIDHASH_LITTLE_ENDIAN
#  elif defined(__BIG_ENDIAN__) ||                                             \
      (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#    define NAO_RAPIDHASH_BIG_ENDIAN
#  else
#    warning "could not determine endianness! Falling back to little endian."
#    define NAO_RAPIDHASH_LITTLE_ENDIAN
#  endif
#endif

/*
 *  Default secret parameters.
 */
NAO_RAPIDHASH_CONSTEXPR uint64_t rapid_secret[8] = {
    0x2d358dccaa6c78a5ull, 0x8bb84b93962eacc9ull, 0x4b33a62ed433d4a3ull,
    0x4d5a2da51de1aa47ull, 0xa0761d6478bd642full, 0xe7037ed1a0b428dbull,
    0x90ed1765281c388cull, 0xaaaaaaaaaaaaaaaaull};

/*
 *  64*64 -> 128bit multiply function.
 *
 *  @param A  Address of 64-bit number.
 *  @param B  Address of 64-bit number.
 *
 *  Calculates 128-bit C = *A * *B.
 *
 *  When NAO_RAPIDHASH_FAST is defined:
 *  Overwrites A contents with C's low 64 bits.
 *  Overwrites B contents with C's high 64 bits.
 *
 *  When NAO_RAPIDHASH_PROTECTED is defined:
 *  Xors and overwrites A contents with C's low 64 bits.
 *  Xors and overwrites B contents with C's high 64 bits.
 */
NAO_RAPIDHASH_INLINE_CONSTEXPR void
nao_rapid_mum(uint64_t *A, uint64_t *B) NAO_RAPIDHASH_NOEXCEPT {
#if defined(__SIZEOF_INT128__)
  __uint128_t r = *A;
  r *= *B;
#  ifdef NAO_RAPIDHASH_PROTECTED
  *A ^= (uint64_t)r;
  *B ^= (uint64_t)(r >> 64);
#  else
  *A = (uint64_t)r;
  *B = (uint64_t)(r >> 64);
#  endif
#elif defined(_MSC_VER) && (defined(_WIN64) || defined(_M_HYBRID_CHPE_ARM64))
#  if defined(_M_X64)
#    ifdef NAO_RAPIDHASH_PROTECTED
  uint64_t a, b;
  a = _umul128(*A, *B, &b);
  *A ^= a;
  *B ^= b;
#    else
  *A = _umul128(*A, *B, B);
#    endif
#  else
#    ifdef NAO_RAPIDHASH_PROTECTED
  uint64_t a, b;
  b = __umulh(*A, *B);
  a = *A * *B;
  *A ^= a;
  *B ^= b;
#    else
  uint64_t c = __umulh(*A, *B);
  *A = *A * *B;
  *B = c;
#    endif
#  endif
#else
  uint64_t ha = *A >> 32, hb = *B >> 32, la = (uint32_t)*A, lb = (uint32_t)*B;
  uint64_t rh = ha * hb, rm0 = ha * lb, rm1 = hb * la, rl = la * lb,
           t = rl + (rm0 << 32), c = t < rl;
  uint64_t lo = t + (rm1 << 32);
  c += lo < t;
  uint64_t hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;
#  ifdef NAO_RAPIDHASH_PROTECTED
  *A ^= lo;
  *B ^= hi;
#  else
  *A = lo;
  *B = hi;
#  endif
#endif
}

/*
 *  Multiply and xor mix function.
 *
 *  @param A  64-bit number.
 *  @param B  64-bit number.
 *
 *  Calculates 128-bit C = A * B.
 *  Returns 64-bit xor between high and low 64 bits of C.
 */
NAO_RAPIDHASH_INLINE_CONSTEXPR uint64_t nao_rapid_mix(uint64_t A, uint64_t B)
    NAO_RAPIDHASH_NOEXCEPT {
  nao_rapid_mum(&A, &B);
  return A ^ B;
}

/*
 *  Read functions.
 */
#ifdef NAO_RAPIDHASH_LITTLE_ENDIAN
NAO_RAPIDHASH_INLINE
uint64_t nao_rapid_read64(const uint8_t *p) NAO_RAPIDHASH_NOEXCEPT {
  uint64_t v;
  memcpy(&v, p, sizeof(uint64_t));
  return v;
}
NAO_RAPIDHASH_INLINE uint64_t nao_rapid_read32(const uint8_t *p)
    NAO_RAPIDHASH_NOEXCEPT {
  uint32_t v;
  memcpy(&v, p, sizeof(uint32_t));
  return v;
}
#elif defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
NAO_RAPIDHASH_INLINE
uint64_t nao_rapid_read64(const uint8_t *p) NAO_RAPIDHASH_NOEXCEPT {
  uint64_t v;
  memcpy(&v, p, sizeof(uint64_t));
  return __builtin_bswap64(v);
}
NAO_RAPIDHASH_INLINE uint64_t nao_rapid_read32(const uint8_t *p)
    NAO_RAPIDHASH_NOEXCEPT {
  uint32_t v;
  memcpy(&v, p, sizeof(uint32_t));
  return __builtin_bswap32(v);
}
#elif defined(_MSC_VER)
NAO_RAPIDHASH_INLINE
uint64_t nao_rapid_read64(const uint8_t *p) NAO_RAPIDHASH_NOEXCEPT {
  uint64_t v;
  memcpy(&v, p, sizeof(uint64_t));
  return _byteswap_uint64(v);
}
NAO_RAPIDHASH_INLINE uint64_t nao_rapid_read32(const uint8_t *p)
    NAO_RAPIDHASH_NOEXCEPT {
  uint32_t v;
  memcpy(&v, p, sizeof(uint32_t));
  return _byteswap_ulong(v);
}
#else
NAO_RAPIDHASH_INLINE
uint64_t nao_rapid_read64(const uint8_t *p) NAO_RAPIDHASH_NOEXCEPT {
  uint64_t v;
  memcpy(&v, p, 8);
  return (((v >> 56) & 0xff) | ((v >> 40) & 0xff00) | ((v >> 24) & 0xff0000) |
          ((v >> 8) & 0xff000000) | ((v << 8) & 0xff00000000) |
          ((v << 24) & 0xff0000000000) | ((v << 40) & 0xff000000000000) |
          ((v << 56) & 0xff00000000000000));
}
NAO_RAPIDHASH_INLINE uint64_t nao_rapid_read32(const uint8_t *p)
    NAO_RAPIDHASH_NOEXCEPT {
  uint32_t v;
  memcpy(&v, p, 4);
  return (((v >> 24) & 0xff) | ((v >> 8) & 0xff00) | ((v << 8) & 0xff0000) |
          ((v << 24) & 0xff000000));
}
#endif

/*
 *  rapidhash main function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *  @param seed    64-bit seed used to alter the hash result predictably.
 *  @param secret  Triplet of 64-bit secrets used to alter hash result
 * predictably.
 *
 *  Returns a 64-bit hash.
 */
NAO_RAPIDHASH_INLINE_CONSTEXPR uint64_t
nao_rapidhash_internal(const void *key, size_t len, uint64_t seed,
                       const uint64_t *secret) NAO_RAPIDHASH_NOEXCEPT {
  const uint8_t *p = (const uint8_t *)key;
  seed ^= nao_rapid_mix(seed ^ secret[2], secret[1]);
  uint64_t a = 0, b = 0;
  size_t i = len;
  if (_likely_(len <= 16)) {
    if (len >= 4) {
      seed ^= len;
      if (len >= 8) {
        const uint8_t *plast = p + len - 8;
        a = nao_rapid_read64(p);
        b = nao_rapid_read64(plast);
      } else {
        const uint8_t *plast = p + len - 4;
        a = nao_rapid_read32(p);
        b = nao_rapid_read32(plast);
      }
    } else if (len > 0) {
      a = (((uint64_t)p[0]) << 45) | p[len - 1];
      b = p[len >> 1];
    } else
      a = b = 0;
  } else {
    if (len > 112) {
      uint64_t see1 = seed, see2 = seed;
      uint64_t see3 = seed, see4 = seed;
      uint64_t see5 = seed, see6 = seed;
#ifdef NAO_RAPIDHASH_COMPACT
      do {
        seed = nao_rapid_mix(nao_rapid_read64(p) ^ secret[0],
                             nao_rapid_read64(p + 8) ^ seed);
        see1 = nao_rapid_mix(nao_rapid_read64(p + 16) ^ secret[1],
                             nao_rapid_read64(p + 24) ^ see1);
        see2 = nao_rapid_mix(nao_rapid_read64(p + 32) ^ secret[2],
                             nao_rapid_read64(p + 40) ^ see2);
        see3 = nao_rapid_mix(nao_rapid_read64(p + 48) ^ secret[3],
                             nao_rapid_read64(p + 56) ^ see3);
        see4 = nao_rapid_mix(nao_rapid_read64(p + 64) ^ secret[4],
                             nao_rapid_read64(p + 72) ^ see4);
        see5 = nao_rapid_mix(nao_rapid_read64(p + 80) ^ secret[5],
                             nao_rapid_read64(p + 88) ^ see5);
        see6 = nao_rapid_mix(nao_rapid_read64(p + 96) ^ secret[6],
                             nao_rapid_read64(p + 104) ^ see6);
        p += 112;
        i -= 112;
      } while (i > 112);
#else
      while (i > 224) {
        seed =
            rapid_mix(rapid_read64(p) ^ secret[0], rapid_read64(p + 8) ^ seed);
        see1 = rapid_mix(rapid_read64(p + 16) ^ secret[1],
                         rapid_read64(p + 24) ^ see1);
        see2 = rapid_mix(rapid_read64(p + 32) ^ secret[2],
                         rapid_read64(p + 40) ^ see2);
        see3 = rapid_mix(rapid_read64(p + 48) ^ secret[3],
                         rapid_read64(p + 56) ^ see3);
        see4 = rapid_mix(rapid_read64(p + 64) ^ secret[4],
                         rapid_read64(p + 72) ^ see4);
        see5 = rapid_mix(rapid_read64(p + 80) ^ secret[5],
                         rapid_read64(p + 88) ^ see5);
        see6 = rapid_mix(rapid_read64(p + 96) ^ secret[6],
                         rapid_read64(p + 104) ^ see6);
        seed = rapid_mix(rapid_read64(p + 112) ^ secret[0],
                         rapid_read64(p + 120) ^ seed);
        see1 = rapid_mix(rapid_read64(p + 128) ^ secret[1],
                         rapid_read64(p + 136) ^ see1);
        see2 = rapid_mix(rapid_read64(p + 144) ^ secret[2],
                         rapid_read64(p + 152) ^ see2);
        see3 = rapid_mix(rapid_read64(p + 160) ^ secret[3],
                         rapid_read64(p + 168) ^ see3);
        see4 = rapid_mix(rapid_read64(p + 176) ^ secret[4],
                         rapid_read64(p + 184) ^ see4);
        see5 = rapid_mix(rapid_read64(p + 192) ^ secret[5],
                         rapid_read64(p + 200) ^ see5);
        see6 = rapid_mix(rapid_read64(p + 208) ^ secret[6],
                         rapid_read64(p + 216) ^ see6);
        p += 224;
        i -= 224;
      }
      if (i > 112) {
        seed =
            rapid_mix(rapid_read64(p) ^ secret[0], rapid_read64(p + 8) ^ seed);
        see1 = rapid_mix(rapid_read64(p + 16) ^ secret[1],
                         rapid_read64(p + 24) ^ see1);
        see2 = rapid_mix(rapid_read64(p + 32) ^ secret[2],
                         rapid_read64(p + 40) ^ see2);
        see3 = rapid_mix(rapid_read64(p + 48) ^ secret[3],
                         rapid_read64(p + 56) ^ see3);
        see4 = rapid_mix(rapid_read64(p + 64) ^ secret[4],
                         rapid_read64(p + 72) ^ see4);
        see5 = rapid_mix(rapid_read64(p + 80) ^ secret[5],
                         rapid_read64(p + 88) ^ see5);
        see6 = rapid_mix(rapid_read64(p + 96) ^ secret[6],
                         rapid_read64(p + 104) ^ see6);
        p += 112;
        i -= 112;
      }
#endif
      seed ^= see1;
      see2 ^= see3;
      see4 ^= see5;
      seed ^= see6;
      see2 ^= see4;
      seed ^= see2;
    }
    if (i > 16) {
      seed = nao_rapid_mix(nao_rapid_read64(p) ^ secret[2],
                           nao_rapid_read64(p + 8) ^ seed);
      if (i > 32) {
        seed = nao_rapid_mix(nao_rapid_read64(p + 16) ^ secret[2],
                             nao_rapid_read64(p + 24) ^ seed);
        if (i > 48) {
          seed = nao_rapid_mix(nao_rapid_read64(p + 32) ^ secret[1],
                               nao_rapid_read64(p + 40) ^ seed);
          if (i > 64) {
            seed = nao_rapid_mix(nao_rapid_read64(p + 48) ^ secret[1],
                                 nao_rapid_read64(p + 56) ^ seed);
            if (i > 80) {
              seed = nao_rapid_mix(nao_rapid_read64(p + 64) ^ secret[2],
                                   nao_rapid_read64(p + 72) ^ seed);
              if (i > 96) {
                seed = nao_rapid_mix(nao_rapid_read64(p + 80) ^ secret[1],
                                     nao_rapid_read64(p + 88) ^ seed);
              }
            }
          }
        }
      }
    }
    a = nao_rapid_read64(p + i - 16) ^ i;
    b = nao_rapid_read64(p + i - 8);
  }
  a ^= secret[1];
  b ^= seed;
  nao_rapid_mum(&a, &b);
  return nao_rapid_mix(a ^ secret[7], b ^ secret[1] ^ i);
}

/*
 *  rapidhashMicro main function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *  @param seed    64-bit seed used to alter the hash result predictably.
 *  @param secret  Triplet of 64-bit secrets used to alter hash result
 * predictably.
 *
 *  Returns a 64-bit hash.
 */
NAO_RAPIDHASH_INLINE_CONSTEXPR uint64_t
nao_rapidhashMicro_internal(const void *key, size_t len, uint64_t seed,
                            const uint64_t *secret) NAO_RAPIDHASH_NOEXCEPT {
  const uint8_t *p = (const uint8_t *)key;
  seed ^= nao_rapid_mix(seed ^ secret[2], secret[1]);
  uint64_t a = 0, b = 0;
  size_t i = len;
  if (_likely_(len <= 16)) {
    if (len >= 4) {
      seed ^= len;
      if (len >= 8) {
        const uint8_t *plast = p + len - 8;
        a = nao_rapid_read64(p);
        b = nao_rapid_read64(plast);
      } else {
        const uint8_t *plast = p + len - 4;
        a = nao_rapid_read32(p);
        b = nao_rapid_read32(plast);
      }
    } else if (len > 0) {
      a = (((uint64_t)p[0]) << 45) | p[len - 1];
      b = p[len >> 1];
    } else
      a = b = 0;
  } else {
    if (i > 80) {
      uint64_t see1 = seed, see2 = seed;
      uint64_t see3 = seed, see4 = seed;
      do {
        seed = nao_rapid_mix(nao_rapid_read64(p) ^ secret[0],
                             nao_rapid_read64(p + 8) ^ seed);
        see1 = nao_rapid_mix(nao_rapid_read64(p + 16) ^ secret[1],
                             nao_rapid_read64(p + 24) ^ see1);
        see2 = nao_rapid_mix(nao_rapid_read64(p + 32) ^ secret[2],
                             nao_rapid_read64(p + 40) ^ see2);
        see3 = nao_rapid_mix(nao_rapid_read64(p + 48) ^ secret[3],
                             nao_rapid_read64(p + 56) ^ see3);
        see4 = nao_rapid_mix(nao_rapid_read64(p + 64) ^ secret[4],
                             nao_rapid_read64(p + 72) ^ see4);
        p += 80;
        i -= 80;
      } while (i > 80);
      seed ^= see1;
      see2 ^= see3;
      seed ^= see4;
      seed ^= see2;
    }
    if (i > 16) {
      seed = nao_rapid_mix(nao_rapid_read64(p) ^ secret[2],
                           nao_rapid_read64(p + 8) ^ seed);
      if (i > 32) {
        seed = nao_rapid_mix(nao_rapid_read64(p + 16) ^ secret[2],
                             nao_rapid_read64(p + 24) ^ seed);
        if (i > 48) {
          seed = nao_rapid_mix(nao_rapid_read64(p + 32) ^ secret[1],
                               nao_rapid_read64(p + 40) ^ seed);
          if (i > 64) {
            seed = nao_rapid_mix(nao_rapid_read64(p + 48) ^ secret[1],
                                 nao_rapid_read64(p + 56) ^ seed);
          }
        }
      }
    }
    a = nao_rapid_read64(p + i - 16) ^ i;
    b = nao_rapid_read64(p + i - 8);
  }
  a ^= secret[1];
  b ^= seed;
  nao_rapid_mum(&a, &b);
  return nao_rapid_mix(a ^ secret[7], b ^ secret[1] ^ i);
}

/*
 *  rapidhashNano main function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *  @param seed    64-bit seed used to alter the hash result predictably.
 *  @param secret  Triplet of 64-bit secrets used to alter hash result
 * predictably.
 *
 *  Returns a 64-bit hash.
 */
NAO_RAPIDHASH_INLINE_CONSTEXPR uint64_t
nao_rapidhashNano_internal(const void *key, size_t len, uint64_t seed,
                           const uint64_t *secret) NAO_RAPIDHASH_NOEXCEPT {
  const uint8_t *p = (const uint8_t *)key;
  seed ^= nao_rapid_mix(seed ^ secret[2], secret[1]);
  uint64_t a = 0, b = 0;
  size_t i = len;
  if (_likely_(len <= 16)) {
    if (len >= 4) {
      seed ^= len;
      if (len >= 8) {
        const uint8_t *plast = p + len - 8;
        a = nao_rapid_read64(p);
        b = nao_rapid_read64(plast);
      } else {
        const uint8_t *plast = p + len - 4;
        a = nao_rapid_read32(p);
        b = nao_rapid_read32(plast);
      }
    } else if (len > 0) {
      a = (((uint64_t)p[0]) << 45) | p[len - 1];
      b = p[len >> 1];
    } else
      a = b = 0;
  } else {
    if (i > 48) {
      uint64_t see1 = seed, see2 = seed;
      do {
        seed = nao_rapid_mix(nao_rapid_read64(p) ^ secret[0],
                             nao_rapid_read64(p + 8) ^ seed);
        see1 = nao_rapid_mix(nao_rapid_read64(p + 16) ^ secret[1],
                             nao_rapid_read64(p + 24) ^ see1);
        see2 = nao_rapid_mix(nao_rapid_read64(p + 32) ^ secret[2],
                             nao_rapid_read64(p + 40) ^ see2);
        p += 48;
        i -= 48;
      } while (i > 48);
      seed ^= see1;
      seed ^= see2;
    }
    if (i > 16) {
      seed = nao_rapid_mix(nao_rapid_read64(p) ^ secret[2],
                           nao_rapid_read64(p + 8) ^ seed);
      if (i > 32) {
        seed = nao_rapid_mix(nao_rapid_read64(p + 16) ^ secret[2],
                             nao_rapid_read64(p + 24) ^ seed);
      }
    }
    a = nao_rapid_read64(p + i - 16) ^ i;
    b = nao_rapid_read64(p + i - 8);
  }
  a ^= secret[1];
  b ^= seed;
  nao_rapid_mum(&a, &b);
  return nao_rapid_mix(a ^ secret[7], b ^ secret[1] ^ i);
}

/*
 *  rapidhash seeded hash function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *  @param seed    64-bit seed used to alter the hash result predictably.
 *
 *  Calls rapidhash_internal using provided parameters and default secrets.
 *
 *  Returns a 64-bit hash.
 */
NAO_RAPIDHASH_INLINE_CONSTEXPR uint64_t nao_rapidhash_withSeed(
    const void *key, size_t len, uint64_t seed) NAO_RAPIDHASH_NOEXCEPT {
  return nao_rapidhash_internal(key, len, seed, rapid_secret);
}

/*
 *  rapidhash general purpose hash function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *
 *  Calls rapidhash_withSeed using provided parameters and the default seed.
 *
 *  Returns a 64-bit hash.
 */
NAO_RAPIDHASH_INLINE_CONSTEXPR uint64_t
nao_rapidhash(const void *key, size_t len) NAO_RAPIDHASH_NOEXCEPT {
  return nao_rapidhash_withSeed(key, len, 0);
}

/*
 *  rapidhashMicro seeded hash function.
 *
 *  Designed for HPC and server applications, where cache misses make a
 * noticeable performance detriment. Clang-18+ compiles it to ~140 instructions
 * without stack usage, both on x86-64 and aarch64. Faster for sizes up to 512
 * bytes, just 15%-20% slower for inputs above 1kb.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *  @param seed    64-bit seed used to alter the hash result predictably.
 *
 *  Calls rapidhash_internal using provided parameters and default secrets.
 *
 *  Returns a 64-bit hash.
 */
NAO_RAPIDHASH_INLINE_CONSTEXPR uint64_t nao_rapidhashMicro_withSeed(
    const void *key, size_t len, uint64_t seed) NAO_RAPIDHASH_NOEXCEPT {
  return nao_rapidhashMicro_internal(key, len, seed, rapid_secret);
}

/*
 *  rapidhashMicro hash function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *
 *  Calls rapidhash_withSeed using provided parameters and the default seed.
 *
 *  Returns a 64-bit hash.
 */
NAO_RAPIDHASH_INLINE_CONSTEXPR uint64_t
nao_rapidhashMicro(const void *key, size_t len) NAO_RAPIDHASH_NOEXCEPT {
  return nao_rapidhashMicro_withSeed(key, len, 0);
}

/*
 *  rapidhashNano seeded hash function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *  @param seed    64-bit seed used to alter the hash result predictably.
 *
 *  Calls rapidhash_internal using provided parameters and default secrets.
 *
 *  Returns a 64-bit hash.
 */
NAO_RAPIDHASH_INLINE_CONSTEXPR uint64_t nao_rapidhashNano_withSeed(
    const void *key, size_t len, uint64_t seed) NAO_RAPIDHASH_NOEXCEPT {
  return nao_rapidhashNano_internal(key, len, seed, rapid_secret);
}

/*
 *  rapidhashNano hash function.
 *
 *  Designed for Mobile and embedded applications, where keeping a small code
 * size is a top priority. Clang-18+ compiles it to less than 100 instructions
 * without stack usage, both on x86-64 and aarch64. The fastest for sizes up to
 * 48 bytes, but may be considerably slower for larger inputs.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *
 *  Calls rapidhash_withSeed using provided parameters and the default seed.
 *
 *  Returns a 64-bit hash.
 */
NAO_RAPIDHASH_INLINE_CONSTEXPR uint64_t
nao_rapidhashNano(const void *key, size_t len) NAO_RAPIDHASH_NOEXCEPT {
  return nao_rapidhashNano_withSeed(key, len, 0);
}

#endif
