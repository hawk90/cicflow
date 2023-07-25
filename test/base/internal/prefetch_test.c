#include "cicflow/base/internal/prefetch.h"

#include <stdio.h>
#include <stdlib.h>

#include "unity.h"

void setUp() {}
void tearDown() {}

int number = 42;

void temporal_locality_none(void) { prefetch_nta(&number); }
void temporal_locality_low(void) { prefetch_t2(&number); }
void temporal_locality_medium(void) { prefetch_t1(&number); }
void temporal_locality_high(void) { prefetch_t0(&number); }

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  RUN_TEST(temporal_locality_none);
  RUN_TEST(temporal_locality_low);
  RUN_TEST(temporal_locality_medium);
  RUN_TEST(temporal_locality_high);

  return UNITY_END();
}
