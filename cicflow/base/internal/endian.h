#ifndef CIC_BASE_INTERNAL_ENDIAN_H_
#define CIC_BASE_INTERNAL_ENDIAN_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cicflow/base/config.h"
#include "cicflow/base/internal/unaligned_access.h"
#include "cicflow/base/port.h"

uint64_t gbswap_64(uint64_t host_int) {
#if CIC_HAVE_BUILTIN(__builtin_bswap64) || defined(__GNUC__)
  return __builtin_bswap64(host_int);
#elif defined(_MSC_VER)
  return _byteswap_uint64(host_int);
#else
  return (((host_int & uint64_t{0xFF}) << 56) | ((host_int & uint64_t{0xFF00}) << 40) |
          ((host_int & uint64_t{0xFF0000}) << 24) | ((host_int & uint64_t{0xFF000000}) << 8) |
          ((host_int & uint64_t{0xFF00000000}) >> 8) |
          ((host_int & uint64_t{0xFF0000000000}) >> 24) |
          ((host_int & uint64_t{0xFF000000000000}) >> 40) |
          ((host_int & uint64_t{0xFF00000000000000}) >> 56));
#endif
}

uint32_t gbswap_32(uint32_t host_int) {
#if CIC_HAVE_BUILTIN(__builtin_bswap32) || defined(__GNUC__)
  return __builtin_bswap32(host_int);
#elif defined(_MSC_VER)
  return _byteswap_ulong(host_int);
#else
  return (((host_int & uint32_t{0xFF}) << 24) | ((host_int & uint32_t{0xFF00}) << 8) |
          ((host_int & uint32_t{0xFF0000}) >> 8) | ((host_int & uint32_t{0xFF000000}) >> 24));
#endif
}

uint16_t gbswap_16(uint16_t host_int) {
#if CIC_HAVE_BUILTIN(__builtin_bswap16) || defined(__GNUC__)
  return __builtin_bswap16(host_int);
#elif defined(_MSC_VER)
  return _byteswap_ushort(host_int);
#else
  return (((host_int & uint16_t{0xFF}) << 8) | ((host_int & uint16_t{0xFF00}) >> 8));
#endif
}

#ifdef CIC_IS_LITTLE_ENDIAN

uint16_t ghtons(uint16_t x) { return gbswap_16(x); }
uint32_t ghtonl(uint32_t x) { return gbswap_32(x); }
uint64_t ghtonll(uint64_t x) { return gbswap_64(x); }

#elif defined CIC_IS_BIG_ENDIAN

uint16_t ghtons(uint16_t x) { return x; }
uint32_t ghtonl(uint32_t x) { return x; }
uint64_t ghtonll(uint64_t x) { return x; }

#else
#error \
    "Unsupported byte order: Either CIC_IS_BIG_ENDIAN or " \
       "CIC_IS_LITTLE_ENDIAN must be defined"
#endif  // byte order

uint16_t gntohs(uint16_t x) { return ghtons(x); }
uint32_t gntohl(uint32_t x) { return ghtonl(x); }
uint64_t gntohll(uint64_t x) { return ghtonll(x); }

// namespace LITTLE_ENDIAN
#ifdef CIC_IS_LITTLE_ENDIAN

uint16_t little_endian_from_host16(uint16_t x) { return x; }
uint16_t little_endian_to_host16(uint16_t x) { return x; }

uint32_t little_endian_from_host32(uint32_t x) { return x; }
uint32_t little_endian_to_host32(uint32_t x) { return x; }

uint64_t little_endian_from_host64(uint64_t x) { return x; }
uint64_t little_endian_to_host64(uint64_t x) { return x; }

const bool little_endian_is_little_endian() { return true; }

#elif defined CIC_IS_BIG_ENDIAN

uint16_t little_endian_from_host16(uint16_t x) { return gbswap_16(x); }
uint16_t little_endian_to_host16(uint16_t x) { return gbswap_16(x); }

uint32_t little_endian_from_host32(uint32_t x) { return gbswap_32(x); }
uint32_t little_endian_to_host32(uint32_t x) { return gbswap_32(x); }

uint64_t little_endian_from_host64(uint64_t x) { return gbswap_64(x); }
uint64_t little_endian_to_host64(uint64_t x) { return gbswap_64(x); }

const bool little_endian_is_little_endian() { return false; }

#endif /* ENDIAN */

uint16_t little_endian_load16(const void *p) {
  return little_endian_to_host16(unaligned_load16(p));
}

void little_endian_store16(void *p, uint16_t v) {
  unaligned_store16(p, little_endian_from_host16(v));
}

uint32_t little_endian_load32(const void *p) {
  return little_endian_to_host32(unaligned_load32(p));
}

void little_endian_store32(void *p, uint32_t v) {
  unaligned_store32(p, little_endian_from_host32(v));
}

uint64_t little_endian_load64(const void *p) {
  return little_endian_to_host64(unaligned_load64(p));
}

void little_endian_store64(void *p, uint64_t v) {
  unaligned_store64(p, little_endian_from_host64(v));
}
// namespace LITTLE_ENDIAN

// namespace BIG_ENDIAN
#ifdef CIC_IS_LITTLE_ENDIAN

uint16_t big_endian_from_host16(uint16_t x) { return gbswap_16(x); }
uint16_t big_endian_to_host16(uint16_t x) { return gbswap_16(x); }

uint32_t big_endian_from_host32(uint32_t x) { return gbswap_32(x); }
uint32_t big_endian_to_host32(uint32_t x) { return gbswap_32(x); }

uint64_t big_endian_from_host64(uint64_t x) { return gbswap_64(x); }
uint64_t big_endian_to_host64(uint64_t x) { return gbswap_64(x); }

bool big_endian_is_little_endian() { return true; }

#elif defined CIC_IS_BIG_ENDIAN

uint16_t big_endian_from_host16(uint16_t x) { return x; }
uint16_t big_endian_to_host16(uint16_t x) { return x; }

uint32_t big_endian_from_host32(uint32_t x) { return x; }
uint32_t big_endian_to_host32(uint32_t x) { return x; }

uint64_t big_endian_from_host64(uint64_t x) { return x; }
uint64_t big_endian_to_host64(uint64_t x) { return x; }

bool big_endian_is_little_endian() { return false; }

#endif /* ENDIAN */

uint16_t big_endian_load16(const void *p) { return big_endian_to_host16(unaligned_load16(p)); }

void big_endian_store16(void *p, uint16_t v) { unaligned_store16(p, big_endian_from_host16(v)); }

uint32_t big_endian_load32(const void *p) { return big_endian_to_host32(unaligned_load32(p)); }

void big_endian_store32(void *p, uint32_t v) { unaligned_store32(p, big_endian_from_host32(v)); }

uint64_t big_endian_load64(const void *p) { return big_endian_to_host64(unaligned_load64(p)); }

void big_endian_store64(void *p, uint64_t v) { unaligned_store64(p, big_endian_from_host64(v)); }
// namespace BIG_ENDIAN

#endif
