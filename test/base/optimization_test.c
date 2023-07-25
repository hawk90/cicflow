#include "cicflow/base/optimization.h"

#include <stdbool.h>
#include <stdio.h>

#include "unity.h"

void setUp() {}
void tearDown() {}

void predict_true_test(void) {
  TEST_ASSERT_TRUE(CIC_PREDICT_TRUE(true));
  TEST_ASSERT_FALSE(CIC_PREDICT_TRUE(false));
  TEST_ASSERT_TRUE(CIC_PREDICT_TRUE(1 == 1));
  TEST_ASSERT_FALSE(CIC_PREDICT_TRUE(1 == 2));

  TEST_ASSERT_TRUE(CIC_PREDICT_TRUE(true) && true);
  TEST_ASSERT_TRUE(CIC_PREDICT_TRUE(true) || false);
}

void predict_false_test(void) {
  TEST_ASSERT_TRUE(CIC_PREDICT_FALSE(true));
  TEST_ASSERT_FALSE(CIC_PREDICT_FALSE(false));
  TEST_ASSERT_TRUE(CIC_PREDICT_FALSE(1 == 1));
  TEST_ASSERT_FALSE(CIC_PREDICT_FALSE(1 == 2));

  TEST_ASSERT_TRUE(CIC_PREDICT_FALSE(true) && true);
  TEST_ASSERT_TRUE(CIC_PREDICT_FALSE(true) || false);
}

void one_evaluation_test(void) {}

void operator_order_test(void) {
  TEST_ASSERT_TRUE(CIC_PREDICT_TRUE(1 && 2) == true);
  TEST_ASSERT_TRUE(CIC_PREDICT_FALSE(1 && 2) == true);
  TEST_ASSERT_TRUE(!CIC_PREDICT_TRUE(1 == 2));
  TEST_ASSERT_TRUE(!CIC_PREDICT_FALSE(1 == 2));
}

void pointer_test(void) {
  const int x = 3;
  const int *good_ptr = &x;
  const int *null_ptr = NULL;

  TEST_ASSERT_TRUE(CIC_PREDICT_TRUE(good_ptr));
  TEST_ASSERT_FALSE(CIC_PREDICT_TRUE(null_ptr));
  TEST_ASSERT_TRUE(CIC_PREDICT_FALSE(good_ptr));
  TEST_ASSERT_FALSE(CIC_PREDICT_FALSE(null_ptr));
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  RUN_TEST(predict_true_test);
  RUN_TEST(predict_false_test);
  RUN_TEST(one_evaluation_test);
  RUN_TEST(operator_order_test);
  RUN_TEST(pointer_test);

  return UNITY_END();
}
