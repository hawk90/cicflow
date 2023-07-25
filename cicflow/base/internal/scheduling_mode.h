#ifndef CIC_BASE_INTERNAL_SCHEDULING_MODE_H_
#define CIC_BASE_INTERNAL_SCHEDULING_MODE_H_

typedef enum {
  SCHEDULE_KERNEL_ONLY = 0,         // Allow scheduling only the host OS.
  SCHEDULE_COOPERATIVE_AND_KERNEL,  // Also allow cooperative scheduling.
} SCHEDULING_MODE;

#endif
