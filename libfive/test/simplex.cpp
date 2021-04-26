/*
libfive: a CAD kernel for modeling with implicit functions
Copyright (C) 2018  Matt Keeter

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "catch.hpp"

#include "libfive/render/brep/simplex/simplex_worker_pool.hpp"
#include "libfive/render/brep/simplex/simplex_mesher.hpp"
#include "libfive/render/brep/simplex/simplex_debug.hpp"
#include "libfive/render/brep/indexes.hpp"
#include "libfive/render/brep/dual.hpp"
#include "libfive/render/brep/settings.hpp"
#include "libfive/eval/evaluator.hpp"

#include "util/shapes.hpp"
#include "util/mesh_checks.hpp"

#include <set>

using namespace libfive;

TEST_CASE("SimplexPool")
{
    auto c = circle(1);
    auto r = Region<2>({-1, -1}, {1, 1});

    BRepSettings settings;
    settings.min_feature = 0.1;
    settings.workers = 1;
    auto t = SimplexWorkerPool<2>::build(c, r, settings);
    REQUIRE(t.get() != nullptr);

    t->assignIndices(settings);
}

TEST_CASE("SimplexTree<2>::assignIndices")
{
    auto c = circle(1);
    auto r = Region<2>({-1, -1}, {1, 1});

    BRepSettings settings;
    settings.min_feature = 1.1;
    settings.workers = 1;
    auto t = SimplexWorkerPool<2>::build(c, r, settings);
    REQUIRE(t.get() != nullptr);

    t->assignIndices(settings);

    REQUIRE(t->isBranch());
    std::set<uint64_t> indices;
    for (auto& c : t->children) {
        CAPTURE(c.load()->region.lower);
        CAPTURE(c.load()->region.upper);

        REQUIRE(c.load() != nullptr);
        REQUIRE(!c.load()->isBranch());
        REQUIRE(c.load()->leaf != nullptr);
        for (auto& i : c.load()->leaf->sub)
        {
            indices.insert(i.load()->index.load());
        }
    }
    REQUIRE(indices.size() == 25);
    REQUIRE(*indices.begin() == 1);
    REQUIRE(*indices.rbegin() == 25);
}

TEST_CASE("SimplexTree<2>: cell collapsing")
{
    SECTION("Rotated box") {
        auto shape = rotate2d(rectangle(-0.5, -0.5, 0.6, 0.6),
                              M_PI / 4);
        auto r = Region<2>({-1, -1}, {1, 1});

        BRepSettings settings;
        settings.min_feature = 0.4;
        settings.workers = 1;
        auto t = SimplexWorkerPool<2>::build(shape, r, settings);
        REQUIRE(!t->isBranch());
    }
}

TEST_CASE("SimplexTree<3>: types")
{
    auto c = sphere(0.5);
    auto r = Region<3>({-1, -1, -1}, {1, 1, 1});

    auto t = SimplexWorkerPool<3>::build(c, r, BRepSettings());
    REQUIRE(t.get() != nullptr);

    REQUIRE(t->isBranch());
    for (auto& c : t->children) {
        REQUIRE(c.load()->type == Interval::AMBIGUOUS);
    }
}

void CHECK_CORNER_POSITIONS(const SimplexTree<3>* ptr, Region<3> r)
{
    std::list<std::pair<const SimplexTree<3>*, Region<3>>> todo;
    todo.push_back({ptr, r});

    unsigned checked_count = 0;
    while (todo.size())
    {
        auto task = todo.front();
        todo.pop_front();

        if (task.first->isBranch())
        {
            auto rs = task.second.subdivide();
            for (unsigned i=0; i < 8; ++i) {
                todo.push_back({task.first->children[i].load(), rs[i]});
            }
        }
        else if (task.first->leaf)
        {
            for (unsigned i=0; i < 8; ++i) {
                Eigen::Vector3d vt =
                    task.first->leaf->sub[CornerIndex(i).neighbor().i].load()
                                    ->vert;
                Eigen::Vector3d vr = task.second.corner(i);
                CAPTURE(vt);
                CAPTURE(vr);
                REQUIRE((vt - vr).norm() < 1e-6);
                checked_count++;
            }
        }
    }
    REQUIRE(checked_count > 0);
}

TEST_CASE("SimplexTree<3>: Corner positions")
{
    SECTION("Single-cell sphere")
    {
        auto c = sphere(0.5);
        auto r = Region<3>({-1, -1, -1}, {1, 1, 1});

        BRepSettings settings;
        settings.min_feature = 2;
        auto t = SimplexWorkerPool<3>::build(c, r, settings);

        REQUIRE(t.get() != nullptr);
        REQUIRE(t->type == Interval::AMBIGUOUS);
        REQUIRE(!t->isBranch());
        REQUIRE(t->leaf != nullptr);

        for (unsigned i=0; i < 26; ++i) {
            REQUIRE(t->leaf->sub[i].load()->inside == false);
        }
        REQUIRE(t->leaf->sub[26].load()->inside == true);

        REQUIRE(t->leaf->sub[0].load()->vert ==  Eigen::RowVector3d(-1, -1, -1));
        REQUIRE(t->leaf->sub[1].load()->vert ==  Eigen::RowVector3d( 1, -1, -1));
        REQUIRE(t->leaf->sub[2].load()->vert ==  Eigen::RowVector3d( 0, -1, -1));

        REQUIRE(t->leaf->sub[3].load()->vert ==  Eigen::RowVector3d(-1,  1, -1));
        REQUIRE(t->leaf->sub[4].load()->vert ==  Eigen::RowVector3d( 1,  1, -1));
        REQUIRE(t->leaf->sub[5].load()->vert ==  Eigen::RowVector3d( 0,  1, -1));

        REQUIRE(t->leaf->sub[6].load()->vert ==  Eigen::RowVector3d(-1,  0, -1));
        REQUIRE(t->leaf->sub[7].load()->vert ==  Eigen::RowVector3d( 1,  0, -1));
        REQUIRE(t->leaf->sub[8].load()->vert ==  Eigen::RowVector3d( 0,  0, -1));

        REQUIRE(t->leaf->sub[9].load()->vert ==  Eigen::RowVector3d(-1, -1,  1));
        REQUIRE(t->leaf->sub[10].load()->vert == Eigen::RowVector3d( 1, -1,  1));
        REQUIRE(t->leaf->sub[11].load()->vert == Eigen::RowVector3d( 0, -1,  1));

        REQUIRE(t->leaf->sub[12].load()->vert == Eigen::RowVector3d(-1,  1,  1));
        REQUIRE(t->leaf->sub[13].load()->vert == Eigen::RowVector3d( 1,  1,  1));
        REQUIRE(t->leaf->sub[14].load()->vert == Eigen::RowVector3d( 0,  1,  1));

        REQUIRE(t->leaf->sub[15].load()->vert == Eigen::RowVector3d(-1,  0,  1));
        REQUIRE(t->leaf->sub[16].load()->vert == Eigen::RowVector3d( 1,  0,  1));
        REQUIRE(t->leaf->sub[17].load()->vert == Eigen::RowVector3d( 0,  0,  1));

        REQUIRE(t->leaf->sub[18].load()->vert == Eigen::RowVector3d(-1, -1,  0));
        REQUIRE(t->leaf->sub[19].load()->vert == Eigen::RowVector3d( 1, -1,  0));
        REQUIRE(t->leaf->sub[20].load()->vert == Eigen::RowVector3d( 0, -1,  0));

        REQUIRE(t->leaf->sub[21].load()->vert == Eigen::RowVector3d(-1,  1,  0));
        REQUIRE(t->leaf->sub[22].load()->vert == Eigen::RowVector3d( 1,  1,  0));
        REQUIRE(t->leaf->sub[23].load()->vert == Eigen::RowVector3d( 0,  1,  0));

        REQUIRE(t->leaf->sub[24].load()->vert == Eigen::RowVector3d(-1,  0,  0));
        REQUIRE(t->leaf->sub[25].load()->vert == Eigen::RowVector3d( 1,  0,  0));
        REQUIRE(t->leaf->sub[26].load()->vert == Eigen::RowVector3d( 0,  0,  0));
    }

    SECTION("Sphere")
    {
        auto c = sphere(0.5);
        auto r = Region<3>({-1, -1, -1}, {1, 1, 1});

        auto t = SimplexWorkerPool<3>::build(c, r, BRepSettings());
        REQUIRE(t.get() != nullptr);
        CHECK_CORNER_POSITIONS(t.get(), r);
    }

    SECTION("Box (low-resolution)")
    {
        auto c = box({-0.4f, -0.4f, -0.4f}, {0.4f, 0.4f, 0.4f});
        auto r = Region<3>({-1, -1, -1}, {1, 1, 1});

        BRepSettings settings;
        settings.min_feature = 0.4;
        settings.max_err = 0;
        settings.workers = 1;
        auto t = SimplexWorkerPool<3>::build(c, r, settings);
        CHECK_CORNER_POSITIONS(t.get(), r);
    }

}

TEST_CASE("SimplexMesher<3>: box with problematic edges")
{
    auto shape = box({-1, -1, -1}, {1.1f, 1.1f, 1.1f}).optimized();
    auto r = Region<3>({-2, -2, -2}, {2, 2, 2});

    // Build a tree without any collapsing
    BRepSettings settings;
    settings.min_feature = 1;
    settings.max_err = -1;
    auto t = SimplexWorkerPool<3>::build(shape, r, settings);
    CHECK_CORNER_POSITIONS(t.get(), r);

    t->assignIndices(settings);

    settings.workers = 8;
    auto m = Dual<3>::walk<SimplexMesher>(t, settings, shape);

    REQUIRE(m->branes.size() > 0);
    REQUIRE(m->verts.size() > 1);

    Evaluator eval(shape);
    for (auto& tri: m->branes) {
        auto a = m->verts[tri[0]];
        auto b = m->verts[tri[1]];
        auto c = m->verts[tri[2]];

        CAPTURE(a);
        CAPTURE(b);
        CAPTURE(c);

        REQUIRE(eval.value(a.template cast<float>()) == Approx(0.0).margin(1e-4));
        REQUIRE(eval.value(b.template cast<float>()) == Approx(0.0).margin(1e-4));
        REQUIRE(eval.value(c.template cast<float>()) == Approx(0.0).margin(1e-4));

        REQUIRE(eval.value(((a + b) / 2).template cast<float>()) == Approx(0.0).margin(1e-4));
        REQUIRE(eval.value(((b + c) / 2).template cast<float>()) == Approx(0.0).margin(1e-4));
        REQUIRE(eval.value(((a + c) / 2).template cast<float>()) == Approx(0.0).margin(1e-4));
    }
}

TEST_CASE("SimplexMesher<3>: tricky shape")
{
    auto b = max(box({-1, 0, -1}, {1, 2, 1}),
                -box({0, 0, -1}, {1, 2, 0})).optimized();

    Region<3> r({-5, -5, -5}, {5, 5, 5});

    BRepSettings settings;
    settings.min_feature = 0.25;
    auto t = SimplexWorkerPool<3>::build(b, r, settings);
    t->assignIndices(settings);

    auto m = Dual<3>::walk<SimplexMesher>(t, settings, b);
    CHECK_EDGE_PAIRS(*m);
}

using SimplexVertexMap = std::map<std::pair<const SimplexTree<3>*, unsigned>,
                                  std::pair<Eigen::Vector3d, bool>>;
void buildVertexMap(const SimplexTree<3>* t, SimplexVertexMap& out)
{
    if (t->isBranch()) {
        for (const auto& c: t->children) {
            buildVertexMap(c.load(), out);
        }
    } else if (t->leaf) {
        for (unsigned i=0; i < ipow(3, 3); ++i) {
            out.insert({{t, i}, {t->leaf->sub[i].load()->vert,
                                 t->leaf->sub[i].load()->inside}});
        }
    }
}

/*  Centers vertices within their respective subspaces.
 *
 *  This isn't used in normal operation, but paired with
 *  SimplexDebugMesher, can be helpful to see where subdivision
 *  topologies are broken. */
void debugCenterVertices(const SimplexTree<3>* t) {
    if (t->isBranch()) {
        for (const auto& c: t->children) {
            debugCenterVertices(c.load());
        }
    } else if (t->leaf) {
        for (unsigned i_=0; i_ < ipow(3, 3); ++i_) {
            const auto i = NeighborIndex(i_);

            // Find the average position of corners within this subspace
            Eigen::Vector3d start = Eigen::Vector3d::Zero();
            unsigned count = 0;
            for (unsigned j=0; j < ipow(2, 3); ++j) {
                if (i.contains(CornerIndex(j))) {
                    start += t->region.corner(j);
                    count++;
                }
            }

            const auto sub = t->leaf->sub[i_].load();
            sub->vert = start / count;
        }
    }
}

TEST_CASE("SimplexTree<3>: assignIndices with cell collapsing")
{
    auto b = max(box({-1, 0, -1}, {1, 2, 1}),
                -box({0, -1, -0.5}, {1, 2.5, 1}));

    Region<3> r({-5, -5, -5}, {5, 5, 5});

    BRepSettings settings;
    settings.min_feature = 0.5;
    settings.workers = 1;
    auto t = SimplexWorkerPool<3>::build(b, r, settings);
    SimplexVertexMap before;
    buildVertexMap(t.get(), before);

    t->assignIndices(settings);
    SimplexVertexMap after;
    buildVertexMap(t.get(), after);

    for (auto& k : before) {
        REQUIRE(after.count(k.first) != 0);
        const auto a = after.at(k.first);
        const auto b = k.second;
        CAPTURE(a.first);
        CAPTURE(b.first);
        CAPTURE(k.first.first->region.lower.transpose());
        CAPTURE(k.first.first->region.upper.transpose());
        CAPTURE((a.first - b.first).norm());
        REQUIRE((a.first - b.first).norm() < 1e-9);
        REQUIRE(a.second == b.second);
    }
}

TEST_CASE("SimplexMesher<3>: cell collapsing and vertex placement")
{
    auto b = max(box({-1, 0, -1}, {1, 2, 1}),
                -box({0, -1, -0.5}, {1, 2.5, 1})).optimized();

    Region<3> r({-5, -5, -5}, {5, 5, 5});

    BRepSettings settings;
    settings.min_feature = 0.5;
    settings.workers = 1;
    auto t = SimplexWorkerPool<3>::build(b, r, settings);
    t->assignIndices(settings);

    auto m = Dual<3>::walk<SimplexMesher>(t, settings, b);
    CHECK_EDGE_PAIRS(*m);
}

TEST_CASE("SimplexMesher<3>: sphere")
{
    auto s = sphere(1).optimized();
    Region<3> r({-2, -2, -2}, {2, 2, 2});

    BRepSettings settings;
    settings.min_feature = 5;
    settings.workers = 1;
    auto t = SimplexWorkerPool<3>::build(s, r, settings);
    t->assignIndices(settings);

    auto m = Dual<3>::walk<SimplexMesher>(t, settings, s);
    CHECK_EDGE_PAIRS(*m);
    m->saveSTL("out.stl");

    auto g = Dual<3>::walk<SimplexDebugMesher>(t, settings, s);
    g->saveSTL("grid.stl");
}

TEST_CASE("SimplexMesher<3>: cylinder meshing")
{
    auto b = extrude(circle(1, {0, 0}), -1, 1).optimized();

    Region<3> r({-5, -5, -5}, {5, 5, 5});

    BRepSettings settings;
    settings.min_feature = 5;
    settings.workers = 1;
    auto t = SimplexWorkerPool<3>::build(b, r, settings);
    t->assignIndices(settings);

    auto m = Dual<3>::walk<SimplexMesher>(t, settings, b);
    CHECK_EDGE_PAIRS(*m);
    m->saveSTL("out.stl");

    auto g = Dual<3>::walk<SimplexDebugMesher>(t, settings, b);
    g->saveSTL("grid.stl");
}

TEST_CASE("SimplexTree<3>: vertex placement in centered cylinder")
{
    auto b = extrude(circle(1, {0, 0}), -1, 1);

    Region<3> r({-5, -5, -5}, {5, 5, 5});

    BRepSettings settings;
    settings.min_feature = 5;
    settings.workers = 1;
    auto t = SimplexWorkerPool<3>::build(b, r, settings);

    // Check that every XY plane has a vertex placed at X = Y
    // (since this is a symmetric model)
    for (unsigned i=0; i < 4; ++i) {
        const auto v = t.get()->children[i].load()->leaf->sub[8].load()->vert;
        CAPTURE(v);
        CAPTURE(t.get()->children[i].load()->type);
        REQUIRE(fabs(v.x()) == Approx(fabs(v.y())));

        const auto w = t.get()->children[4 + i].load()->leaf->sub[17].load()->vert;
        CAPTURE(w);
        REQUIRE(fabs(w.x()) == Approx(fabs(w.y())));
    }
}

TEST_CASE("SimplexTree<3>: meshing + cell collapsing")
{
    auto c = box({-3.1f, -3.1f, -3.1f}, {3.1f, 3.1f, 3.1f}).optimized();
    Region<3> r({-4, -4, -4}, {4, 4, 4});

    BRepSettings settings;
    settings.min_feature = 0.9;
    auto t = SimplexWorkerPool<3>::build(c, r, settings);
    t->assignIndices(settings);

    auto m = Dual<3>::walk<SimplexMesher>(t, settings, c);

    REQUIRE(m->branes.size() > 0);
    REQUIRE(m->verts.size() > 1);
    CHECK_EDGE_PAIRS(*m);
}

TEST_CASE("SimplexTree<3>::assignIndices")
{
    SECTION("Sphere") {
        auto c = sphere(0.5);
        auto r = Region<3>({-1, -1, -1}, {1, 1, 1});

        BRepSettings settings;
        settings.min_feature = 1.1;
        settings.workers = 1;
        auto t = SimplexWorkerPool<3>::build(c, r, settings);
        REQUIRE(t.get() != nullptr);

        t->assignIndices(settings);

        REQUIRE(t->isBranch());
        std::set<uint64_t> indices;
        for (auto& c : t->children) {
            REQUIRE(c.load() != nullptr);
            REQUIRE(!c.load()->isBranch());
            REQUIRE(c.load()->leaf != nullptr);
            for (auto& i : c.load()->leaf->sub)
            {
                indices.insert(i.load()->index.load());
            }
        }
        REQUIRE(indices.size() == 125);
        REQUIRE(*indices.begin() == 1);
        REQUIRE(*indices.rbegin() == 125);
    }


    SECTION("Sphere-cube intersection") {
        auto c = min(sphere(0.7, {0, 0, 0.1f}), box({-1, -1, -1}, {1, 1, 0.1f}));
        Region<3> r({-10, -10, -10}, {10, 10, 10});

        BRepSettings settings;
        settings.min_feature = 1;
        settings.workers = 1;
        auto t = SimplexWorkerPool<3>::build(c, r, settings);
        t->assignIndices(settings);

        std::list<const SimplexTree<3>*> todo;
        todo.push_back(t.get());

        while (todo.size()) {
            const auto next = todo.front();
            todo.pop_front();

            if (next->isBranch()) {
                for (const auto& c: next->children) {
                    todo.push_back(c.load());
                }
            } else if (next->leaf) {
                for (const auto& sub: next->leaf->sub) {
                    REQUIRE(sub.load()->index.load() != 0);
                }
            }
        }
    }
}

TEST_CASE("SimplexTree<3>::leafLevel")
{
    auto c = sphere(0.5);
    auto r = Region<3>({-1, -1, -1}, {1, 1, 1});

    BRepSettings settings;
    settings.min_feature = 1.1;
    settings.workers = 1;
    auto t = SimplexWorkerPool<3>::build(c, r, settings);
    REQUIRE(t->isBranch());
    for (auto& c : t->children) {
        REQUIRE(c.load()->leafLevel() == 0);
    }
}

TEST_CASE("SimplexMesher (smoke test)")
{
    SECTION("High-resolution sphere")
    {
        auto r = Region<3>({-1, -1, -1}, {1, 1, 1});
        auto c = sphere(0.5).optimized();

        BRepSettings settings;
        settings.min_feature = 1.1;
        settings.workers = 1;
        auto t = SimplexWorkerPool<3>::build(c, r, settings);
        REQUIRE(t->isBranch());
        for (auto& c : t->children) {
            REQUIRE(c.load()->type == Interval::AMBIGUOUS);
        }
        t->assignIndices(settings);

        auto m = Dual<3>::walk<SimplexMesher>(t, settings, c);

        REQUIRE(m->branes.size() > 0);
        REQUIRE(m->verts.size() > 1);
    }

    SECTION("Low-resolution sphere")
    {
        auto r = Region<3>({-1, -1, -1}, {1, 1, 1});
        auto c = sphere(0.1).optimized();

        BRepSettings settings;
        settings.min_feature = 1;
        auto t = SimplexWorkerPool<3>::build(c, r, settings);
        t->assignIndices(settings);

        auto m = Dual<3>::walk<SimplexMesher>(t, settings, c);
        REQUIRE(m->verts.size() > 1);
        REQUIRE(m->branes.size() > 1);
    }

    SECTION("Low-resolution box")
    {
        auto c = box({-0.4f, -0.4f, -0.4f}, {0.4f, 0.4f, 0.4f}).optimized();
        auto r = Region<3>({-1, -1, -1}, {1, 1, 1});

        BRepSettings settings;
        settings.min_feature = 0.4;
        settings.max_err = 0;
        settings.workers = 1;
        auto t = SimplexWorkerPool<3>::build(c, r, settings);
        t->assignIndices(settings);

        settings.workers = 8;
        auto m = Dual<3>::walk<SimplexMesher>(t, settings, c);
        REQUIRE(m->verts.size() > 1);
        REQUIRE(m->branes.size() > 1);
    }
}

TEST_CASE("SimplexMesher: sphere-box intersection vertex placement")
{
    auto c = min(sphere(0.7, {0, 0, 0.1f}), box({-1, -1, -1}, {1, 1, 0.1f}))
        .optimized();
    Region<3> r({-10, -10, -10}, {10, 10, 10});

    BRepSettings settings;
    settings.min_feature = 0.2;
    auto t = SimplexWorkerPool<3>::build(c, r, settings);
    t->assignIndices(settings);

    auto m = Dual<3>::walk<SimplexMesher>(t, settings, c);

    // We pick out any triangle that's at the sphere-cube intersection,
    // and check that the intersection vertices are at the sphere's radius.
    auto is_top_surface = [&](uint32_t i) {
        const auto z = m->verts[i].z();
        return z > 0.099 && z < 0.101;
    };
    auto is_above_surface = [&](uint32_t i) {
        return m->verts[i].z() > 0.101;
    };
    auto is_intersection_triangle = [&](Eigen::Matrix<uint32_t, 3, 1> tri)
    {
        return (is_top_surface(tri[0]) ||
                is_top_surface(tri[1]) ||
                is_top_surface(tri[2])) &&
               (is_above_surface(tri[0]) ||
                is_above_surface(tri[1]) ||
                is_above_surface(tri[2]));
    };
    for (auto& tri : m->branes) {
        if (is_intersection_triangle(tri)) {
            for (unsigned i=0; i < 3; ++i) {
                if (is_top_surface(tri[i])) {
                    CAPTURE(m->verts[tri[i]]);
                    REQUIRE(m->verts[tri[i]].head<2>().norm() ==
                            Approx(0.7).margin(0.01));
                }
            }
        }
    }
}

TEST_CASE("SimplexMesher: edge pairing")
{
    const unsigned workers = 8;

    SECTION("Sphere (low resolution)")
    {
        auto c = sphere(0.4).optimized();
        auto r = Region<3>({-1, -1, -1}, {1, 1, 1});

        BRepSettings settings;
        settings.min_feature = 1.1;
        settings.max_err = 0;
        settings.workers = 1;
        auto t = SimplexWorkerPool<3>::build(c, r, settings);
        REQUIRE(t->isBranch());
        for (auto& c : t->children) {
            REQUIRE(!c.load()->isBranch());
            REQUIRE(c.load()->type == Interval::AMBIGUOUS);
        }
        t->assignIndices(settings);

        settings.workers = workers;
        auto m = Dual<3>::walk<SimplexMesher>(t, settings, c);

        CHECK_EDGE_PAIRS(*m);
    }

    SECTION("Sphere (higher-resolution)")
    {
        auto c = sphere(0.4).optimized();
        auto r = Region<3>({-1, -1, -1}, {1, 1, 1});

        BRepSettings settings;
        settings.min_feature = 0.4;
        settings.max_err = 0;
        settings.workers = 1;
        auto t = SimplexWorkerPool<3>::build(c, r, settings);
        t->assignIndices(settings);

        settings.workers = workers;
        auto m = Dual<3>::walk<SimplexMesher>(t, settings, c);

        CHECK_EDGE_PAIRS(*m);
    }

    SECTION("Box (low-resolution)")
    {
        auto c = box({-0.4f, -0.4f, -0.4f}, {0.4f, 0.4f, 0.4f}).optimized();
        auto r = Region<3>({-1, -1, -1}, {1, 1, 1});

        BRepSettings settings;
        settings.min_feature = 0.4;
        settings.max_err = 0;
        settings.workers = 1;
        auto t = SimplexWorkerPool<3>::build(c, r, settings);
        t->assignIndices(settings);

        settings.workers = workers;
        auto m = Dual<3>::walk<SimplexMesher>(t, settings, c);

        CHECK_EDGE_PAIRS(*m);
    }
}

TEST_CASE("SimplexMesher: menger sponge")
{
    Tree sponge = max(menger(2), -sphere(1, {1.5, 1.5, 1.5})).optimized();
    Region<3> r({-2.5, -2.5, -2.5}, {2.5, 2.5, 2.5});

    BRepSettings settings;
    settings.min_feature = 0.1;
    auto t = SimplexWorkerPool<3>::build(sponge, r, settings);
    t->assignIndices(settings);

    auto m = Dual<3>::walk<SimplexMesher>(t, settings, sponge);
    REQUIRE(true);
}

TEST_CASE("SimplexWorkerPool: gyroid-sphere intersection vertex positions")
{
    Region<3> r({ -5, -5, -5 }, { 5, 5, 5 });

    auto s = sphereGyroid();

    BRepSettings settings;
    settings.min_feature = 0.5;
    settings.workers = 1;
    auto t = SimplexWorkerPool<3>::build(s, r, settings);

    std::list<std::pair<const SimplexTree<3>*, Region<3>>> todo;
    todo.push_back({t.get(), r});

    unsigned checked_count = 0;
    while (todo.size())
    {
        auto task = todo.front();
        todo.pop_front();

        if (task.first->isBranch())
        {
            auto rs = task.second.subdivide();
            for (unsigned i=0; i < 8; ++i) {
                todo.push_back({task.first->children[i].load(), rs[i]});
            }
        }
        else if (task.first->leaf)
        {
            for (unsigned i=0; i < 27; ++i) {
                Eigen::Vector3d vt = task.first->leaf->sub[i].load()->vert;
                CAPTURE(vt.transpose());
                CAPTURE(task.second.lower);
                CAPTURE(task.second.upper);
                REQUIRE(task.second.contains(vt));
                checked_count++;
            }
        }
    }
    REQUIRE(checked_count > 0);
}


TEST_CASE("Simplex meshing (gyroid performance breakdown)", "[!benchmark]")
{
    Region<3> r({ -5, -5, -5 }, { 5, 5, 5 });

    Root<SimplexTree<3>> t;

    auto s = sphereGyroid().optimized();

    BRepSettings settings;
    settings.min_feature = 0.05;

    BENCHMARK("SimplexTree construction")
    {
        t = SimplexWorkerPool<3>::build(s, r, settings);
    };

    BENCHMARK("Assigning indices")
    {
        t->assignIndices(settings);
    };

    std::unique_ptr<Mesh> m;
    BENCHMARK("Mesh building")
    {
        m = Dual<3>::walk<SimplexMesher>(t, settings, s);
    };

    BENCHMARK("SimplexTree deletion")
    {
        t.reset(settings);
    };

    BENCHMARK("Mesh deletion")
    {
        m.reset();
    };
}
