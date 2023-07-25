#ifndef CIC_BASE_INTERNAL_HIDE_PTR_H_
#define CIC_BASE_INTERNAL_HIDE_PTR_H_

#include <stdint.h>

uintptr_t hide_mask() { return ((uintptr_t)0xF03A5F7BU) << (sizeof(uintptr_t) - 4) * 8 | 0xF03A5F7BU; }

uintptr_t hide_ptr(void* ptr) { return (uintptr_t)(ptr) ^ hide_mask(); }
void* unhide_ptr(uintptr_t hidden) { return (void *)(hidden ^ hide_mask()); }

#endif
