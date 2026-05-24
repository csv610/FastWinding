#include "test_framework.h"

#include "../WindingNumber/UT_Array.h"
#include "../WindingNumber/UT_SmallArray.h"

using UTArrayInt = UT_Array<int>;
using UTArrayFloat = UT_Array<float>;

TEST(UT_Array, DefaultConstructor)
{
    UTArrayInt arr;
    ASSERT_EQ(arr.size(), 0);
}

TEST(UT_Array, SizeConstructor)
{
    UTArrayInt arr;
    for (int i = 0; i < 10; i++) arr.append(i);
    ASSERT_EQ(arr.size(), 10);
}

TEST(UT_Array, PushBack)
{
    UTArrayInt arr;
    arr.append(1);
    arr.append(2);
    arr.append(3);
    ASSERT_EQ(arr.size(), 3);
    ASSERT_EQ(arr[0], 1);
    ASSERT_EQ(arr[1], 2);
    ASSERT_EQ(arr[2], 3);
}

TEST(UT_Array, Clear)
{
    UTArrayInt arr;
    arr.append(1);
    arr.append(2);
    arr.clear();
    ASSERT_EQ(arr.size(), 0);
}

TEST(UT_Array, MultiplePushBack)
{
    UTArrayInt arr;
    for (int i = 0; i < 1000; i++) {
        arr.append(i);
    }
    ASSERT_EQ(arr.size(), 1000);
    ASSERT_EQ(arr[999], 999);
}

TEST(UT_Array, FloatArray)
{
    UTArrayFloat arr;
    arr.append(1.5f);
    arr.append(2.5f);
    arr.append(3.5f);
    ASSERT_FLOAT_EQ(arr[0], 1.5f, 1e-5f);
    ASSERT_FLOAT_EQ(arr[1], 2.5f, 1e-5f);
    ASSERT_FLOAT_EQ(arr[2], 3.5f, 1e-5f);
}

int main() {
    return TestFramework::TestRunner::instance().runAll();
}
