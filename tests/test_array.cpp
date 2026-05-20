#include "test_framework.h"

#include "../WindingNumber/UT_ArrayImpl.h"
#include "../WindingNumber/UT_SmallArray.h"

using UTArrayInt = HDK_Sample::UT_Array<int>;
using UTArrayFloat = HDK_Sample::UT_Array<float>;

TEST_SUITE(UT_Array)

TEST(UT_Array, DefaultConstructor)
{
    UTArrayInt arr;
    ASSERT_EQ(arr.size(), 0);
}

TEST(UT_Array, SizeConstructor)
{
    UTArrayInt arr(10);
    ASSERT_EQ(arr.size(), 10);
}

TEST(UT_Array, PushBack)
{
    UTArrayInt arr;
    arr.pushBack(1);
    arr.pushBack(2);
    arr.pushBack(3);
    ASSERT_EQ(arr.size(), 3);
    ASSERT_EQ(arr[0], 1);
    ASSERT_EQ(arr[1], 2);
    ASSERT_EQ(arr[2], 3);
}

TEST(UT_Array, Clear)
{
    UTArrayInt arr;
    arr.pushBack(1);
    arr.pushBack(2);
    arr.clear();
    ASSERT_EQ(arr.size(), 0);
}

TEST(UT_Array, MultiplePushBack)
{
    UTArrayInt arr;
    for (int i = 0; i < 1000; i++) {
        arr.pushBack(i);
    }
    ASSERT_EQ(arr.size(), 1000);
    ASSERT_EQ(arr[999], 999);
}

TEST(UT_Array, FloatArray)
{
    UTArrayFloat arr;
    arr.pushBack(1.5f);
    arr.pushBack(2.5f);
    arr.pushBack(3.5f);
    ASSERT_FLOAT_EQ(arr[0], 1.5f, 1e-5f);
    ASSERT_FLOAT_EQ(arr[1], 2.5f, 1e-5f);
    ASSERT_FLOAT_EQ(arr[2], 3.5f, 1e-5f);
}

TEST_END

int main() {
    return TestFramework::TestRegistry::instance().runAll();
}