#include "cicflow/base/internal/low_level_alloc.h"

#include "cicflow/base/attributes.h"
#include "cicflow/base/call_once.h"
#include "cicflow/base/config.h"
#include "cicflow/base/internal/direct_mmap.h"
#include "cicflow/base/internal/scheduling_mode.h"
#include "cicflow/base/macros.h"
#include "cicflow/base/thread_annotations.h"

#ifndef CIC_LOW_LEVEL_ALLOC_MISSING

#ifndef _WIN32
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

#ifdef __linux__
#include <sys/prctl.h>
#endif

#include <string.h>

// #include <algorithm>
#include <errno.h>
#include <stdatomic.h>
#include <stddef.h>
// #include <new>  // for placement-new
//
#include "cicflow/base/dynamic_annotations.h"
#include "cicflow/base/internal/raw_logging.h"
#include "cicflow/base/internal/spinlock.h"

#if defined(__APPLE__)
#if !defined MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif  // !MAP_ANONYMOUS
#endif  // __APPLE__

typedef struct HEADER_ {
  uintptr_t size;
  uintptr_t magic;
  struct ARENA_ *arena;
  void *dummy_for_alignment;
} HEADER;

typedef struct ALLOC_LIST_ {
  int levels;
  struct HEADER_ header;
  struct ALLOC_LIST_ *next[MAX_LEVEL];
} ALLOC_LIST;

int int_log2(size_t size, size_t base) {
  int result = 0;
  for (size_t i = size; i > base; i >>= 1) {  // i == floor(size/2**result)
    result++;
  }
  return result;
}

int random_(uint32_t *state) {
  uint32_t r = *state;
  int result = 1;
  while ((((r = r * 1103515245 + 12345) >> 30) & 1) == 0) {
    result++;
  }
  *state = r;
  return result;
}

int lla_skip_list_levels(size_t size, size_t base, uint32_t *random) {
  size_t max_fit = (size - offsetof(ALLOC_LIST, next)) / sizeof(ALLOC_LIST *);
  int level = int_log2(size, base) + (random != NULL ? random_(random) : 1);
  if ((size_t)(level) > max_fit) level = (int)(max_fit);
  if (level > MAX_LEVEL - 1) level = MAX_LEVEL - 1;
  CIC_RAW_CHECK(level >= 1, "block not big enough for even one level");
  return level;
}

ALLOC_LIST *lla_skip_list_search(ALLOC_LIST *head, ALLOC_LIST *e, ALLOC_LIST **prev) {
  ALLOC_LIST *p = head;
  for (int level = head->levels - 1; level >= 0; level--) {
    for (ALLOC_LIST *n; (n = (*prev)->next[level]) != NULL && n < e; p = n) {
    }
    prev[level] = p;
  }
  return (head->levels == 0) ? NULL : prev[0]->next[0];
}

void lla_skip_list_insert(ALLOC_LIST *head, ALLOC_LIST *e, ALLOC_LIST **prev) {
  lla_skip_list_search(head, e, prev);
  for (; head->levels < e->levels; head->levels++) {  // extend prev pointers
    prev[head->levels] = head;                        // to all *e's levels
  }
  for (int i = 0; i != e->levels; i++) {  // add element to list
    e->next[i] = prev[i]->next[i];
    prev[i]->next[i] = e;
  }
}

void lla_skip_list_delete(ALLOC_LIST *head, ALLOC_LIST *e, ALLOC_LIST **prev) {
  ALLOC_LIST *found = lla_skip_list_search(head, e, prev);
  CIC_RAW_CHECK(e == found, "element not in freelist");
  for (int i = 0; i != e->levels && prev[i]->next[i] == e; i++) {
    prev[i]->next[i] = e->next[i];
  }
  while (head->levels > 0 && head->next[head->levels - 1] == NULL) {
    head->levels--;  // reduce head->levels if level unused
  }
}

typedef struct ARENA_ {
  SPIN_LOCK mu;
  struct ALLOC_LIST_ freelist;
  int32_t allocation_count;
  uint32_t flags;
  size_t pagesize;
  size_t round_up;
  size_t min_size;
  uint32_t random;
} ARENA;

// ---------------------------------------------------------------------------
// Arena implementation

unsigned char default_arena_storage[sizeof(ARENA)];
unsigned char unhooked_arena_storage[sizeof(ARENA)];
#ifndef CIC_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
unsigned char unhooked_async_sig_safe_arena_storage[sizeof(ARENA)];
#endif

once_flag_t create_globals_once;

ARENA *unhooked_arena() {
  // low_level_call_once(&create_globals_once, create_global_arenas);
  return (ARENA *)(&unhooked_arena_storage);
}

#ifndef CIC_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
ARENA *unhooked_async_sig_safe_arena() {
  // low_level_call_once(&create_globals_once, create_global_arenas);
  return (ARENA *)(&unhooked_async_sig_safe_arena_storage);
}
#endif

ARENA *default_arena() {
  // low_level_call_once(&create_globals_once, create_global_arenas);
  return (ARENA *)(&default_arena_storage);
}
void *low_level_alloc();

const uintptr_t MAGIC_ALLOCATED = 0x4c833e95U;
const uintptr_t MAGIC_UNALLOCATED = ~MAGIC_ALLOCATED;

typedef struct ARENA_LOCK_ {
  bool left_;
  ARENA *arena_;
} ARENA_LOCK;

void init_arena_lock(ARENA_LOCK *lock, ARENA *arena) {
  lock->arena_ = arena;
  // lock->arena_->mu.lockword; LOCK() #TODO: LOCK function
  lock->left_ = false;
}

void destroy_arena_lock(ARENA_LOCK *lock) { CIC_RAW_CHECK(lock->left_, "haven't left Arena region"); }

void leave_arena_lock(ARENA_LOCK *lock) {
  // lock->arena_.mu  #TODO: UNLOCK
  lock->left_ = true;
}

uintptr_t magic_(uintptr_t magic, HEADER *ptr) { return magic ^ (uintptr_t)(ptr); }

size_t get_page_size() {
#ifdef _WIN32
  SYSTEM_INFO system_info;
  GetSystemInfo(&system_info);
  max(system_info.dwPageSize, system_info.dwAllocationGranularity);
#elif defined(__wasm__) || defined(__asmjs__)
  return getpagesize();
#else
  return (size_t)(sysconf(_SC_PAGESIZE));
#endif
}

size_t rounded_up_block_size() {
  // Round up block sizes to a power of two close to the header size.
  size_t round_up = 16;
  while (round_up < sizeof(HEADER)) {
    round_up += round_up;
  }
  return round_up;
}

void init_arena(ARENA *arena, uint32_t flags_value) {
  arena->mu.lockword = SCHEDULE_KERNEL_ONLY;
  arena->allocation_count = 0;
  arena->flags = flags_value;
  arena->pagesize = get_page_size();
  arena->round_up = rounded_up_block_size();
  arena->random = 0;
  arena->freelist.header.size = 0;
  arena->freelist.header.magic = magic_(MAGIC_UNALLOCATED, &(arena->freelist.header));
  arena->freelist.header.arena = arena;
  arena->freelist.levels = 0;
}

// TODO:
void create_global_arenas() {
  ARENA *new;
  memcpy(default_arena_storage, new, sizeof(ARENA));
  memcpy(unhooked_arena_storage, new, sizeof(ARENA));
  memcpy(unhooked_async_sig_safe_arena_storage, new, sizeof(ARENA));
#ifndef CIC_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
#endif
}

uintptr_t checked_add(uintptr_t a, uintptr_t b) {
  uintptr_t sum = a + b;
  CIC_RAW_CHECK(sum >= a, "LowLevelAlloc arithmetic overflow");
  return sum;
}

uintptr_t round_up(uintptr_t addr, uintptr_t align) { return checked_add(addr, align - 1) & ~(align - 1); }

ALLOC_LIST *next_(int i, ALLOC_LIST *prev, ARENA *arena) {
  CIC_RAW_CHECK(i < prev->levels, "too few levels in Next()");
  ALLOC_LIST *next = prev->next[i];
  if (next != NULL) {
    CIC_RAW_CHECK(next->header.magic == magic_(MAGIC_UNALLOCATED, &next->header), "bad magic number in Next()");
    CIC_RAW_CHECK(next->header.arena == arena, "bad arena pointer in Next()");
    if (prev != &arena->freelist) {
      CIC_RAW_CHECK(prev < next, "unordered freelist");
      CIC_RAW_CHECK((char *)(prev) + prev->header.size < (char *)(next), "malformed freelist");
    }
  }
  return next;
}

void coalesce(ALLOC_LIST *a) {
  ALLOC_LIST *n = a->next[0];
  if (n != NULL && (char *)(a) + a->header.size == (char *)(n)) {
    ARENA *arena = a->header.arena;
    a->header.size += n->header.size;
    n->header.magic = 0;
    n->header.arena = NULL;
    ALLOC_LIST *prev[MAX_LEVEL];
    lla_skip_list_delete(&arena->freelist, n, prev);
    lla_skip_list_delete(&arena->freelist, a, prev);
    a->levels = lla_skip_list_levels(a->header.size, arena->min_size, &arena->random);
    lla_skip_list_insert(&arena->freelist, a, prev);
  }
}

void add_to_free_list(void *v, ARENA *arena) {
  ALLOC_LIST *f = (ALLOC_LIST *)((char *)(v) - sizeof(f->header));
  CIC_RAW_CHECK(f->header.magic == magic_(MAGIC_ALLOCATED, &f->header), "bad magic number in AddToFreelist()");
  CIC_RAW_CHECK(f->header.arena == arena, "bad arena pointer in AddToFreelist()");
  f->levels = lla_skip_list_levels(f->header.size, arena->min_size, &arena->random);
  ALLOC_LIST *prev[MAX_LEVEL];
  lla_skip_list_insert(&arena->freelist, f, prev);
  f->header.magic = magic_(MAGIC_UNALLOCATED, &f->header);
  coalesce(f);        // maybe coalesce with successor
  coalesce(prev[0]);  // maybe coalesce with predecessor
}

void low_level_alloc_free(void *v) {
  if (v != NULL) {
    ALLOC_LIST *f = (ALLOC_LIST *)((char *)(v) - sizeof(f->header));
    ARENA *arena = f->header.arena;
    // ARENA_LOCK section(arena);
    add_to_free_list(v, arena);
    CIC_RAW_CHECK(arena->allocation_count > 0, "nothing in arena to free");
    arena->allocation_count--;
    // section.Leave();
  }
}

void *do_alloc_with_arena(size_t request, ARENA *arena) {
  void *result = NULL;
  if (request != 0) {
    ALLOC_LIST *s;  // will point to region that satisfies request
    ARENA_LOCK section;
    init_arena_lock(&section, arena);
    size_t req_rnd = round_up(checked_add(request, sizeof(s->header)), arena->round_up);
    for (;;) {  // loop until we find a suitable region
      int i = lla_skip_list_levels(req_rnd, arena->min_size, NULL) - 1;
      if (i < arena->freelist.levels) {         // potential blocks exist
        ALLOC_LIST *before = &arena->freelist;  // PREDECESSOR OF S
        while ((s = next_(i, before, arena)) != NULL && s->header.size < req_rnd) {
          before = s;
        }
        if (s != NULL) {  // we found a region
          break;
        }
      }
      size_t new_pages_size = round_up(req_rnd, arena->pagesize * 16);
      void *new_pages;
#ifdef _WIN32
      new_pages = VirtualAlloc(nullptr, new_pages_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
      CIC_RAW_CHECK(new_pages != nullptr, "VirtualAlloc failed");
#else
#ifndef CIC_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
      if ((arena->flags & ASYNC_SIGNAL_SAFE) != 0) {
      } else {
        new_pages = mmap(NULL, new_pages_size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
      }
#else
      new_pages = mmap(NULL, new_pages_size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
#endif  // CIC_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
      if (new_pages == MAP_FAILED) {
      }

#ifdef __linux__
#if defined(PR_SET_VMA) && defined(PR_SET_VMA_ANON_NAME)
      prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, new_pages, new_pages_size, "cicflow");
#endif
#endif  // __linux__
#endif  // _WIN32
      // arena->mu.Lock();
      s = (ALLOC_LIST *)(new_pages);
      s->header.size = new_pages_size;
      s->header.magic = magic_(MAGIC_ALLOCATED, &s->header);
      s->header.arena = arena;
      add_to_free_list(&s->levels,
                       arena);  // insert new region into free list
    }
    ALLOC_LIST *prev[MAX_LEVEL];
    lla_skip_list_delete(&arena->freelist, s, prev);  // remove from free list
    if (checked_add(req_rnd, arena->min_size) <= s->header.size) {
      // big enough to split
      ALLOC_LIST *n = (ALLOC_LIST *)(req_rnd + (char *)(s));
      n->header.size = s->header.size - req_rnd;
      n->header.magic = magic_(MAGIC_ALLOCATED, &n->header);
      n->header.arena = arena;
      s->header.size = req_rnd;
      add_to_free_list(&n->levels, arena);
    }
    s->header.magic = magic_(MAGIC_ALLOCATED, &s->header);
    CIC_RAW_CHECK(s->header.arena == arena, "");
    arena->allocation_count++;
    leave_arena_lock(&section);
    result = &s->levels;
  }
  CIC_ANNOTATE_MEMORY_IS_UNINITIALIZED(result, request);
  return result;
}

void *alloc(size_t request) {
  void *result = do_alloc_with_arena(request, default_arena());
  return result;
}

void *alloc_with_arena(size_t request, ARENA *arena) {
  CIC_RAW_CHECK(arena != NULL, "must pass a valid arena");
  void *result = do_alloc_with_arena(request, arena);
  return result;
}
void alloc_free(void *s);

ARENA *new_arena(uint32_t flags) {
  ARENA *meta_data_arena = default_arena();
#ifndef CIC_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
  if ((flags & ASYNC_SIGNAL_SAFE) != 0) {
    meta_data_arena = unhooked_async_sig_safe_arena();
  } else  // NOLINT(readability/braces)
#endif
      if ((flags & CALL_MALLOC_HOOK) == 0) {
    meta_data_arena = unhooked_arena();
  }

  ARENA *result;
  ARENA *new;
  result = alloc_with_arena(sizeof(*result), meta_data_arena);
  init_arena(new, flags);
  // memcpy(result, new, sizoef(ARENA*));  // TODO:
  return result;
}

bool delete_arena(ARENA *arena) {
  CIC_RAW_CHECK(arena != NULL && arena != default_arena() && arena != unhooked_arena(), "may not delete default arena");
  ARENA_LOCK section;
  init_arena_lock(&section, arena);

  if (arena->allocation_count != 0) {
    leave_arena_lock(&section);
    return false;
  }
  while (arena->freelist.next[0] != NULL) {
    ALLOC_LIST *region = arena->freelist.next[0];
    size_t size = region->header.size;
    arena->freelist.next[0] = region->next[0];
    CIC_RAW_CHECK(region->header.magic == magic_(MAGIC_UNALLOCATED, &region->header),
                  "bad magic number in DeleteArena()");
    CIC_RAW_CHECK(region->header.arena == arena, "bad arena pointer in DeleteArena()");
    CIC_RAW_CHECK(size % arena->pagesize == 0, "empty arena has non-page-aligned block size");
    CIC_RAW_CHECK((uintptr_t)(region) % arena->pagesize == 0, "empty arena has non-page-aligned block");
    int munmap_result;
#ifdef _WIN32
    munmap_result = VirtualFree(region, 0, MEM_RELEASE);
    CIC_RAW_CHECK(munmap_result != 0, "LowLevelAlloc::DeleteArena: VitualFree failed");
#else
#ifndef CIC_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
    if ((arena->flags & ASYNC_SIGNAL_SAFE) == 0) {
      munmap_result = munmap(region, size);
    } else {
      // munmap_result = direct_munmap(region, size);
    }
#else
    munmap_result = munmap(region, size);
#endif  // CIC_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
    if (munmap_result != 0) {
      CIC_RAW_LOG(CIC_LOG_FATAL, "DeleteArena: munmap failed");
    }
#endif  // _WIN32
  }
  leave_arena_lock(&section);
  free(arena);
  return true;
}

#endif
