#include "test_framework.h"

#include "../WindingNumber/UT_BVHImpl.h"

TEST(UT_BVH, EmptyBVH)
{
    HDK_Sample::UT::BVH<4> bvh;
    ASSERT_EQ(bvh.getNumNodes(), 0u);
}

TEST(UT_BVH, SingleBox)
{
    HDK_Sample::UT::BVH<4> bvh;

    HDK_Sample::UT::Box<float, 3> box;
    box.vals[0][0] = 0.0f; box.vals[1][0] = 0.0f; box.vals[2][0] = 0.0f;
    box.vals[0][1] = 1.0f; box.vals[1][1] = 1.0f; box.vals[2][1] = 1.0f;

    HDK_Sample::UT::Box<float, 3> boxes[1] = {box};
    unsigned int indices[1] = {0};

    bvh.init<HDK_Sample::UT::BVH_Heuristic::BOX_AREA, float, 3>(boxes, 1, indices, false, 4);

    ASSERT_TRUE(bvh.getNumNodes() > 0);
}

TEST(UT_BVH, MultipleBoxes)
{
    HDK_Sample::UT::BVH<4> bvh;

    HDK_Sample::UT::Box<float, 3> boxes[3];
    boxes[0].vals[0][0] = 0.0f; boxes[0].vals[1][0] = 0.0f; boxes[0].vals[2][0] = 0.0f;
    boxes[0].vals[0][1] = 1.0f; boxes[0].vals[1][1] = 1.0f; boxes[0].vals[2][1] = 1.0f;
    boxes[1].vals[0][0] = 2.0f; boxes[1].vals[1][0] = 2.0f; boxes[1].vals[2][0] = 2.0f;
    boxes[1].vals[0][1] = 3.0f; boxes[1].vals[1][1] = 3.0f; boxes[1].vals[2][1] = 3.0f;
    boxes[2].vals[0][0] = 4.0f; boxes[2].vals[1][0] = 4.0f; boxes[2].vals[2][0] = 4.0f;
    boxes[2].vals[0][1] = 5.0f; boxes[2].vals[1][1] = 5.0f; boxes[2].vals[2][1] = 5.0f;

    unsigned int indices[3] = {0, 1, 2};

    bvh.init<HDK_Sample::UT::BVH_Heuristic::BOX_AREA, float, 3>(boxes, 3, indices, false, 4);

    ASSERT_TRUE(bvh.getNumNodes() > 0);
}

TEST(UT_BVH, BoxOverlap)
{
    HDK_Sample::UT::Box<float, 3> a;
    a.vals[0][0] = 0.0f; a.vals[1][0] = 0.0f; a.vals[2][0] = 0.0f;
    a.vals[0][1] = 2.0f; a.vals[1][1] = 2.0f; a.vals[2][1] = 2.0f;

    HDK_Sample::UT::Box<float, 3> b;
    b.vals[0][0] = 1.0f; b.vals[1][0] = 1.0f; b.vals[2][0] = 1.0f;
    b.vals[0][1] = 3.0f; b.vals[1][1] = 3.0f; b.vals[2][1] = 3.0f;

    bool overlap = true;
    for (int axis = 0; axis < 3; axis++) {
        if (a.vals[axis][1] < b.vals[axis][0] || b.vals[axis][1] < a.vals[axis][0]) {
            overlap = false;
            break;
        }
    }
    ASSERT_TRUE(overlap);
}

TEST(UT_BVH, BoxNoOverlap)
{
    HDK_Sample::UT::Box<float, 3> a;
    a.vals[0][0] = 0.0f; a.vals[1][0] = 0.0f; a.vals[2][0] = 0.0f;
    a.vals[0][1] = 1.0f; a.vals[1][1] = 1.0f; a.vals[2][1] = 1.0f;

    HDK_Sample::UT::Box<float, 3> b;
    b.vals[0][0] = 2.0f; b.vals[1][0] = 2.0f; b.vals[2][0] = 2.0f;
    b.vals[0][1] = 3.0f; b.vals[1][1] = 3.0f; b.vals[2][1] = 3.0f;

    bool overlap = true;
    for (int axis = 0; axis < 3; axis++) {
        if (a.vals[axis][1] < b.vals[axis][0] || b.vals[axis][1] < a.vals[axis][0]) {
            overlap = false;
            break;
        }
    }
    ASSERT_FALSE(overlap);
}

TEST(UT_BVH, BoxHalfSurfaceArea)
{
    HDK_Sample::UT::Box<float, 3> box;
    box.vals[0][0] = 0.0f; box.vals[1][0] = 0.0f; box.vals[2][0] = 0.0f;
    box.vals[0][1] = 2.0f; box.vals[1][1] = 3.0f; box.vals[2][1] = 4.0f;

    float area = box.half_surface_area();
    float expected = 2.0f*3.0f + 3.0f*4.0f + 4.0f*2.0f;
    ASSERT_FLOAT_EQ(area, expected, 1e-5f);
}

TEST(UT_BVH, BoxCenter)
{
    HDK_Sample::UT::Box<float, 3> box;
    box.vals[0][0] = 0.0f; box.vals[1][0] = 0.0f; box.vals[2][0] = 0.0f;
    box.vals[0][1] = 2.0f; box.vals[1][1] = 4.0f; box.vals[2][1] = 6.0f;

    float center[3];
    for (int axis = 0; axis < 3; axis++)
        center[axis] = (box.vals[axis][0] + box.vals[axis][1]) * 0.5f;

    ASSERT_FLOAT_EQ(center[0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(center[1], 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(center[2], 3.0f, 1e-5f);
}

TEST(UT_BVH, BoxContains)
{
    HDK_Sample::UT::Box<float, 3> box;
    box.vals[0][0] = 0.0f; box.vals[1][0] = 0.0f; box.vals[2][0] = 0.0f;
    box.vals[0][1] = 2.0f; box.vals[1][1] = 2.0f; box.vals[2][1] = 2.0f;

    float point[3] = {1.0f, 1.0f, 1.0f};
    bool inside = true;
    for (int axis = 0; axis < 3; axis++)
        if (point[axis] < box.vals[axis][0] || point[axis] > box.vals[axis][1])
            inside = false;
    ASSERT_TRUE(inside);

    float outside[3] = {3.0f, 3.0f, 3.0f};
    inside = true;
    for (int axis = 0; axis < 3; axis++)
        if (outside[axis] < box.vals[axis][0] || outside[axis] > box.vals[axis][1])
            inside = false;
    ASSERT_FALSE(inside);
}

TEST(UT_BVH, MergeBoxes)
{
    HDK_Sample::UT::Box<float, 3> a;
    a.vals[0][0] = 0.0f; a.vals[1][0] = 0.0f; a.vals[2][0] = 0.0f;
    a.vals[0][1] = 1.0f; a.vals[1][1] = 1.0f; a.vals[2][1] = 1.0f;

    HDK_Sample::UT::Box<float, 3> b;
    b.vals[0][0] = 2.0f; b.vals[1][0] = 2.0f; b.vals[2][0] = 2.0f;
    b.vals[0][1] = 3.0f; b.vals[1][1] = 3.0f; b.vals[2][1] = 3.0f;

    HDK_Sample::UT::Box<float, 3> merged = a;
    merged.combine(b);

    ASSERT_FLOAT_EQ(merged.vals[0][0], 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(merged.vals[1][0], 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(merged.vals[2][0], 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(merged.vals[0][1], 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(merged.vals[1][1], 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(merged.vals[2][1], 3.0f, 1e-5f);
}

TEST(UT_BVH, LargeBVH)
{
    HDK_Sample::UT::BVH<4> bvh;

    const int n = 100;
    std::vector<HDK_Sample::UT::Box<float, 3>> boxes(n);
    std::vector<unsigned int> indices(n);

    for (int i = 0; i < n; i++) {
        boxes[i].vals[0][0] = static_cast<float>(i);
        boxes[i].vals[1][0] = static_cast<float>(i);
        boxes[i].vals[2][0] = static_cast<float>(i);
        boxes[i].vals[0][1] = static_cast<float>(i) + 1.0f;
        boxes[i].vals[1][1] = static_cast<float>(i) + 1.0f;
        boxes[i].vals[2][1] = static_cast<float>(i) + 1.0f;
        indices[i] = i;
    }

    bvh.init<HDK_Sample::UT::BVH_Heuristic::BOX_AREA, float, 3>(boxes.data(), n, indices.data(), false, 4);

    ASSERT_TRUE(bvh.getNumNodes() > 0);
}

int main() {
    return TestFramework::TestRunner::instance().runAll();
}
