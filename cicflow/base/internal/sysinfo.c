#include "cicflow/base/internal/sysinfo.h"

#include "cicflow/base/attributes.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef __linux__
#include <sys/syscall.h>
#endif

#if defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/sysctl.h>
#endif

#if defined(__myriad2__)
#include <rtems.h>
#endif

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// #include <limits>
// #include <thread> // NOLINT(build/c++11)
// #include <utility>
// #include <vector>

#include "cicflow/base/call_once.h"
#include "cicflow/base/config.h"
#include "cicflow/base/internal/raw_logging.h"
// #include "cicflow/base/internal/spinlock.h"
#include "cicflow/base/internal/unscaledcycleclock.h"
// #include "cicflow/base/thread_annotations.h"

#if defined(_WIN32)

#define Info SYSTEM_LOGICAL_PROCESSOR_INFORMATION

DWORD
Win32CountSetBits(ULONG_PTR bitMask) {
  for (DWORD bitSetCount = 0;; ++bitSetCount) {
    if (bitMask == 0) return bitSetCount;
    bitMask &= bitMask - 1;
  }
}

int Win32NumCPUs() {
#pragma comment(lib, "kernel32.lib")

  DWORD info_size = sizeof(Info);
  Info *info = (Info *)(malloc(info_size));
  if (info == NULL) return 0;

  bool success = GetLogicalProcessorInformation(info, &info_size);
  if (!success && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
    free(info);
    info = (Info *)(malloc(info_size));
    if (info == NULL) return 0;
    success = GetLogicalProcessorInformation(info, &info_size);
  }

  DWORD logicalProcessorCount = 0;
  if (success) {
    Info *ptr = info;
    DWORD byteOffset = 0;
    while (byteOffset + sizeof(Info) <= info_size) {
      switch (ptr->Relationship) {
        case RelationProcessorCore:
          logicalProcessorCount += Win32CountSetBits(ptr->ProcessorMask);
          break;

        case RelationNumaNode:
        case RelationCache:
        case RelationProcessorPackage:
          // Ignore other entries
          break;

        default:
          // Ignore unknown entries
          break;
      }
      byteOffset += sizeof(Info);
      ptr++;
    }
  }
  free(info);
  return (int)(logicalProcessorCount);
}

#endif

static int GetNumCPUs() {
#if defined(__myriad2__)
  return 1;
#elif defined(_WIN32)
  const int hardware_concurrency = Win32NumCPUs();
  return hardware_concurrency ? hardware_concurrency : 1;
#elif defined(_AIX)
  return sysconf(_SC_NPROCESSORS_ONLN);
#else
  return pthread_getconcurrency();
#endif
}

#if defined(_WIN32)

static double GetNominalCPUFrequency() {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) && \
    !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
  return 1.0;
#else
#pragma comment(lib, "advapi32.lib")  // For Reg* functions.
  HKEY key;
  if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0,
                    KEY_READ, &key) == ERROR_SUCCESS) {
    DWORD type = 0;
    DWORD data = 0;
    DWORD data_size = sizeof(data);
    auto result =
        RegQueryValueExA(key, "~MHz", nullptr, &type, reinterpret_cast<LPBYTE>(&data), &data_size);
    RegCloseKey(key);
    if (result == ERROR_SUCCESS && type == REG_DWORD && data_size == sizeof(data)) {
      return data * 1e6;  // Value is MHz.
    }
  }
  return 1.0;
#endif                                // WINAPI_PARTITION_APP && !WINAPI_PARTITION_DESKTOP
}

#elif defined(CTL_HW) && defined(HW_CPU_FREQ)

static double GetNominalCPUFrequency() {
  unsigned freq;
  size_t size = sizeof(freq);
  int mib[2] = {CTL_HW, HW_CPU_FREQ};
  if (sysctl(mib, 2, &freq, &size, NULL, 0) == 0) {
    return (double)(freq);
  }
  return 1.0;
}

#else

static bool ReadLongFromFile(const char *file, long *value) {
  bool ret = false;
  int fd = open(file, O_RDONLY | O_CLOEXEC);
  if (fd != -1) {
    char line[1024];
    char *err;
    memset(line, '\0', sizeof(line));
    ssize_t len;
    do {
      len = read(fd, line, sizeof(line) - 1);
    } while (len < 0 && errno == EINTR);
    if (len <= 0) {
      ret = false;
    } else {
      const long temp_value = strtol(line, &err, 10);
      if (line[0] != '\0' && (*err == '\n' || *err == '\0')) {
        *value = temp_value;
        ret = true;
      }
    }
    close(fd);
  }
  return ret;
}

#if defined(CIC_INTERNAL_UNSCALED_CYCLECLOCK_FREQUENCY_IS_CPU_FREQUENCY)

static int64_t ReadMonotonicClockNanos() {
  struct timespec t;
#ifdef CLOCK_MONOTONIC_RAW
  int rc = clock_gettime(CLOCK_MONOTONIC_RAW, &t);
#else
  int rc = clock_gettime(CLOCK_MONOTONIC, &t);
#endif
  if (rc != 0) {
    perror("clock_gettime() failed");
    abort();
  }
  return int64_t{t.tv_sec} * 1000000000 + t.tv_nsec;
}

static int64_t UnscaledCycleClockWrapperForInitializeFrequencyNow() {
  return UnscaledCycleClockNow();
}

struct TimeTscPair {
  int64_t time;  // From ReadMonotonicClockNanos().
  int64_t tsc;   // From UnscaledCycleClock::Now().
};

static TimeTscPair GetTimeTscPair() {
  int64_t best_latency = INT_MAX;
  TimeTscPair best;
  for (int i = 0; i < 10; ++i) {
    int64_t t0 = ReadMonotonicClockNanos();
    int64_t tsc = UnscaledCycleClockWrapperForInitializeFrequencyNow();
    int64_t t1 = ReadMonotonicClockNanos();
    int64_t latency = t1 - t0;
    if (latency < best_latency) {
      best_latency = latency;
      best.time = t0;
      best.tsc = tsc;
    }
  }
  return best;
}

// Measures and returns the TSC frequency by taking a pair of
// measurements approximately `sleep_nanoseconds` apart.
static double MeasureTscFrequencyWithSleep(int sleep_nanoseconds) {
  auto t0 = GetTimeTscPair();
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = sleep_nanoseconds;
  while (nanosleep(&ts, &ts) != 0 && errno == EINTR) {
  }
  auto t1 = GetTimeTscPair();
  double elapsed_ticks = t1.tsc - t0.tsc;
  double elapsed_time = (t1.time - t0.time) * 1e-9;
  return elapsed_ticks / elapsed_time;
}

// Measures and returns the TSC frequency by calling
// MeasureTscFrequencyWithSleep(), doubling the sleep interval until the
// frequency measurement stabilizes.
static double MeasureTscFrequency() {
  double last_measurement = -1.0;
  int sleep_nanoseconds = 1000000;  // 1 millisecond.
  for (int i = 0; i < 8; ++i) {
    double measurement = MeasureTscFrequencyWithSleep(sleep_nanoseconds);
    if (measurement * 0.99 < last_measurement && last_measurement < measurement * 1.01) {
      // Use the current measurement if it is within 1% of the
      // previous measurement.
      return measurement;
    }
    last_measurement = measurement;
    sleep_nanoseconds *= 2;
  }
  return last_measurement;
}

#endif  // CIC_INTERNAL_UNSCALED_CYCLECLOCK_FREQUENCY_IS_CPU_FREQUENCY

static double GetNominalCPUFrequency() {
  long freq = 0;

  if (ReadLongFromFile("/sys/devices/system/cpu/cpu0/tsc_freq_khz", &freq)) {
    return freq * 1e3;  // Value is kHz.
  }

#if defined(CIC_INTERNAL_UNSCALED_CYCLECLOCK_FREQUENCY_IS_CPU_FREQUENCY)
  return MeasureTscFrequency();
#else

  if (ReadLongFromFile("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", &freq)) {
    return freq * 1e3;  // Value is kHz.
  }

  return 1.0;
#endif  // !CIC_INTERNAL_UNSCALED_CYCLECLOCK_FREQUENCY_IS_CPU_FREQUENCY
}

#endif

static once_flag_t init_num_cpus_once;
static int num_cpus = 0;

// NumCPUs() may be called before main() and before malloc is properl
// initialized, therefore this must not allocate memory.
int NumCPUs() {
  // LowLevelCallOnce (&init_num_cpus_once, [] () { num_cpus = GetNumCPUs ();
  // });
  return num_cpus;
}

// A default frequency of 0.0 might be dangerous if it is used in division.
static once_flag init_nominal_cpu_frequency_once;
static double nominal_cpu_frequency = 1.0;

// NominalCPUFrequency() may be called before main() and before malloc is
// properly initialized, therefore this must not allocate memory.
double NominalCPUFrequency() {
  base_internal::LowLevelCallOnce(&init_nominal_cpu_frequency_once,
                                  []() { nominal_cpu_frequency = GetNominalCPUFrequency(); });
  return nominal_cpu_frequency;
}

#if defined(_WIN32)

pid_t GetTID() { return pid_t{GetCurrentThreadId()}; }

#elif defined(__linux__)

#ifndef SYS_gettid
#define SYS_gettid __NR_gettid
#endif

pid_t GetTID() { return (pid_t)(syscall(SYS_gettid)); }

#elif defined(__akaros__)

pid_t GetTID() {
  if (in_vcore_context()) return 0;
  return reinterpret_cast<struct pthread_tcb *>(current_uthread)->id;
}

#elif defined(__myriad2__)

pid_t GetTID() {
  uint32_t tid;
  rtems_task_ident(RTEMS_SELF, 0, &tid);
  return tid;
}

#else

// Fallback implementation of GetTID using pthread_getspecific.
once_flag tid_once;
sthread_key_t tid_key;
// SpinLock tid_lock (cicflow::kConstInit,
// base_internal::SCHEDULE_KERNEL_ONLY);

// We set a bit per thread in this array to indicate that an ID is in
// use. ID 0 is unused because it is the default value returned by
// pthread_getspecific().
// CIC_CONST_INIT static std::vector<uint32_t> *
//     tid_array CIC_GUARDED_BY (tid_lock)
//     = nullptr;
// static const int kBitsPerWord = 32; // tid_array is uint32_t.

// Returns the TID to tid_array.
static void FreeTID(void *v) {
  intptr_t tid = (intptr_t)(v);
  intptr_t word = tid / kBitsPerWord;
  // uint32_t mask = ~(1u << (tid % kBitsPerWord));
  // struct SpinLockHolder lock = &tid_lock;
  // assert (0 <= word && (size_t) (word) < tid_array->size ());
  // (*tid_array)[(size_t) (word)] &= mask;
}

static void InitGetTID() {
  if (pthread_key_create(&tid_key, FreeTID) != 0) {
    // The logging system calls GetTID() so it can't be used here.
    perror("pthread_key_create failed");
    abort();
  }

  // Initialize tid_array.
  // cicflow::base_internal::SpinLockHolder lock (&tid_lock);
  // tid_array = new std::vector<uint32_t> (1);
  // (*tid_array)[0] = 1; // ID 0 is never-allocated.
}

pid_t GetTID() {
  cicflow::call_once(tid_once, InitGetTID);

  intptr_t tid = (intptr_t)(pthread_getspecific(tid_key));
  if (tid != 0) {
    return (pid_t)(tid);
  }

  int bit;  // tid_array[word] = 1u << bit;
  size_t word;
  {
    // Search for the first unused ID.
    cicflow::base_internal::SpinLockHolder lock(&tid_lock);
    // First search for a word in the array that is not all ones.
    word = 0;
    while (word < tid_array->size() && ~(*tid_array)[word] == 0) {
      ++word;
    }
    if (word == tid_array->size()) {
      tid_array->push_back(0);  // No space left, add kBitsPerWord more IDs.
    }
    // Search for a zero bit in the word.
    bit = 0;
    while (bit < kBitsPerWord && (((*tid_array)[word] >> bit) & 1) != 0) {
      ++bit;
    }
    tid = static_cast<intptr_t>((word * kBitsPerWord) + static_cast<size_t>(bit));
    (*tid_array)[word] |= 1u << bit;  // Mark the TID as allocated.
  }

  if (pthread_setspecific(tid_key, (void *)(tid)) != 0) {
    perror("pthread_setspecific failed");
    abort();
  }

  return (pid_t)(tid);
}

#endif

pid_t GetCachedTID() {
#ifdef CIC_HAVE_THREAD_LOCAL
  static thread_local pid_t thread_id = GetTID();
  return thread_id;
#else
  return GetTID();
#endif  // CIC_HAVE_THREAD_LOCAL
}
