#include "test_framework.h"

#include "../WindingNumber/SYS_Math.h"
#include "../WindingNumber/SYS_Types.h"
#include <cmath>

TEST_SUITE(Math)

TEST(SYS_Math, Sqrt)
{
    ASSERT_FLOAT_EQ(SYSsqrt(4.0f), 2.0f, 1e-6f);
    ASSERT_FLOAT_EQ(SYSsqrt(2.0f), std::sqrt(2.0f), 1e-6f);
    ASSERT_FLOAT_EQ(SYSsqrt(0.0f), 0.0f, 1e-6f);
}

TEST(SYS_Math, Abs)
{
    ASSERT_FLOAT_EQ(SYSabs(-3.0f), 3.0f, 1e-6f);
    ASSERT_FLOAT_EQ(SYSabs(3.0f), 3.0f, 1e-6f);
    ASSERT_FLOAT_EQ(SYSabs(0.0f), 0.0f, 1e-6f);
}

TEST(SYS_Math, MinMax)
{
    ASSERT_FLOAT_EQ(SYSmin(3.0f, 5.0f), 3.0f, 1e-6f);
    ASSERT_FLOAT_EQ(SYSmax(3.0f, 5.0f), 5.0f, 1e-6f);
}

TEST(SYS_Math, Clamp)
{
    ASSERT_FLOAT_EQ(SYSclamp(5.0f, 0.0f, 10.0f), 5.0f, 1e-6f);
    ASSERT_FLOAT_EQ(SYSclamp(-1.0f, 0.0f, 10.0f), 0.0f, 1e-6f);
    ASSERT_FLOAT_EQ(SYSclamp(15.0f, 0.0f, 10.0f), 10.0f, 1e-6f);
}

TEST(SYS_Math, Trigonometry)
{
    ASSERT_FLOAT_EQ(SYSsin(0.0f), 0.0f, 1e-6f);
    ASSERT_FLOAT_EQ(SYScos(0.0f), 1.0f, 1e-6f);
}

TEST_END

int main() {
    return TestFramework::TestRegistry::instance().runAll();
}