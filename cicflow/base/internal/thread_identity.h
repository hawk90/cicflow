#ifndef CIC_BASE_INTERNAL_THREAD_IDENTITY_H_
#define CIC_BASE_INTERNAL_THREAD_IDENTITY_H_

#ifndef _WIN32
#include <pthread.h>
#include <unistd.h>
#endif

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "cicflow/base/config.h"
#include "cicflow/base/internal/per_thread_tls.h"
#include "cicflow/base/optimization.h"

typedef enum { AVAILABLE, QUEUED } STATE;

struct SYNC_WAIT_PARAMS;
struct THREAD_IDENTITY_;

static const int LOW_ZERO_BITS = 8;
static const int Alignment = 1 << LOW_ZERO_BITS;

typedef struct PER_THREAD_SYNCH_ {
  struct PER_THREAD_SYNCH_ *next;
  struct PER_THREAD_SYNCH_ *skip;

  bool may_skip;
  bool wake;
  bool cond_watier;
  bool maybe_unlocking;
  bool suppress_fatal_errors;
  int priority;

  _Atomic(STATE) state;
  struct SYNC_WAIT_PARAMS *waitp;
  intptr_t readers;
  int64_t next_priority_read_cycles;
  // SYNCH_LOCKS_HELD * all_locks;

} PER_THREAD_SYNCH;

struct THREAD_IDENTITY_ {
  PER_THREAD_SYNCH per_thread_synch;
  struct WAITER_STATES {
    char date[256];
  } waiter_state;

  _Atomic(int *) blocked_count_ptr;

  _Atomic(int) ticker;
  _Atomic(int) wait_start;
  _Atomic(bool) is_idle;

  struct THREAD_IDENTITY_ *next;
};

struct THREAD_IDENTITY_ *current_thread_identity_if_present();

typedef void (*thread_indentity_reclaimer_function)(void *);

void set_current_thread_identity(struct THREAD_IDENTITY_ *identity, thread_indentity_reclaimer_function reclaimer);
void clear_current_thread_identity();

#endif
