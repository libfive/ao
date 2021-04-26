/*
libfive: a CAD kernel for modeling with implicit functions
Copyright (C) 2018  Matt Keeter

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "catch.hpp"

#include "libfive/render/brep/indexes.hpp"

using namespace libfive;

TEST_CASE("NeighborIndex::dimension")
{
    REQUIRE(NeighborIndex(0).dimension() == 0);
    REQUIRE(NeighborIndex(1).dimension() == 0);
    REQUIRE(NeighborIndex(2).dimension() == 1);
    REQUIRE(NeighborIndex(5).dimension() == 1);
    REQUIRE(NeighborIndex(6).dimension() == 1);
    REQUIRE(NeighborIndex(8).dimension() == 2);
    REQUIRE(NeighborIndex(9).dimension() == 0);
    REQUIRE(NeighborIndex(11).dimension() == 1);
    REQUIRE(NeighborIndex(17).dimension() == 2);
    REQUIRE(NeighborIndex(18).dimension() == 1);
    REQUIRE(NeighborIndex(20).dimension() == 2);
    REQUIRE(NeighborIndex(26).dimension() == 3);
}

TEST_CASE("NeighborIndex::contains")
{
    REQUIRE(NeighborIndex(0).contains(CornerIndex(0)));

    REQUIRE(NeighborIndex(1).contains(CornerIndex(1)));
    REQUIRE(!NeighborIndex(1).contains(CornerIndex(0)));
    REQUIRE(!NeighborIndex(1).contains(CornerIndex(2)));
    REQUIRE(!NeighborIndex(1).contains(CornerIndex(3)));

    REQUIRE(NeighborIndex(2).contains(CornerIndex(0)));
    REQUIRE(NeighborIndex(2).contains(CornerIndex(1)));
    REQUIRE(!NeighborIndex(2).contains(CornerIndex(2)));
    REQUIRE(!NeighborIndex(2).contains(CornerIndex(3)));

    REQUIRE(!NeighborIndex(5).contains(CornerIndex(0)));
    REQUIRE(!NeighborIndex(5).contains(CornerIndex(1)));
    REQUIRE(NeighborIndex(5).contains(CornerIndex(2)));
    REQUIRE(NeighborIndex(5).contains(CornerIndex(3)));

    for (unsigned i=0; i < 8; ++i) {
        REQUIRE(NeighborIndex(26).contains(CornerIndex(i)));
    }
}

TEST_CASE("CornerIndex::neighbor")
{
    REQUIRE(CornerIndex(0).neighbor().i == 0);
    REQUIRE(CornerIndex(1).neighbor().i == 1);
    REQUIRE(CornerIndex(2).neighbor().i == 3);
    REQUIRE(CornerIndex(3).neighbor().i == 4);
    REQUIRE(CornerIndex(4).neighbor().i == 9);
    REQUIRE(CornerIndex(5).neighbor().i == 10);
    REQUIRE(CornerIndex(6).neighbor().i == 12);
    REQUIRE(CornerIndex(7).neighbor().i == 13);
}

TEST_CASE("NeighborIndex::operator|")
{
    REQUIRE((NeighborIndex(0) | NeighborIndex(1)).i == 2);
    REQUIRE((NeighborIndex(0)| NeighborIndex(2)).i == 2);
    REQUIRE((NeighborIndex(0)| NeighborIndex(4)).i == 8);
    REQUIRE((NeighborIndex(2)| NeighborIndex(4)).i == 8);
    REQUIRE((NeighborIndex(1)| NeighborIndex(4)).i == 7);
    REQUIRE((NeighborIndex(9)| NeighborIndex(0)).i == 18);
}


TEST_CASE("NeighborIndex::fromPosAndFloating")
{
    for (int i=0; i < pow(3, 3); ++i)
    {
        NeighborIndex n(i);
        CAPTURE(n.pos());
        CAPTURE(n.floating());
        auto m = NeighborIndex::fromPosAndFloating(n.pos(), n.floating());
        REQUIRE(i == m.i);
    }
}

TEST_CASE("NeighborIndex::isAxisFixed")
{
    REQUIRE(NeighborIndex(0).isAxisFixed(0));
    REQUIRE(NeighborIndex(0).isAxisFixed(1));
    REQUIRE(NeighborIndex(0).isAxisFixed(2));

    REQUIRE(!NeighborIndex(2).isAxisFixed(0));
    REQUIRE(NeighborIndex(2).isAxisFixed(1));
    REQUIRE(NeighborIndex(2).isAxisFixed(2));

    REQUIRE(!NeighborIndex(8).isAxisFixed(0));
    REQUIRE(!NeighborIndex(8).isAxisFixed(1));
    REQUIRE(NeighborIndex(8).isAxisFixed(2));
}
