#ifndef CIC_BASE_INTERNAL_UNALIGNED_ACCESS_H_
#define CIC_BASE_INTERNAL_UNALIGNED_ACCESS_H_

#include <stdint.h>
#include <string.h>

#include "cicflow/base/attributes.h"
#include "cicflow/base/config.h"

uint16_t unaligned_load16(const void *p) {
  uint16_t t;
  memcpy(&t, p, sizeof(t));
  return t;
}

uint32_t unaligned_load32(const void *p) {
  uint32_t t;
  memcpy(&t, p, sizeof(t));
  return t;
}

uint64_t unaligned_load64(const void *p) {
  uint64_t t;
  memcpy(&t, p, sizeof(t));
  return t;
}

void unaligned_store16(void *p, uint16_t v) { memcpy(p, &v, sizeof(v)); }

void unaligned_store32(void *p, uint32_t v) { memcpy(p, &v, sizeof(v)); }

void unaligned_store64(void *p, uint64_t v) { memcpy(p, &v, sizeof(v)); }

#define CIC_INTERNAL_UNALIGNED_LOAD16(_p) (unaligned_load16(_p))
#define CIC_INTERNAL_UNALIGNED_LOAD32(_p) (unaligned_load32(_p))
#define CIC_INTERNAL_UNALIGNED_LOAD64(_p) (unaligned_load64(_p))

#define CIC_INTERNAL_UNALIGNED_STORE16(_p, _val) (unaligned_store16(_p, _val))
#define CIC_INTERNAL_UNALIGNED_STORE32(_p, _val) (unaligned_store32(_p, _val))
#define CIC_INTERNAL_UNALIGNED_STORE64(_p, _val) (unaligned_store64(_p, _val))

#endif
