#include "test_framework.h"

#include "fastwinding/mesh_object.h"
#include "fastwinding/solver.h"

void test_WindingSolver_TetrahedronInside();
namespace {
    struct Reg_WindingSolver_TetrahedronInside {
        Reg_WindingSolver_TetrahedronInside() {
            TestFramework::TestRunner::instance().addTest("WindingSolver", "TetrahedronInside", test_WindingSolver_TetrahedronInside);
        }
    } reg_WindingSolver_TetrahedronInside;
}
void test_WindingSolver_TetrahedronInside()
{
    fastwinding::MeshObject mesh;
    mesh.vertices = {
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    };
    mesh.faces = {
        {0, 2, 1},
        {0, 1, 3},
        {0, 3, 2},
        {1, 2, 3}
    };

    ASSERT_TRUE(mesh.isValid());

    fastwinding::FastWindingSolver solver;
    ASSERT_TRUE(solver.init(mesh));

    std::array<float, 3> q = {0.1f, 0.1f, 0.1f};
    float wn = solver.compute(q);
    ASSERT_FLOAT_EQ(wn, 1.0f, 0.05f);
}

void test_WindingSolver_TetrahedronOutside();
namespace {
    struct Reg_WindingSolver_TetrahedronOutside {
        Reg_WindingSolver_TetrahedronOutside() {
            TestFramework::TestRunner::instance().addTest("WindingSolver", "TetrahedronOutside", test_WindingSolver_TetrahedronOutside);
        }
    } reg_WindingSolver_TetrahedronOutside;
}
void test_WindingSolver_TetrahedronOutside()
{
    fastwinding::MeshObject mesh;
    mesh.vertices = {
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    };
    mesh.faces = {
        {0, 2, 1},
        {0, 1, 3},
        {0, 3, 2},
        {1, 2, 3}
    };

    fastwinding::FastWindingSolver solver;
    ASSERT_TRUE(solver.init(mesh));

    std::array<float, 3> q = {2.0f, 2.0f, 2.0f};
    float wn = solver.compute(q);
    ASSERT_FLOAT_EQ(wn, 0.0f, 0.05f);
}

void test_WindingSolver_BatchQuery();
namespace {
    struct Reg_WindingSolver_BatchQuery {
        Reg_WindingSolver_BatchQuery() {
            TestFramework::TestRunner::instance().addTest("WindingSolver", "BatchQuery", test_WindingSolver_BatchQuery);
        }
    } reg_WindingSolver_BatchQuery;
}
void test_WindingSolver_BatchQuery()
{
    fastwinding::MeshObject mesh;
    mesh.vertices = {
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    };
    mesh.faces = {
        {0, 2, 1},
        {0, 1, 3},
        {0, 3, 2},
        {1, 2, 3}
    };

    fastwinding::FastWindingSolver solver;
    ASSERT_TRUE(solver.init(mesh));

    std::vector<std::array<float, 3>> queries = {
        {0.1f, 0.1f, 0.1f},
        {2.0f, 2.0f, 2.0f}
    };

    std::vector<float> results = solver.computeBatch(queries);
    ASSERT_EQ(results.size(), 2u);
    ASSERT_FLOAT_EQ(results[0], 1.0f, 0.05f);
    ASSERT_FLOAT_EQ(results[1], 0.0f, 0.05f);
}

void test_WindingSolver_InvalidMesh();
namespace {
    struct Reg_WindingSolver_InvalidMesh {
        Reg_WindingSolver_InvalidMesh() {
            TestFramework::TestRunner::instance().addTest("WindingSolver", "InvalidMesh", test_WindingSolver_InvalidMesh);
        }
    } reg_WindingSolver_InvalidMesh;
}
void test_WindingSolver_InvalidMesh()
{
    fastwinding::MeshObject empty;
    fastwinding::FastWindingSolver solver;
    ASSERT_FALSE(solver.init(empty));
}

void test_WindingSolver_MoveSemantics();
namespace {
    struct Reg_WindingSolver_MoveSemantics {
        Reg_WindingSolver_MoveSemantics() {
            TestFramework::TestRunner::instance().addTest("WindingSolver", "MoveSemantics", test_WindingSolver_MoveSemantics);
        }
    } reg_WindingSolver_MoveSemantics;
}
void test_WindingSolver_MoveSemantics()
{
    fastwinding::MeshObject mesh;
    mesh.vertices = {
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    };
    mesh.faces = {
        {0, 2, 1},
        {0, 1, 3},
        {0, 3, 2},
        {1, 2, 3}
    };

    fastwinding::FastWindingSolver a;
    ASSERT_TRUE(a.init(mesh));
    fastwinding::FastWindingSolver b(std::move(a));

    std::array<float, 3> q = {0.1f, 0.1f, 0.1f};
    float wn = b.compute(q);
    ASSERT_FLOAT_EQ(wn, 1.0f, 0.05f);
}

int main() {
    return TestFramework::TestRunner::instance().runAll();
}
