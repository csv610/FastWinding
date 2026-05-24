#include "test_framework.h"

#include "../WindingNumber/UT_SolidAngle.h"
#include <numbers>
#include <cmath>

using Vec3 = HDK_Sample::UT_Vector3T<float>;

static Vec3 make_vec3(float x, float y, float z) {
    Vec3 v;
    v[0] = x; v[1] = y; v[2] = z;
    return v;
}

TEST(UT_SolidAngle, EmptyMesh)
{
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;
    solid_angle.init(0, nullptr, 0, nullptr, 2);

    Vec3 query = make_vec3(0.0f, 0.0f, 10.0f);
    float result = solid_angle.computeSolidAngle(query, 2.0);

    ASSERT_FLOAT_EQ(result, 0.0f, 1e-10f);
}

TEST(UT_SolidAngle, SingleTriangle)
{
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    Vec3 vertices[3];
    vertices[0] = make_vec3(1.0f, 0.0f, 0.0f);
    vertices[1] = make_vec3(0.0f, 1.0f, 0.0f);
    vertices[2] = make_vec3(0.0f, 0.0f, 1.0f);

    int indices[3] = {0, 1, 2};

    solid_angle.init(1, indices, 3, vertices, 2);

    Vec3 query = make_vec3(0.0f, 0.0f, 2.0f);
    float result = solid_angle.computeSolidAngle(query, 2.0);

    ASSERT_TRUE(result > 0.0f);
    ASSERT_TRUE(result <= 1.0f);
}

TEST(UT_SolidAngle, PointOutsideMesh)
{
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    Vec3 vertices[3];
    vertices[0] = make_vec3(1.0f, 0.0f, 0.0f);
    vertices[1] = make_vec3(0.0f, 1.0f, 0.0f);
    vertices[2] = make_vec3(0.0f, 0.0f, 1.0f);

    int indices[3] = {0, 1, 2};

    solid_angle.init(1, indices, 3, vertices, 2);

    Vec3 query = make_vec3(100.0f, 100.0f, 100.0f);
    float result = solid_angle.computeSolidAngle(query, 2.0);

    ASSERT_TRUE(std::fabs(result) < 1e-6f);
}

TEST(UT_SolidAngle, MultipleTriangles)
{
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    Vec3 vertices[4];
    vertices[0] = make_vec3(-1.0f, -1.0f, 0.0f);
    vertices[1] = make_vec3(1.0f, -1.0f, 0.0f);
    vertices[2] = make_vec3(1.0f, 1.0f, 0.0f);
    vertices[3] = make_vec3(-1.0f, 1.0f, 0.0f);

    int indices[6] = {0, 1, 2, 0, 2, 3};

    solid_angle.init(2, indices, 4, vertices, 2);

    Vec3 query = make_vec3(0.0f, 0.0f, 2.0f);
    float result = solid_angle.computeSolidAngle(query, 2.0);

    ASSERT_TRUE(result > 0.0f);
}

TEST(UT_SolidAngle, WindingNumberNormalization)
{
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    Vec3 vertices[3];
    vertices[0] = make_vec3(1.0f, 0.0f, 0.0f);
    vertices[1] = make_vec3(0.0f, 1.0f, 0.0f);
    vertices[2] = make_vec3(0.0f, 0.0f, 1.0f);

    int indices[3] = {0, 1, 2};

    solid_angle.init(1, indices, 3, vertices, 2);

    Vec3 query = make_vec3(0.0f, 0.0f, 10.0f);
    float solid = solid_angle.computeSolidAngle(query, 2.0);
    float wn = solid / (4.0f * static_cast<float>(std::numbers::pi));

    ASSERT_TRUE(wn >= 0.0f);
    ASSERT_TRUE(wn <= 1.0f);
}

int main() {
    return TestFramework::TestRunner::instance().runAll();
}
