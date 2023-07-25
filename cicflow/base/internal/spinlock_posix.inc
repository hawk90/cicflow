
#include <sched.h>

#include <stdatomic.h>
#include <stdbool.h>
#include <time.h>

#include "cicflow/base/internal/errno_saver.h"
#include "cicflow/base/internal/scheduling_mode.h"
#include "cicflow/base/port.h"

CIC_ATTRIBUTE_WEAK void
cicflowInternalSpinLockDelay (_Atomic uint32_t * /* lock_word */,
                              uint32_t /* value */, int loop,
                              SchedulingMode /* mode */)
{
  if (loop == 0)
    {
    }
  else if (loop == 1)
    {
      sched_yield ();
    }
  else
    {
      struct timespec tm;
      tm.tv_sec = 0;
      tm.tv_nsec = SpinLockSuggestedDelayNS (loop);
      nanosleep (&tm, NULL);
    }
}

CIC_ATTRIBUTE_WEAK void
cicflowInternalSpinLockWake (_Atomic uint32_t * /* lock_word */,
                             bool /* all */)
{
}
