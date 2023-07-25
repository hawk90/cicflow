#include "cicflow/base/internal/raw_logging.h"

#include <stdbool.h>
#include <stdio.h>

#include "unity.h"

void setUp() {}
void tearDown() {}

void log_test() {
  CIC_RAW_LOG(CIC_LOG_INFO, "RAW INFO: %d", 1);
  CIC_RAW_LOG(CIC_LOG_INFO, "RAW INFO: %d %d", 1, 2);
  CIC_RAW_LOG(CIC_LOG_INFO, "RAW INFO: %d %d %d", 1, 2, 3);
  CIC_RAW_LOG(CIC_LOG_INFO, "RAW INFO: %d %d %d %d", 1, 2, 3, 4);
  CIC_RAW_LOG(CIC_LOG_INFO, "RAW INFO: %d %d %d %d %d", 1, 2, 3, 4, 5);
  CIC_RAW_LOG(CIC_LOG_WARNING, "RAW WARNING: %d", 1);
  CIC_RAW_LOG(CIC_LOG_ERROR, "RAW ERROR: %d", 1);
}

void passing_check_test() { CIC_RAW_CHECK(true, "RAW CHECK"); }

void failing_check_test() { CIC_RAW_CHECK(1 == 0, "explanation"); }

void log_fatal_test() { CIC_RAW_LOG(CIC_LOG_FATAL, "explanation"); }

void compolation_test() {
  atomic_hook_init();

  CIC_INTERNAL_LOG(CIC_LOG_INFO, "Internal Log");
  char *log_msg = "Internal Log";
  CIC_INTERNAL_LOG(CIC_LOG_INFO, log_msg);
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  RUN_TEST(log_test);
  // RUN_TEST(passing_check_test);
  // RUN_TEST(failing_check_test);
  // RUN_TEST(log_fatal_test);
  RUN_TEST(compolation_test);

  return UNITY_END();
}
