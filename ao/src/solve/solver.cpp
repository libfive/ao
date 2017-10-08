
#include "ao/solve/solver.hpp"
#include "ao/tree/tree.hpp"
#include "ao/eval/eval_jacobian.hpp"

namespace Kernel {

namespace Solver
{

static std::pair<float, Solution> findRoot(
        JacobianEvaluator& e, const Eigen::Vector3f pos,
        Solution vars, unsigned gas)
{
    const float EPSILON = 1e-6;

    // Create a static map for all of our derivatives
    std::map<Tree::Id, float> ds;
    for (auto& v : vars)
    {
        ds.insert({v.first, 0});
    }

    float r = e.eval(pos);
    bool converged = false;
    while (!converged && fabs(r) >= EPSILON && --gas)
    {
        // Evaluate and update our local gradient
        for (auto& d : e.gradient(pos))
        {
            auto v = ds.find(d.first);
            if (v != ds.end())
            {
                v->second = d.second;
            }
        }

        // Break if all of our gradients are nearly zero
        if (std::all_of(ds.begin(), ds.end(),
            [&EPSILON](decltype(ds)::value_type itr) {
                return fabs(itr.second) < EPSILON;
            }))
        {
            break;
        }

        // Solve for step size using a backtracking line search
        const float slope = std::accumulate(ds.begin(), ds.end(), 0.0f,
                [](float d, const decltype(ds)::value_type& itr) {
                    return d + pow(itr.second, 2); });

        for (float step = r / slope; true; step /= 2)
        {
            for (auto& v : vars)
            {
                e.setVar(v.first, v.second - step * ds.at(v.first));
            }

            // Get new residual
            const auto r_ = e.eval(pos);

            // Find change in residuals
            const auto diff = r - r_;

            // If we've satisfied the Armijo–Goldstein condition, then break
            // (along with a bunch of other conditions to detect infinite loops)
            if (diff / step >= slope * 0.5 || fabs(diff) < EPSILON ||
                slope < EPSILON || r_ < EPSILON)
            {
                // If residuals are converging, then exit outer loop too
                converged = fabs(diff) < EPSILON;
                r = r_;

                // Store new variable values in our map
                for (auto& v : vars)
                {
                    v.second -= step * ds.at(v.first);
                }
                break;
            }
        }
    }
    return {r, vars};
}

////////////////////////////////////////////////////////////////////////////////

std::pair<float, Solution> findRoot(
        const Tree& t, const std::map<Tree::Id, float>& vars,
        const Eigen::Vector3f pos, const Mask& mask, unsigned gas)
{
    auto tape = std::make_shared<Tape>(t);
    JacobianEvaluator e(tape, vars);
    return findRoot(e, vars, pos, mask, gas);
}

std::pair<float, Solution> findRoot(
        JacobianEvaluator& e, std::map<Tree::Id, float> vars,
        const Eigen::Vector3f pos, const Mask& mask, unsigned gas)
{
    // Load initial variable values here
    for (auto& v : vars)
    {
        e.setVar(v.first, v.second);
    }

    // Ignore masked variables
    for (const auto& v : mask)
    {
        vars.erase(v);
    }

    return findRoot(e, pos, vars, gas);
}

} // namespace Solver

}   // namespace Kernel
