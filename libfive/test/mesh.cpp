/*
libfive: a CAD kernel for modeling with implicit functions

Copyright (C) 2017  Matt Keeter

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <chrono>

#include "catch.hpp"

#include "libfive/render/brep/mesh.hpp"
#include "libfive/render/brep/xtree_pool.hpp"
#include "libfive/render/brep/region.hpp"

#include "util/shapes.hpp"

using namespace Kernel;

TEST_CASE("Mesh::render (sphere normals)")
{
    Tree s = sphere(0.5);
    Region<3> r({-1, -1, -1}, {1, 1, 1});

    auto mesh = Mesh::render(s, r);

    float dot = 2;
    int pos = 0;
    int neg = 0;
    for (auto t : mesh->branes)
    {
        auto norm = (mesh->verts[t(1)] - mesh->verts[t(0)])
            .cross(mesh->verts[t(2)] - mesh->verts[t(0)])
            .normalized();
        auto center = ((mesh->verts[t(0)] +
                        mesh->verts[t(1)] +
                        mesh->verts[t(2)])).normalized();
        auto dot_ = norm.dot(center);
        neg += (dot_ < 0);
        pos += (dot_ > 0);
        dot = fmin(dot, dot_);
    }
    CAPTURE(neg);
    CAPTURE(pos);
    REQUIRE(dot > 0.9);
}

TEST_CASE("Mesh::render (cube)")
{
    auto cube = max(max(
                    max(-(Tree::X() + 1.5),
                          Tree::X() - 1.5),
                    max(-(Tree::Y() + 1.5),
                          Tree::Y() - 1.5)),
                    max(-(Tree::Z() + 1.5),
                          Tree::Z() - 1.5));
    Region<3> r({-2.5, -2.5, -2.5}, {2.5, 2.5, 2.5});

    auto mesh = Mesh::render(cube, r);
}

TEST_CASE("Mesh::render (face count in rectangular prism)")
{
    auto t = max(max(max(-Tree::X(), Tree::X() - 4),
                     max(-Tree::Y(), Tree::Y() - 1)),
                     max(-Tree::Z(), Tree::Z() - 0.25));
    auto m = Mesh::render(t, Region<3>({-1, -1, -1}, {5, 2, 1.25}), 0.125);
    REQUIRE(m->verts.size() == 9); // index 0 is unused
    REQUIRE(m->branes.size() == 12);
}

TEST_CASE("Mesh::render (sphere)")
{
    auto s = sphere(1);
    auto m = Mesh::render(s, Region<3>({-1.6, -1, -8}, {1.6, 1, 1}),
                          1/32.0f, pow(10, -3));
    REQUIRE(true);
}

TEST_CASE("Mesh::render (cone)")
{
    auto z = Tree::Z();
    auto s = 1 / (-z);
    auto r = sqrt(square(Tree::X() * s) + square(Tree::Y() * s));
    auto cone = max(r - 1, max(Tree::Z(), -1 - z));
    auto m = Mesh::render(cone, Region<3>({-10, -10, -10}, {10, 10, 10}), 0.1);
    REQUIRE(true);
}

////////////////////////////////////////////////////////////////////////////////

Kernel::Tree sphereGyroid()
{
    auto scale = 0.5f;
    auto radius = 1.5f;
    auto thickness = 0.5;

    auto gyroidSrf =
        sin(Kernel::Tree::X() / scale) * cos(Kernel::Tree::Y() / scale) +
        sin(Kernel::Tree::Y() / scale) * cos(Kernel::Tree::Z() / scale) +
        sin(Kernel::Tree::Z() / scale) * cos(Kernel::Tree::X() / scale);

    auto gyroid = shell(gyroidSrf, thickness);
    auto sphere1 = sphere(3.0f, { 0.f,0.f,0.f });

    auto sphereGyroid = max(sphere1, gyroid);
    sphereGyroid = min(sphereGyroid,
                     min(sphereGyroid ,
                     (sqrt(abs(sphereGyroid)) + sqrt(abs( sphereGyroid ))) - .5));

    return sphereGyroid;
}

TEST_CASE("Mesh::render (performance)", "[!benchmark]")
{
    BENCHMARK("Menger sponge")
    {
        Tree sponge = max(menger(2), -sphere(1, {1.5, 1.5, 1.5}));
        Region<3> r({-2.5, -2.5, -2.5}, {2.5, 2.5, 2.5});
        auto mesh = Mesh::render(sponge, r, 0.02);
    }

    BENCHMARK("Gradient blended-round spheres")
    {
        float blendAmt = 0.125f;

        auto boxB = box({ -2,-2,0 }, { 2,2,1 });
        auto sphereB = sphere(2.f, {2.f,2.f,0.f});
        auto blendObj = blend(boxB, sphereB, blendAmt);

        Region<3> r({ -5, -5, -5 }, { 5, 5, 5 });

        auto mesh = Mesh::render(blendObj, r, 0.025);
    }

    BENCHMARK("Sphere / gyroid intersection")
    {
        Region<3> r({ -5, -5, -5 }, { 5, 5, 5 });
        auto mesh = Mesh::render(sphereGyroid(), r, 0.025);
    }
}

TEST_CASE("Mesh::render (gyroid performance breakdown)", "[!benchmark]")
{
    Region<3> r({ -5, -5, -5 }, { 5, 5, 5 });

    XTree<3>::Root t;
    BENCHMARK("XTree construction")
    {
        t = XTreePool<3>::build(sphereGyroid(), r, 0.025, 1e-8, 8);
    }

    std::unique_ptr<Mesh> m;
    std::atomic_bool cancel(false);
    BENCHMARK("Mesh building")
    {
        m = Mesh::mesh(t, cancel);
    }

    BENCHMARK("XTree deletion")
    {
        t.reset();
    }

    BENCHMARK("Mesh deletion")
    {
        m.reset();
    }
}

TEST_CASE("Mesh::render (gyroid with progress callback)", "[!benchmark]")
{
    std::vector<float> progress;
    auto progress_callback = [&](float f)
    {
        progress.push_back(f);
    };

    Region<3> r({ -5, -5, -5 }, { 5, 5, 5 });

    XTree<3>::Root t;
    BENCHMARK("XTree construction")
    {
        t = XTreePool<3>::build(sphereGyroid(), r, 0.025, 1e-8, 8,
                                progress_callback);
    }

    std::unique_ptr<Mesh> m;
    std::atomic_bool cancel(false);
    BENCHMARK("Mesh building")
    {
        m = Mesh::mesh(t, cancel, progress_callback);
    }

    BENCHMARK("XTree deletion")
    {
        t.reset(progress_callback);
    }

    // Confirm that the progress counter is monotonically increasing
    CAPTURE(progress);
    float prev = -1;
    for (auto& p : progress)
    {
        REQUIRE(p > prev);
        prev = p;
    }
    REQUIRE(progress[0] == 0.0f);
    REQUIRE(prev == 3.0f);
}
