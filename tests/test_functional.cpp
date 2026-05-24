#include "test_framework.h"

#include <Eigen/Core>
#include <igl/read_triangle_mesh.h>
#include <igl/readDMAT.h>
#include <igl/writeDMAT.h>

#include "../WindingNumber/UT_SolidAngle.h"

#include <numbers>
#include <fstream>
#include <cmath>

TEST(Functional, LoadPigHeadMesh)
{
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    bool success = igl::read_triangle_mesh("pig-head.ply", V, F);

    ASSERT_TRUE(success);
    ASSERT_TRUE(V.rows() > 0);
    ASSERT_TRUE(F.rows() > 0);
    ASSERT_EQ(V.cols(), 3);
    ASSERT_EQ(F.cols(), 3);
}

TEST(Functional, LoadQueryPoints)
{
    Eigen::MatrixXf P;
    bool success = igl::readDMAT("pig-head-Q.dmat", P);

    ASSERT_TRUE(success);
    ASSERT_TRUE(P.rows() > 0);
    ASSERT_EQ(P.cols(), 3);
}

TEST(Functional, SolidAngleComputation)
{
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh("pig-head.ply", V, F);

    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    std::vector<HDK_Sample::UT_Vector3T<float>> U(V.rows());
    for (int i = 0; i < V.rows(); i++) {
        U[i][0] = V(i, 0);
        U[i][1] = V(i, 1);
        U[i][2] = V(i, 2);
    }

    solid_angle.init(F.rows(), F.data(), V.rows(), U.data(), 2);

    Eigen::VectorXf W(10);
    float accuracy_scale = 2.0;

    for (int i = 0; i < 10; i++) {
        HDK_Sample::UT_Vector3T<float> Pp;
        Pp[0] = static_cast<float>(i) * 0.1f;
        Pp[1] = 0.0f;
        Pp[2] = 2.0f;

        float solid = solid_angle.computeSolidAngle(Pp, accuracy_scale);
        W(i) = solid / (4.0f * static_cast<float>(std::numbers::pi));
    }

    for (int i = 0; i < 10; i++) {
        ASSERT_TRUE(W(i) >= 0.0f && W(i) <= 1.0f);
    }
}

TEST(Functional, WindingNumberConsistency)
{
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh("pig-head.ply", V, F);

    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    std::vector<HDK_Sample::UT_Vector3T<float>> U(V.rows());
    for (int i = 0; i < V.rows(); i++) {
        U[i][0] = V(i, 0);
        U[i][1] = V(i, 1);
        U[i][2] = V(i, 2);
    }

    solid_angle.init(F.rows(), F.data(), V.rows(), U.data(), 2);

    float accuracy_scale = 2.0;

    HDK_Sample::UT_Vector3T<float> center;
    center[0] = 0.0f; center[1] = 0.0f; center[2] = 0.0f;
    for (int i = 0; i < V.rows(); i++) {
        center[0] += V(i, 0);
        center[1] += V(i, 1);
        center[2] += V(i, 2);
    }
    center[0] /= V.rows();
    center[1] /= V.rows();
    center[2] /= V.rows();

    float solid = solid_angle.computeSolidAngle(center, accuracy_scale);
    float wn = solid / (4.0f * static_cast<float>(std::numbers::pi));

    ASSERT_TRUE(wn > 0.0f);
}

TEST(Functional, ParallelComputation)
{
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh("pig-head.ply", V, F);

    Eigen::MatrixXf P;
    igl::readDMAT("pig-head-Q.dmat", P);

    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    std::vector<HDK_Sample::UT_Vector3T<float>> U(V.rows());
    for (int i = 0; i < V.rows(); i++) {
        U[i][0] = V(i, 0);
        U[i][1] = V(i, 1);
        U[i][2] = V(i, 2);
    }

    solid_angle.init(F.rows(), F.data(), V.rows(), U.data(), 2);

    const int batch_size = 1000;
    int num_batches = (P.rows() + batch_size - 1) / batch_size;

    Eigen::VectorXf W(P.rows());
    float accuracy_scale = 2.0;

    for (int batch = 0; batch < num_batches; batch++) {
        int start = batch * batch_size;
        int end = std::min(start + batch_size, static_cast<int>(P.rows()));

        for (int i = start; i < end; i++) {
            HDK_Sample::UT_Vector3T<float> Pp;
            Pp[0] = P(i, 0);
            Pp[1] = P(i, 1);
            Pp[2] = P(i, 2);
            W(i) = solid_angle.computeSolidAngle(Pp, accuracy_scale) / (4.0f * static_cast<float>(std::numbers::pi));
        }
    }

    int inside_count = 0;
    for (int i = 0; i < P.rows(); i++) {
        if (W(i) > 0.99f) inside_count++;
    }

    ASSERT_TRUE(inside_count > 0);
}

TEST(Functional, SaveOutputDMAT)
{
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh("pig-head.ply", V, F);

    Eigen::VectorXf W(3);
    W(0) = 0.0f;
    W(1) = 0.5f;
    W(2) = 1.0f;

    bool success = igl::writeDMAT("test_output.dmat", W, false);
    ASSERT_TRUE(success);

    std::ifstream file("test_output.dmat");
    ASSERT_TRUE(file.good());
    file.close();

    std::remove("test_output.dmat");
}

TEST(Functional, MeshProperties)
{
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh("pig-head.ply", V, F);

    float min_x = V.col(0).minCoeff();
    float max_x = V.col(0).maxCoeff();
    float min_y = V.col(1).minCoeff();
    float max_y = V.col(1).maxCoeff();
    float min_z = V.col(2).minCoeff();
    float max_z = V.col(2).maxCoeff();

    float expected_min = -0.5f;
    float expected_max = 0.5f;

    ASSERT_TRUE(min_x > expected_min - 1.0f);
    ASSERT_TRUE(max_x < expected_max + 1.0f);
    ASSERT_TRUE(min_y > expected_min - 1.0f);
    ASSERT_TRUE(max_y < expected_max + 1.0f);
    ASSERT_TRUE(min_z > expected_min - 1.0f);
    ASSERT_TRUE(max_z < expected_max + 1.0f);
}

TEST(Functional, AccuracyScaleEffect)
{
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh("pig-head.ply", V, F);

    HDK_Sample::UT_SolidAngle<float, float> solid_angle;

    std::vector<HDK_Sample::UT_Vector3T<float>> U(V.rows());
    for (int i = 0; i < V.rows(); i++) {
        U[i][0] = V(i, 0);
        U[i][1] = V(i, 1);
        U[i][2] = V(i, 2);
    }

    solid_angle.init(F.rows(), F.data(), V.rows(), U.data(), 2);

    HDK_Sample::UT_Vector3T<float> query;
    query[0] = 0.0f; query[1] = 0.0f; query[2] = 0.0f;

    float solid_scale1 = solid_angle.computeSolidAngle(query, 1.0);
    float solid_scale2 = solid_angle.computeSolidAngle(query, 2.0);
    float solid_scale3 = solid_angle.computeSolidAngle(query, 4.0);

    ASSERT_TRUE(solid_scale1 > 0.0f);
    ASSERT_TRUE(solid_scale2 > 0.0f);
    ASSERT_TRUE(solid_scale3 > 0.0f);
}

int main() {
    return TestFramework::TestRunner::instance().runAll();
}
