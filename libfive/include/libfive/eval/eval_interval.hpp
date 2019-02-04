/*
libfive: a CAD kernel for modeling with implicit functions
Copyright (C) 2017  Matt Keeter

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#pragma once

#include <Eigen/Eigen>

#include "libfive/eval/base.hpp"
#include "libfive/eval/interval.hpp"
#include "libfive/eval/clause.hpp"

namespace Kernel {
class Tape; /* Forward declaration */

class IntervalEvaluator : public BaseEvaluator
{
public:
    IntervalEvaluator(const Tree& root);
    IntervalEvaluator(const Tree& root,
                      const std::map<Tree::Id, float>& vars);
    IntervalEvaluator(std::shared_ptr<Deck> d);
    IntervalEvaluator(std::shared_ptr<Deck> d,
                      const std::map<Tree::Id, float>& vars);

    /*
     *  Interval evaluation
     */
    Interval::I eval(const Eigen::Vector3f& lower,
                     const Eigen::Vector3f& upper);
    Interval::I eval(const Eigen::Vector3f& lower,
                     const Eigen::Vector3f& upper,
                     std::shared_ptr<Tape> tape);

    std::pair<Interval::I, std::shared_ptr<Tape>> evalAndPush(
                     const Eigen::Vector3f& lower,
                     const Eigen::Vector3f& upper);
    std::pair<Interval::I, std::shared_ptr<Tape>> evalAndPush(
                     const Eigen::Vector3f& lower,
                     const Eigen::Vector3f& upper,
                     std::shared_ptr<Tape> tape);

    /*
     *  Returns a shortened tape based on the most recent evaluation.
     *
     *  Normally, this is invoked through evalAndPush, but in some cases,
     *  we need to call it as a standalone function.  If you're not using
     *  Oracles, then you probably don't need to call it.
     */
    std::shared_ptr<Tape> push(/* uses top-level tape */);
    std::shared_ptr<Tape> push(std::shared_ptr<Tape> tape);

    /*
     *  Changes a variable's value
     *
     *  If the variable isn't present in the tree, does nothing
     *  Returns true if the variable's value changes
     */
    bool setVar(Tree::Id var, float value);

    bool isSafe() const { return safe; }

protected:
    /*  i[clause].first is the interval result for that clause,
     *  i[clause].second indicates whether the result might be NaN (which is
     *  generally not included in interval evaluation) */
    std::vector<std::pair<Interval::I, bool>> i;

    /*  Marks whether the most recent evaluation could have any NaN in its
     *  root clause.
     */
     bool safe;

    /*
     *  Per-clause evaluation, used in tape walking
     */
    void operator()(Opcode::Opcode op, Clause::Id id,
                    Clause::Id a, Clause::Id b);

    friend class Tape; // for rwalk<IntervalEvaluator>
};

}   // namespace Kernel
