/*
 Copyright (c) 2014-present PlatformIO <contact@platformio.org>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
**/

#include <calculator.hpp>
#include <unity.h>

Calculator calc;

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_calculatorAddition(void) { TEST_ASSERT_EQUAL(32, calc.add(25, 7)); }

void test_calculatorSubtraction(void) {
  TEST_ASSERT_EQUAL(20, calc.sub(23, 3));
}

void test_calculatorMultiplication(void) {
  TEST_ASSERT_EQUAL(50, calc.mul(25, 2));
}

void test_calculatorDivision(void) { TEST_ASSERT_EQUAL(32, calc.div(96, 3)); }

void test_expensiveOperation(void) { TEST_IGNORE(); }

void RUN_UNITY_TESTS() {
  UNITY_BEGIN();
  RUN_TEST(test_calculatorAddition);
  RUN_TEST(test_calculatorSubtraction);
  RUN_TEST(test_calculatorMultiplication);
  RUN_TEST(test_calculatorDivision);
  RUN_TEST(test_expensiveOperation);
  UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  RUN_UNITY_TESTS();
}

void loop() {
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(500);
}

#else

int main(int argc, char **argv) {
  RUN_UNITY_TESTS();
  return 0;
}

#endif
