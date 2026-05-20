#include "test_framework.h"

#include "../WindingNumber/VM_SIMD.h"
#include "../WindingNumber/VM_SSEFunc.h"
#include "../WindingNumber/SYS_Types.h"
#include <cstring>

TEST_SUITE(SIMD)

TEST(VM_SIMD, v4ufConstruction)
{
    v4uf a(1.0f);
    ASSERT_FLOAT_EQ(a[0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(a[1], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(a[2], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(a[3], 1.0f, 1e-5f);
}

TEST(VM_SIMD, v4ufArrayConstruction)
{
    float arr[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    v4uf a(arr);
    ASSERT_FLOAT_EQ(a[0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(a[1], 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(a[2], 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(a[3], 4.0f, 1e-5f);
}

TEST(VM_SIMD, v4ufComponentConstruction)
{
    v4uf a(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_FLOAT_EQ(a[0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(a[1], 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(a[2], 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(a[3], 4.0f, 1e-5f);
}

TEST(VM_SIMD, v4ufAdd)
{
    v4uf a(1.0f, 2.0f, 3.0f, 4.0f);
    v4uf b(5.0f, 6.0f, 7.0f, 8.0f);
    v4uf c = a + b;
    ASSERT_FLOAT_EQ(c[0], 6.0f, 1e-5f);
    ASSERT_FLOAT_EQ(c[1], 8.0f, 1e-5f);
    ASSERT_FLOAT_EQ(c[2], 10.0f, 1e-5f);
    ASSERT_FLOAT_EQ(c[3], 12.0f, 1e-5f);
}

TEST(VM_SIMD, v4ufSub)
{
    v4uf a(5.0f, 6.0f, 7.0f, 8.0f);
    v4uf b(1.0f, 2.0f, 3.0f, 4.0f);
    v4uf c = a - b;
    ASSERT_FLOAT_EQ(c[0], 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(c[1], 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(c[2], 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(c[3], 4.0f, 1e-5f);
}

TEST(VM_SIMD, v4ufMul)
{
    v4uf a(2.0f, 3.0f, 4.0f, 5.0f);
    v4uf b(1.0f, 2.0f, 3.0f, 4.0f);
    v4uf c = a * b;
    ASSERT_FLOAT_EQ(c[0], 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(c[1], 6.0f, 1e-5f);
    ASSERT_FLOAT_EQ(c[2], 12.0f, 1e-5f);
    ASSERT_FLOAT_EQ(c[3], 20.0f, 1e-5f);
}

TEST(VM_SIMD, v4ufNegate)
{
    v4uf a(1.0f, -2.0f, 3.0f, -4.0f);
    v4uf b = -a;
    ASSERT_FLOAT_EQ(b[0], -1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(b[1], 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(b[2], -3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(b[3], 4.0f, 1e-5f);
}

TEST(VM_SIMD, v4ufAbs)
{
    v4uf a(-1.0f, 2.0f, -3.0f, 4.0f);
    v4uf b = a.abs();
    ASSERT_FLOAT_EQ(b[0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(b[1], 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(b[2], 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(b[3], 4.0f, 1e-5f);
}

TEST(VM_SIMD, v4ufComparison)
{
    v4uf a(1.0f, 2.0f, 3.0f, 4.0f);
    v4uf b(2.0f, 2.0f, 4.0f, 4.0f);

    v4uu lt = a < b;
    ASSERT_TRUE(lt[0] != 0);
    ASSERT_FALSE(lt[1] != 0);
    ASSERT_TRUE(lt[2] != 0);
    ASSERT_FALSE(lt[3] != 0);

    v4uu eq = a == b;
    ASSERT_FALSE(eq[0] != 0);
    ASSERT_TRUE(eq[1] != 0);
    ASSERT_FALSE(eq[2] != 0);
    ASSERT_TRUE(eq[3] != 0);
}

TEST(VM_SIMD, v4uuConstruction)
{
    v4uu a(1);
    ASSERT_EQ(a[0], 1);
    ASSERT_EQ(a[1], 1);
    ASSERT_EQ(a[2], 1);
    ASSERT_EQ(a[3], 1);
}

TEST(VM_SIMD, v4uuAdd)
{
    v4uu a(1);
    v4uu b(2);
    v4uu c = a + b;
    ASSERT_EQ(c[0], 3);
    ASSERT_EQ(c[1], 3);
    ASSERT_EQ(c[2], 3);
    ASSERT_EQ(c[3], 3);
}

TEST(VM_SIMD, sqrt)
{
    v4uf a(4.0f, 9.0f, 16.0f, 25.0f);
    v4uf b = sqrt(a);
    ASSERT_FLOAT_EQ(b[0], 2.0f, 1e-4f);
    ASSERT_FLOAT_EQ(b[1], 3.0f, 1e-4f);
    ASSERT_FLOAT_EQ(b[2], 4.0f, 1e-4f);
    ASSERT_FLOAT_EQ(b[3], 5.0f, 1e-4f);
}

TEST(VM_SIMD, v4ufLoadStore)
{
    float src[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    v4uf a = v4uf(src);

    float dst[4];
    dst[0] = a[0];
    dst[1] = a[1];
    dst[2] = a[2];
    dst[3] = a[3];

    ASSERT_FLOAT_EQ(dst[0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(dst[1], 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(dst[2], 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(dst[3], 4.0f, 1e-5f);
}

TEST(VM_SIMD, v4ufSplats)
{
    v4uf a = v4uf(3.14159f);
    ASSERT_FLOAT_EQ(a[0], 3.14159f, 1e-5f);
    ASSERT_FLOAT_EQ(a[1], 3.14159f, 1e-5f);
    ASSERT_FLOAT_EQ(a[2], 3.14159f, 1e-5f);
    ASSERT_FLOAT_EQ(a[3], 3.14159f, 1e-5f);
}

TEST(VM_SIMD, dot3)
{
    v4uf a(1.0f, 2.0f, 3.0f, 4.0f);
    v4uf b(2.0f, 3.0f, 4.0f, 5.0f);
    float d = dot3(a, b);
    ASSERT_FLOAT_EQ(d, 20.0f, 1e-5f);
}

TEST(VM_SIMD, dot4)
{
    v4uf a(1.0f, 2.0f, 3.0f, 4.0f);
    v4uf b(2.0f, 3.0f, 4.0f, 5.0f);
    float d = dot4(a, b);
    ASSERT_FLOAT_EQ(d, 40.0f, 1e-5f);
}

TEST(VM_SIMD, v4ufExtract)
{
    v4uf a(1.0f, 2.0f, 3.0f, 4.0f);
    float v0 = a[0];
    float v2 = a[2];
    ASSERT_FLOAT_EQ(v0, 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v2, 3.0f, 1e-5f);
}

TEST(VM_SIMD, madd)
{
    v4uf v(2.0f, 3.0f, 4.0f, 5.0f);
    v4uf f(1.0f, 2.0f, 3.0f, 4.0f);
    v4uf a(10.0f, 20.0f, 30.0f, 40.0f);
    v4uf result = madd(v, f, a);
    ASSERT_FLOAT_EQ(result[0], 12.0f, 1e-4f);
    ASSERT_FLOAT_EQ(result[1], 26.0f, 1e-4f);
    ASSERT_FLOAT_EQ(result[2], 42.0f, 1e-4f);
    ASSERT_FLOAT_EQ(result[3], 60.0f, 1e-4f);
}

TEST(VM_SIMD, v4ufMinMax)
{
    v4uf a(1.0f, 5.0f, 3.0f, 7.0f);
    v4uf b(2.0f, 4.0f, 6.0f, 8.0f);
    v4uf minv = vmin(a, b);
    v4uf maxv = vmax(a, b);

    ASSERT_FLOAT_EQ(minv[0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(minv[1], 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(minv[2], 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(minv[3], 7.0f, 1e-5f);

    ASSERT_FLOAT_EQ(maxv[0], 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(maxv[1], 5.0f, 1e-5f);
    ASSERT_FLOAT_EQ(maxv[2], 6.0f, 1e-5f);
    ASSERT_FLOAT_EQ(maxv[3], 8.0f, 1e-5f);
}

TEST(VM_SIMD, v4ufRecip)
{
    v4uf a(2.0f, 4.0f, 0.5f, 1.0f);
    v4uf r = a.recip();
    ASSERT_FLOAT_EQ(r[0], 0.5f, 1e-2f);
    ASSERT_FLOAT_EQ(r[1], 0.25f, 1e-2f);
    ASSERT_FLOAT_EQ(r[2], 2.0f, 1e-2f);
    ASSERT_FLOAT_EQ(r[3], 1.0f, 1e-2f);
}

TEST_END

int main() {
    return TestFramework::TestRegistry::instance().runAll();
}