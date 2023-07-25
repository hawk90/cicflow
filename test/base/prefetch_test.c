#include "cicflow/base/prefetch.h"

#include <stdio.h>
#include <stdlib.h>

#include "unity.h"

void setUp() {}
void tearDown() {}

void stack_test(void) {
  char buf[100] = {};

  prefetch_to_local_cache(buf);
  prefetch_to_local_cache_nta(buf);
  prefetch_to_local_cache_for_write(buf);
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  RUN_TEST(stack_test);
  // RUN_TEST(prefectch_to_local_cache_invalid_ptr_test);

  return UNITY_END();
}
