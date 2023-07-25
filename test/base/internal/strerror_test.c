#include "cicflow/base/internal/strerror.h"

#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

#include "unity.h"

void setUp() {}
void tearDown() {}

void valide_error_code(void) {
  // errno = ERANGE;
  // TEST_ASSERT_EQUAL_STRING(str_error(ERANGE), strerror(ERANGE));
  str_error(ERANGE);
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  RUN_TEST(valide_error_code);
  // RUN_TEST(temporal_locality_high);

  return UNITY_END();
}
