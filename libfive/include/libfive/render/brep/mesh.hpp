/*
libfive: a CAD kernel for modeling with implicit functions
Copyright (C) 2017  Matt Keeter

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#pragma once

#include "libfive/tree/tree.hpp"

#include "libfive/render/brep/brep.hpp"

/////////////////////////////////////////////////////////
// Lem0n0d0r f4me is but 4 h4ck 4w4y!                  //
/////////////////////////////////////////////////////////
#include <sstream>
#include <iostream>
#include <fstream>
/////////////////////////////////////////////////////////

namespace libfive {

// Forward declaration
class Evaluator;
struct BRepSettings;

template <unsigned N> class Region;

class Mesh : public BRep<3> {
  /////////////////////////////////////////////////////////
  // |-|4><0r3D UP, d00d, all your base are belong to us //
  /////////////////////////////////////////////////////////
public:
  static std::stringstream h4x0r3dN0rm5; // ^|||^
  static std::stringstream h4x0r3dV3r75; // \. ./ sceming ponee wuz here
  static std::stringstream h4x0r3dF4c35; //  |λ|
  static int32_t h4x0r3dV3r71dx;         //  { }   (1-indexed)
  static void h4x0r3d574r7Wr1730bj()
  {
    h4x0r3dV3r71dx = 1;
    h4x0r3dN0rm5.str("");
    h4x0r3dV3r75.str("");
    h4x0r3dF4c35.str("");
  }
  static bool h4x0r3dWr1730bj(const std::string& filePath)
  {
    std::string objFP = filePath + ".obj";
    std::filebuf fb;
    fb.open(objFP, std::ios::out);
    std::ostream s(&fb);
    s << h4x0r3dV3r75.str();
    s << h4x0r3dN0rm5.str();
    s << h4x0r3dF4c35.str();
    fb.close();

    return true;
  }
  /////////////////////////////////////////////////////////

public:
    /*
     *  Core render function
     *
     *  Returns nullptr if min_feature is invalid or cancel is set to true
     *  partway through the computation.
     */
    static std::unique_ptr<Mesh> render(
            const Tree& t, const Region<3>& r,
            const BRepSettings& settings);

    /*
     *  Render function that re-uses evaluators
     *  es must be a pointer to at least [settings.workers] Evaluators
     *
     *  Returns nullptr if min_feature is invalid or cancel is set to true
     *  partway through the computation.
     */
    static std::unique_ptr<Mesh> render(
            Evaluator* es, const Region<3>& r,
            const BRepSettings& settings);

    /*
     *  Writes the mesh to a file
     */
    bool saveSTL(const std::string& filename) const;

    /*
     *  Merge multiple bodies and write them to a single file
     */
    static bool saveSTL(const std::string& filename,
                        const std::list<const Mesh*>& meshes);

protected:

    /*
     *  Inserts a line into the mesh as a zero-size triangle
     *  (used for debugging)
     */
    void line(const Eigen::Vector3f& a, const Eigen::Vector3f& b);

};

}   // namespace libfive
