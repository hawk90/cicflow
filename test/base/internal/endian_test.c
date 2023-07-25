#include "cicflow/base/internal/endian.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cicflow/base/config.h"
#include "cicflow/base/internal/unaligned_access.h"
#include "unity.h"

void setUp() {}
void tearDown() {}

#define BUF_SIZE 1000

const uint64_t INITIAL_NUMBER = 0x0123456789abcdef;
const uint64_t VALUE64 = 0x0123456789abcdef;
const uint32_t VALUE32 = 0x01234567;
const uint32_t VALUE16 = 0x0123;
const int NUM_VALUES_TO_TEST = 1000000;
const int RANDOM_SEED = 12345;

#if defined(CIC_IS_BIG_ENDIAN)
const uint64_t INITIAL_IN_NETWORK_ORDER = 0x0123456789abcdef;

const uint64_t VALUE_LE64 = 0xefcdab8967452301;
const uint32_t VALUE_LE32 = 0x67452301;
const uint16_t VALUE_LE16 = 0x2301;

const uint64_t VALUE_BE64 = 0x0123456789abcdef;
const uint32_t VALUE_BE32 = 0x01234567;
const uint16_t VALUE_BE16 = 0x0123;
#elif defined(CIC_IS_LITTLE_ENDIAN)
const uint64_t INITIAL_IN_NETWORK_ORDER = 0xefcdab8967452301;

const uint64_t VALUE_LE64 = 0x0123456789abcdef;
const uint32_t VALUE_LE32 = 0x01234567;
const uint16_t VALUE_LE16 = 0x0123;

const uint64_t VALUE_BE64 = 0xefcdab8967452301;
const uint32_t VALUE_BE32 = 0x67452301;
const uint16_t VALUE_BE16 = 0x2301;
#endif

uint16_t* generate_all_uint16_values() {
  uint16_t* result = malloc(1 << (sizeof(uint16_t) * 8));

  for (uint32_t i = 0; i < USHRT_MAX; ++i) {
    result[i] = (uint16_t)i;
  }
  return result;
}

void manual_byte_swap(char* bytes, int length) {
  if (length == 1) return;
  char tmp;

  TEST_ASSERT_EQUAL(0, length % 2);
  for (int i = 0; i < length / 2; ++i) {
    int j = (length - 1) - i;

    tmp = bytes[i];
    bytes[i] = bytes[j];
    bytes[j] = tmp;
  }
}

void swap16(char* bytes) { unaligned_store16(bytes, gbswap_16(unaligned_load16(bytes))); }
void swap32(char* bytes) { unaligned_store32(bytes, gbswap_32(unaligned_load32(bytes))); }
void swap64(char* bytes) { unaligned_store64(bytes, gbswap_64(unaligned_load64(bytes))); }

void uint16_test() {
  char message[BUF_SIZE];
  uint16_t* host_values_to_test = generate_all_uint16_values();
  uint16_t host_value;

  for (uint32_t i = 0; i <= USHRT_MAX; ++i) {
    host_value = host_values_to_test[i];
    char actual_value[sizeof(host_value)];
    memcpy(actual_value, &host_value, sizeof(host_value));
    swap16(actual_value);

    char expected_value[sizeof(host_value)];
    memcpy(expected_value, &host_value, sizeof(host_value));
    manual_byte_swap(expected_value, sizeof(host_value));

    sprintf(message, "Swap output for %x does not match. Expected: %x; Actual: %x", host_value,
            unaligned_load16(expected_value), unaligned_load16(actual_value));
    TEST_ASSERT_EQUAL_MESSAGE(0, memcmp(actual_value, expected_value, sizeof(host_value)), message);
  }
}

void uint32_test() {
  char message[BUF_SIZE];
  uint32_t host_value;

  srand(RANDOM_SEED);
  for (uint32_t i = 0; i < NUM_VALUES_TO_TEST; ++i) {
    host_value = (uint32_t)rand();

    char actual_value[sizeof(host_value)];
    memcpy(actual_value, &host_value, sizeof(host_value));
    swap32(actual_value);

    char expected_value[sizeof(host_value)];
    memcpy(expected_value, &host_value, sizeof(host_value));
    manual_byte_swap(expected_value, sizeof(host_value));

    sprintf(message, "Swap output for %x does not match. Expected: %x; Actual: %x", host_value,
            unaligned_load16(expected_value), unaligned_load16(actual_value));
    TEST_ASSERT_EQUAL_MESSAGE(0, memcmp(actual_value, expected_value, sizeof(host_value)), message);
  }

  return;
}

void uint64_test() {
  char message[BUF_SIZE];
  uint64_t host_value;

  srand(RANDOM_SEED);
  for (uint32_t i = 0; i < NUM_VALUES_TO_TEST; ++i) {
    host_value = (uint64_t)rand();

    char actual_value[sizeof(host_value)];
    memcpy(actual_value, &host_value, sizeof(host_value));
    swap64(actual_value);

    char expected_value[sizeof(host_value)];
    memcpy(expected_value, &host_value, sizeof(host_value));
    manual_byte_swap(expected_value, sizeof(host_value));

    sprintf(message, "Swap output for %lx does not match. Expected: %x; Actual: %x", host_value,
            unaligned_load16(expected_value), unaligned_load16(actual_value));
    TEST_ASSERT_EQUAL_MESSAGE(0, memcmp(actual_value, expected_value, sizeof(host_value)), message);
  }

  return;
}

void ghtonll_gntohll() {
  uint32_t test = 0x01234567;
  TEST_ASSERT_EQUAL(gntohl(ghtonl(test)), test);

  uint64_t comp = ghtonll(INITIAL_NUMBER);
  TEST_ASSERT_EQUAL(comp, INITIAL_IN_NETWORK_ORDER);
  comp = gntohll(INITIAL_IN_NETWORK_ORDER);
  TEST_ASSERT_EQUAL(comp, INITIAL_NUMBER);

  uint64_t value = 1;
  for (int i = 0; i < 100; ++i) {
    comp = ghtonll(gntohll(value));
    TEST_ASSERT_EQUAL(value, comp);
    comp = gntohll(ghtonll(value));
    TEST_ASSERT_EQUAL(value, comp);
    value *= 37;
  }
}

void little_endian_test() {
  uint64_t comp = little_endian_from_host16(VALUE16);
  TEST_ASSERT_EQUAL(comp, VALUE_LE16);
  comp = little_endian_to_host16(VALUE16);
  TEST_ASSERT_EQUAL(comp, VALUE16);

  comp = little_endian_from_host32(VALUE32);
  TEST_ASSERT_EQUAL(comp, VALUE_LE32);
  comp = little_endian_to_host32(VALUE32);
  TEST_ASSERT_EQUAL(comp, VALUE32);

  comp = little_endian_from_host64(VALUE64);
  TEST_ASSERT_EQUAL(comp, VALUE_LE64);
  comp = little_endian_to_host64(VALUE64);
  TEST_ASSERT_EQUAL(comp, VALUE64);

  uint16_t u16Buf;
  uint32_t u32Buf;
  uint64_t u64Buf;

  little_endian_store16(&u16Buf, VALUE16);
  TEST_ASSERT_EQUAL(u16Buf, VALUE_LE16);
  comp = little_endian_load16(&u16Buf);
  TEST_ASSERT_EQUAL(comp, VALUE16);

  little_endian_store32(&u32Buf, VALUE32);
  TEST_ASSERT_EQUAL(u32Buf, VALUE_LE32);
  comp = little_endian_load32(&u32Buf);
  TEST_ASSERT_EQUAL(comp, VALUE32);

  little_endian_store64(&u64Buf, VALUE64);
  TEST_ASSERT_EQUAL(u64Buf, VALUE_LE64);
  comp = little_endian_load64(&u64Buf);
  TEST_ASSERT_EQUAL(comp, VALUE64);
}

void big_endian_test() {
  uint16_t u16Buf;
  uint32_t u32Buf;
  uint64_t u64Buf;

  unsigned char buffer[10];
  big_endian_store16(&u16Buf, VALUE16);
  TEST_ASSERT_EQUAL(u16Buf, VALUE_BE16);
  uint64_t comp = big_endian_load16(&u16Buf);
  TEST_ASSERT_EQUAL(comp, VALUE16);

  big_endian_store32(&u32Buf, VALUE32);
  TEST_ASSERT_EQUAL(u32Buf, VALUE_BE32);
  comp = big_endian_load32(&u32Buf);
  TEST_ASSERT_EQUAL(comp, VALUE32);

  big_endian_store64(&u64Buf, VALUE64);
  TEST_ASSERT_EQUAL(u64Buf, VALUE_BE64);
  comp = big_endian_load64(&u64Buf);
  TEST_ASSERT_EQUAL(comp, VALUE64);

  big_endian_store16(buffer + 1, VALUE16);
  TEST_ASSERT_EQUAL(u16Buf, VALUE_BE16);
  comp = big_endian_load16(buffer + 1);
  TEST_ASSERT_EQUAL(comp, VALUE16);

  big_endian_store32(buffer + 1, VALUE32);
  TEST_ASSERT_EQUAL(u32Buf, VALUE_BE32);
  comp = big_endian_load32(buffer + 1);
  TEST_ASSERT_EQUAL(comp, VALUE32);

  big_endian_store64(buffer + 1, VALUE64);
  TEST_ASSERT_EQUAL(u64Buf, VALUE_BE64);
  comp = big_endian_load64(buffer + 1);
  TEST_ASSERT_EQUAL(comp, VALUE64);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(uint16_test);
  RUN_TEST(uint32_test);
  RUN_TEST(uint64_test);
  RUN_TEST(ghtonll_gntohll);
  RUN_TEST(little_endian_test);

  return UNITY_END();
}
