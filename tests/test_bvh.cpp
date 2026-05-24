#include "test_framework.h"

#include "../WindingNumber/UT_BVHImpl.h"

TEST(UT_BVH, EmptyBVH)
{
    HDK_Sample::UT::BVH<4> bvh;
    ASSERT_EQ(bvh.primitiveCount(), 0);
}

TEST(UT_BVH, SingleBox)
{
    HDK_Sample::UT::BVH<4> bvh;

    HDK_Sample::UT::Box<float, 3> box;
    box.min[0] = 0.0f; box.min[1] = 0.0f; box.min[2] = 0.0f;
    box.max[0] = 1.0f; box.max[1] = 1.0f; box.max[2] = 1.0f;

    HDK_Sample::UT::Box<float, 3> boxes[1] = {box};
    unsigned int indices[1] = {0};

    bvh.init<HDK_Sample::UT::BVH_Heuristic::BOX_AREA>(boxes, 1, indices, 1, 4);

    ASSERT_EQ(bvh.primitiveCount(), 1);
}

TEST(UT_BVH, MultipleBoxes)
{
    HDK_Sample::UT::BVH<4> bvh;

    HDK_Sample::UT::Box<float, 3> boxes[3];
    boxes[0].min[0] = 0.0f; boxes[0].min[1] = 0.0f; boxes[0].min[2] = 0.0f;
    boxes[0].max[0] = 1.0f; boxes[0].max[1] = 1.0f; boxes[0].max[2] = 1.0f;
    boxes[1].min[0] = 2.0f; boxes[1].min[1] = 2.0f; boxes[1].min[2] = 2.0f;
    boxes[1].max[0] = 3.0f; boxes[1].max[1] = 3.0f; boxes[1].max[2] = 3.0f;
    boxes[2].min[0] = 4.0f; boxes[2].min[1] = 4.0f; boxes[2].min[2] = 4.0f;
    boxes[2].max[0] = 5.0f; boxes[2].max[1] = 5.0f; boxes[2].max[2] = 5.0f;

    unsigned int indices[3] = {0, 1, 2};

    bvh.init<HDK_Sample::UT::BVH_Heuristic::BOX_AREA>(boxes, 3, indices, 1, 4);

    ASSERT_EQ(bvh.primitiveCount(), 3);
}

TEST(UT_BVH, BoxOverlap)
{
    HDK_Sample::UT::Box<float, 3> a;
    a.min[0] = 0.0f; a.min[1] = 0.0f; a.min[2] = 0.0f;
    a.max[0] = 2.0f; a.max[1] = 2.0f; a.max[2] = 2.0f;

    HDK_Sample::UT::Box<float, 3> b;
    b.min[0] = 1.0f; b.min[1] = 1.0f; b.min[2] = 1.0f;
    b.max[0] = 3.0f; b.max[1] = 3.0f; b.max[2] = 3.0f;

    ASSERT_TRUE(a.overlaps(b));
}

TEST(UT_BVH, BoxNoOverlap)
{
    HDK_Sample::UT::Box<float, 3> a;
    a.min[0] = 0.0f; a.min[1] = 0.0f; a.min[2] = 0.0f;
    a.max[0] = 1.0f; a.max[1] = 1.0f; a.max[2] = 1.0f;

    HDK_Sample::UT::Box<float, 3> b;
    b.min[0] = 2.0f; b.min[1] = 2.0f; b.min[2] = 2.0f;
    b.max[0] = 3.0f; b.max[1] = 3.0f; b.max[2] = 3.0f;

    ASSERT_FALSE(a.overlaps(b));
}

TEST(UT_BVH, BoxArea)
{
    HDK_Sample::UT::Box<float, 3> box;
    box.min[0] = 0.0f; box.min[1] = 0.0f; box.min[2] = 0.0f;
    box.max[0] = 2.0f; box.max[1] = 3.0f; box.max[2] = 4.0f;

    float area = box.area();
    float expected = 2.0f * 3.0f * 2.0f + 2.0f * 4.0f * 2.0f + 2.0f * 4.0f * 3.0f;
    ASSERT_FLOAT_EQ(area, expected, 1e-5f);
}

TEST(UT_BVH, BoxCentroid)
{
    HDK_Sample::UT::Box<float, 3> box;
    box.min[0] = 0.0f; box.min[1] = 0.0f; box.min[2] = 0.0f;
    box.max[0] = 2.0f; box.max[1] = 4.0f; box.max[2] = 6.0f;

    HDK_Sample::UT::Box<float, 3>::T center[3];
    box.centroid(center);

    ASSERT_FLOAT_EQ(center[0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(center[1], 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(center[2], 3.0f, 1e-5f);
}

TEST(UT_BVH, BoxContains)
{
    HDK_Sample::UT::Box<float, 3> box;
    box.min[0] = 0.0f; box.min[1] = 0.0f; box.min[2] = 0.0f;
    box.max[0] = 2.0f; box.max[1] = 2.0f; box.max[2] = 2.0f;

    HDK_Sample::UT::Box<float, 3>::T point[3] = {1.0f, 1.0f, 1.0f};
    ASSERT_TRUE(box.contains(point));

    HDK_Sample::UT::Box<float, 3>::T outside[3] = {3.0f, 3.0f, 3.0f};
    ASSERT_FALSE(box.contains(outside));
}

TEST(UT_BVH, MergeBoxes)
{
    HDK_Sample::UT::Box<float, 3> a;
    a.min[0] = 0.0f; a.min[1] = 0.0f; a.min[2] = 0.0f;
    a.max[0] = 1.0f; a.max[1] = 1.0f; a.max[2] = 1.0f;

    HDK_Sample::UT::Box<float, 3> b;
    b.min[0] = 2.0f; b.min[1] = 2.0f; b.min[2] = 2.0f;
    b.max[0] = 3.0f; b.max[1] = 3.0f; b.max[2] = 3.0f;

    HDK_Sample::UT::Box<float, 3> merged = a;
    merged.merge(b);

    ASSERT_FLOAT_EQ(merged.min[0], 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(merged.min[1], 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(merged.min[2], 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(merged.max[0], 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(merged.max[1], 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(merged.max[2], 3.0f, 1e-5f);
}

TEST(UT_BVH, LargeBVH)
{
    HDK_Sample::UT::BVH<4> bvh;

    const int n = 100;
    std::vector<HDK_Sample::UT::Box<float, 3>> boxes(n);
    std::vector<unsigned int> indices(n);

    for (int i = 0; i < n; i++) {
        boxes[i].min[0] = static_cast<float>(i);
        boxes[i].min[1] = static_cast<float>(i);
        boxes[i].min[2] = static_cast<float>(i);
        boxes[i].max[0] = static_cast<float>(i) + 1.0f;
        boxes[i].max[1] = static_cast<float>(i) + 1.0f;
        boxes[i].max[2] = static_cast<float>(i) + 1.0f;
        indices[i] = i;
    }

    bvh.init<HDK_Sample::UT::BVH_Heuristic::BOX_AREA>(boxes.data(), n, indices.data(), 1, 4);

    ASSERT_EQ(bvh.primitiveCount(), n);
}

int main() {
    return TestFramework::TestRunner::instance().runAll();
}
