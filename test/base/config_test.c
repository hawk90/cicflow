#include "cicflow/base/config.h"

#include <stdint.h>

#include "unity.h"

void setUp() {}
void tearDown() {}

void endianness_test(void) {
  union {
    uint32_t value;
    uint8_t data[sizeof(uint32_t)];
  } number;

  number.data[0] = 0x00;
  number.data[1] = 0x01;
  number.data[2] = 0x02;
  number.data[3] = 0x03;

#if defined(CIC_IS_LITTLE_ENDIAN) && defined(CIC_IS_BIG_ENDIAN)
#error Both CIC_IS_LITTLE_ENDIAN and CIC_IS_BIG_ENDIAN are defined
#elif defined(CIC_IS_LITTLE_ENDIAN)
  TEST_ASSERT_EQUAL_UINT(UINT32_C(0x03020100), number.value);
#elif defined(CIC_IS_BIG_ENDIAN)
  TEST_ASSERT_EQUAL_UINT(UINT32_C(0x00010203), number.value);
#else
#error Unknown endianness
#endif
}

void config_test_thread_local(void) {}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  RUN_TEST(endianness_test);
#if defined(CIC_HAVE_THREAD_LOCAL)
  RUN_TEST(config_test_thread_local);
#endif
  return UNITY_END();
}
