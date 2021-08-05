#include "utils.h"

#include <cctype>
#include <cmath>

using namespace std;

namespace
{
constexpr auto Precision = 0.0001;

double relativeDiff(double lhs, double rhs)
{
    double max = std::max(fabs(lhs), fabs(rhs));
    return max < std::numeric_limits<double>::epsilon() ? 0.0 : fabs(lhs - rhs) / max;
}
} // namespace

bool fuzzyCompare(double lhs, double rhs)
{
    return (relativeDiff(lhs, rhs) < Precision);
}

string_view strip(string_view line)
{
    while (!line.empty() && isspace(line.front()))
    {
        line.remove_prefix(1);
    }
    while (!line.empty() && isspace(line.back()))
    {
        line.remove_suffix(1);
    }
    return line;
}

bool isZero(double x)
{
    return abs(x) < 1e-6;
}
