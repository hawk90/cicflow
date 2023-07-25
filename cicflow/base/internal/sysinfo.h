
#ifndef CIC_BASE_INTERNAL_SYSINFO_H_
#define CIC_BASE_INTERNAL_SYSINFO_H_

#ifndef _WIN32
#include <sys/types.h>
#endif

#include <stdint.h>

#include "cicflow/base/config.h"
#include "cicflow/base/port.h"

double nominal_cpu_frequency();

int num_cpus();

#ifdef _WIN32
using pid_t = uint32_t;
#endif

pid_t get_tid();

pid_t get_cached_tid();

#endif  // CIC_BASE_INTERNAL_SYSINFO_H_
