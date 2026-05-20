#include "test_framework.h"

#include "../WindingNumber/UT_SolidAngle.h"
#include <cmath>

using Vec3 = HDK_Sample::UT_Vector3T<float>;

TEST_SUITE(UT_SolidAngle)

TEST(UT_SolidAngle, EmptyMesh)
{
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;
    solid_angle.init(0, nullptr, 0, nullptr, 2);

    Vec3 query(0.0f, 0.0f, 10.0f);
    float result = solid_angle.computeSolidAngle(query, 2.0);

    ASSERT_FLOAT_EQ(result, 0.0f, 1e-10f);
}

TEST(UT_SolidAngle, SingleTriangle)
{
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    Vec3 vertices[3];
    vertices[0] = Vec3(1.0f, 0.0f, 0.0f);
    vertices[1] = Vec3(0.0f, 1.0f, 0.0f);
    vertices[2] = Vec3(0.0f, 0.0f, 1.0f);

    int indices[3] = {0, 1, 2};

    solid_angle.init(1, indices, 3, vertices, 2);

    Vec3 query(0.0f, 0.0f, 2.0f);
    float result = solid_angle.computeSolidAngle(query, 2.0);

    ASSERT_TRUE(result > 0.0f);
    ASSERT_TRUE(result <= 1.0f);
}

TEST(UT_SolidAngle, PointOutsideMesh)
{
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    Vec3 vertices[3];
    vertices[0] = Vec3(1.0f, 0.0f, 0.0f);
    vertices[1] = Vec3(0.0f, 1.0f, 0.0f);
    vertices[2] = Vec3(0.0f, 0.0f, 1.0f);

    int indices[3] = {0, 1, 2};

    solid_angle.init(1, indices, 3, vertices, 2);

    Vec3 query(100.0f, 100.0f, 100.0f);
    float result = solid_angle.computeSolidAngle(query, 2.0);

    ASSERT_TRUE(std::fabs(result) < 1e-6f);
}

TEST(UT_SolidAngle, MultipleTriangles)
{
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    Vec3 vertices[4];
    vertices[0] = Vec3(-1.0f, -1.0f, 0.0f);
    vertices[1] = Vec3(1.0f, -1.0f, 0.0f);
    vertices[2] = Vec3(1.0f, 1.0f, 0.0f);
    vertices[3] = Vec3(-1.0f, 1.0f, 0.0f);

    int indices[6] = {0, 1, 2, 0, 2, 3};

    solid_angle.init(2, indices, 4, vertices, 2);

    Vec3 query(0.0f, 0.0f, 2.0f);
    float result = solid_angle.computeSolidAngle(query, 2.0);

    ASSERT_TRUE(result > 0.0f);
}

TEST(UT_SolidAngle, WindingNumberNormalization)
{
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    Vec3 vertices[3];
    vertices[0] = Vec3(1.0f, 0.0f, 0.0f);
    vertices[1] = Vec3(0.0f, 1.0f, 0.0f);
    vertices[2] = Vec3(0.0f, 0.0f, 1.0f);

    int indices[3] = {0, 1, 2};

    solid_angle.init(1, indices, 3, vertices, 2);

    Vec3 query(0.0f, 0.0f, 10.0f);
    float solid = solid_angle.computeSolidAngle(query, 2.0);
    float wn = solid / (4.0f * M_PI);

    ASSERT_TRUE(wn >= 0.0f);
    ASSERT_TRUE(wn <= 1.0f);
}

TEST_END

int main() {
    return TestFramework::TestRegistry::instance().runAll();
}