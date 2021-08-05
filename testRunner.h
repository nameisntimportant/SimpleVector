#pragma once

#include "utils.h"

#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

template <typename T, typename Y>
std::ostream& operator<<(std::ostream& os, const std::variant<T, Y>& var)
{
    std::visit([&os](const auto& arg) { os << arg; }, var);
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::optional<T>& opt)
{
    if (!opt)
    {
        return os << "optional is empty";
    }
    else
    {
        return os << *opt;
    }
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& s)
{
    os << "{";
    bool first = true;
    for (const auto& x : s)
    {
        if (!first)
        {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::list<T>& s)
{
    os << "{";
    bool first = true;
    for (const auto& x : s)
    {
        if (!first)
        {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& s)
{
    os << "{";
    bool first = true;
    for (const auto& x : s)
    {
        if (!first)
        {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m)
{
    os << "{";
    bool first = true;
    for (const auto& kv : m)
    {
        if (!first)
        {
            os << ", ";
        }
        first = false;
        os << kv.first << ": " << kv.second;
    }
    return os << "}";
}

template <class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint = {})
{
    if (!(t == u))
    {
        std::ostringstream os;
        os << "Assertion failed:\n\"" << t << "\"\n!=\n\"" << u << "\"";
        if (!hint.empty())
        {
            os << " hint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

inline void AssertDoubleEqual(double lhs, double rhs, const std::string& hint = {})
{
    if (!fuzzyCompare(lhs, rhs))
    {
        std::ostringstream os;
        os << "Assertion failed: \"" << std::setprecision(6) << lhs << "\" != \"" << rhs << "\"";
        if (!hint.empty())
        {
            os << " hint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

inline void Assert(bool b, const std::string& hint)
{
    AssertEqual(b, true, hint);
}

class TestRunner
{
public:
    template <class TestFunc>
    void RunTest(TestFunc func, const std::string& test_name)
    {
        // TODO: add flag, which will make RunTest not to catch exceptions, to let debugger catch it
        try
        {
            func();
            std::cerr << test_name << " OK" << std::endl;
        }
        catch (std::exception& e)
        {
            ++failCount;
            std::cerr << test_name << " fail: " << e.what() << std::endl;
        }
        catch (...)
        {
            ++failCount;
            std::cerr << "Unknown exception caught" << std::endl;
        }
    }

    ~TestRunner()
    {
        if (failCount > 0)
        {
            std::cerr << failCount << " unit tests failed. Terminate" << std::endl;
            exit(1);
        }
    }

private:
    int failCount = 0;
};

#define ASSERT_EXCEPTION_THROWN(expression, exceptionType)                              \
    {                                                                                   \
        bool isExceptionThrown = false;                                                 \
        try                                                                             \
        {                                                                               \
            expression;                                                                 \
        }                                                                               \
        catch (const exceptionType&)                                                    \
        {                                                                               \
            isExceptionThrown = true;                                                   \
        }                                                                               \
        catch (...)                                                                     \
        {                                                                               \
            std::ostringstream os;                                                      \
            os << #expression << " hasn't thrown an expected exception " #exceptionType \
               << __FILE__ << ":" << __LINE__;                                          \
            Assert(false, os.str());                                                    \
        }                                                                               \
        std::ostringstream os;                                                          \
        os << #expression << " was expected to throw an exception " << #exceptionType   \
           << " but it hasn't thrown any exception. " << __FILE__ << ":" << __LINE__;   \
        Assert(isExceptionThrown, os.str());                                            \
    }

#define ASSERT_DOUBLE_EQUAL(x, y)                                        \
    {                                                                    \
        std::ostringstream os;                                           \
        os << #x << " != " << #y << ", " << __FILE__ << ":" << __LINE__; \
        AssertDoubleEqual(x, y, os.str());                               \
    }

#define ASSERT_EQUAL(x, y)                                               \
    {                                                                    \
        std::ostringstream os;                                           \
        os << #x << " != " << #y << ", " << __FILE__ << ":" << __LINE__; \
        AssertEqual(x, y, os.str());                                     \
    }

#define ASSERT(x)                                                 \
    {                                                             \
        std::ostringstream os;                                    \
        os << #x << " is false, " << __FILE__ << ":" << __LINE__; \
        Assert(x, os.str());                                      \
    }

#define RUN_TEST(tr, func) tr.RunTest(func, #func)
