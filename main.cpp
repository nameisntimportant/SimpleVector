#include "vector.h"
#include "utils.h"
#include "testRunner.h"

#include <limits>

using namespace std;
class ClassWithStrangeConstructor
{
public:
    int x, y;

    ClassWithStrangeConstructor(int& r, const int& cr)
        : x(r)
        , y(cr)
    {
    }
};

class C
{
public:
    inline static int created = 0;
    inline static int assigned = 0;
    inline static int deleted = 0;
    inline static int moved = 0;
    static void Reset()
    {
        created = assigned = deleted = moved = 0;
    }
    C()
    {
        ++created;
    }
    C(const C& other)
    {
        UNUSED(other);
        ++created;
    }
    C& operator=(const C& other)
    {
        UNUSED(other);
        ++assigned;
        return *this;
    }
    ~C()
    {
        ++deleted;
    }
};

void TestInsert()
{
    Vector<int> v;
    v.pushBack(1);
    v.pushBack(2);
    auto it = v.insert(v.cbegin(), 0);
    ASSERT(v.size() == 3 && v[0] == 0 && v[1] == 1 && v[2] == 2 && it == v.begin());

    it = v.insert(v.cend(), 3);
    ASSERT(v.size() == 4 && v[0] == 0 && v[1] == 1 && v[2] == 2 && v[3] == 3 && it + 1 == v.end());
}

void TestInsertWithReserve()
{
    Vector<int> v;
    v.reserve(5);
    v.pushBack(1);
    v.pushBack(2);
    auto it = v.insert(v.cbegin(), 0);
    ASSERT(v.size() == 3 && v[0] == 0 && v[1] == 1 && v[2] == 2 && it == v.begin());

    it = v.insert(v.cend(), 3);
    ASSERT(v.size() == 4 && v[0] == 0 && v[1] == 1 && v[2] == 2 && v[3] == 3 && it + 1 == v.end());

    it = v.insert(v.begin() + 2, 10);
    ASSERT(v.size() == 5 && v[0] == 0 && v[1] == 1 && v[2] == 10 && v[3] == 2 && v[4] == 3 &&
           it + 3 == v.end());
}

void TestEmplace()
{
    Vector<ClassWithStrangeConstructor> v;
    int x = 1;
    const int y = 2;
    int z = 3;
    ClassWithStrangeConstructor c(z, z);
    v.pushBack(c);
    auto it = v.emplace(v.cbegin(), x, y);
    ASSERT(v.size() == 2 && v[0].x == x && v[0].y == y && v[1].x == z && v[1].y == z &&
           it == v.begin());
}

void TestErase()
{
    Vector<int> v;
    v.pushBack(1);
    v.pushBack(2);
    v.pushBack(3);
    auto it = v.erase(v.cbegin() + 1);
    ASSERT(v.size() == 2 && v[0] == 1 && v[1] == 3 && it == v.begin() + 1);
}

void TestInit()
{
    {
        C::Reset();
        Vector<C> v(3);
        ASSERT(C::created == 3 && C::assigned == 0 && C::deleted == 0);
    }
    ASSERT(C::deleted == 3);
}

void TestAssign()
{
    {
        C::Reset();
        Vector<C> v1(2), v2(3);
        ASSERT(C::created == 5 && C::assigned == 0 && C::deleted == 0);
        v1 = v2;
        ASSERT(C::created == 8 && C::assigned == 0 && C::deleted == 2);
        ASSERT(v1.size() == 3 && v2.size() == 3);
    }
    ASSERT(C::deleted == 8);
    ASSERT_DOUBLE_EQUAL(10, 10);

    {
        C::Reset();
        Vector<C> v1(3), v2(2);
        ASSERT(C::created == 5 && C::assigned == 0 && C::deleted == 0);
        v1 = v2;
        ASSERT(C::created == 5 && C::assigned == 2 && C::deleted == 1);
        ASSERT(v1.size() == 2 && v2.size() == 2);
    }
    ASSERT(C::deleted == 5);
}

void TestPushBack()
{
    {
        C::Reset();
        Vector<C> v;
        C c;
        v.pushBack(c);
        ASSERT(C::created == 2 && C::assigned == 0 && C::deleted == 0);

        v.pushBack(c); // reallocation
        ASSERT(C::created == 4 && C::assigned == 0 && C::deleted == 1);
    }
    ASSERT(C::deleted == 4);
}

void TestEmplaceBack()
{
    {
        C::Reset();
        Vector<C> v;
        v.emplaceBack();
        ASSERT(C::created == 1 && C::assigned == 0 && C::deleted == 0);

        v.emplaceBack(); // reallocation
        ASSERT(C::created == 3 && C::assigned == 0 && C::deleted == 1);
        ASSERT_EQUAL(v.size(), 2u);
    }
    ASSERT(C::deleted == 3);
}

void TestReserveEmpty()
{
    {
        C::Reset();
        Vector<C> v;
        v.reserve(5);
        ASSERT_EQUAL(C::created, 0);
        C c;
        v.pushBack(c);
        v.pushBack(c); // no reallocation
        ASSERT_EQUAL(C::created, 3);
        ASSERT_EQUAL(v.size(), 2u);
    }
    ASSERT(C::deleted == 3);
}

void TestReserveWithSomething()
{
    {
        C::Reset();
        Vector<C> v;
        v.emplaceBack();

        v.reserve(2); // reallocation
        ASSERT_EQUAL(C::created, 2);
        ASSERT_EQUAL(v.size(), 1u);
        ASSERT_EQUAL(v.capacity(), 2u);

        v.emplaceBack(); // no reallocation
        ASSERT_EQUAL(C::created, 3);
        ASSERT_EQUAL(v.size(), 2u);
    }
    ASSERT(C::deleted == 3);
}

void TestResizeEmpty()
{
    {
        C::Reset();
        Vector<C> v;
        v.resize(5);
        ASSERT(C::created == 5 && C::assigned == 0 && C::deleted == 0);
        ASSERT_EQUAL(v.size(), 5u);
    }
    ASSERT(C::deleted == 5);
}

void TestResizeWithSomething()
{
    {
        C::Reset();
        Vector<C> v;
        v.emplaceBack();
        ASSERT(C::created == 1 && C::assigned == 0 && C::deleted == 0);

        v.resize(5); // resize + reallocation
        ASSERT(C::created == 6 && C::assigned == 0 && C::deleted == 1);
        ASSERT_EQUAL(v.size(), 5u);
    }
    ASSERT(C::deleted == 6);
}

void TestResizeToLess()
{
    {
        C::Reset();
        Vector<C> v(5);
        ASSERT(C::created == 5 && C::assigned == 0 && C::deleted == 0);
        v.resize(3);
        ASSERT(C::created == 5 && C::assigned == 0 && C::deleted == 2);
    }
    ASSERT(C::deleted == 5);
}

void TestPopBack()
{
    Vector<C> v;
    v.resize(5);
    ASSERT_EQUAL(v.size(), 5u);
    for (size_t i = v.size(); i >= 1; i--)
    {
        v.popBack();
        ASSERT_EQUAL(v.size(), (i - 1));
    }
}

void TestCtors()
{
    {
        C::Reset(); // Test default
        Vector<C> empty;
        ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0);
        ASSERT_EQUAL(empty.size(), 0u);
        ASSERT_EQUAL(empty.capacity(), 0u);

        Vector<C> v(5);
        ASSERT(C::created == 5 && C::assigned == 0 && C::deleted == 0);
        ASSERT_EQUAL(v.size(), 5u);
        ASSERT_EQUAL(v.capacity(), 5u);

        C::Reset();
        Vector<C> v2(v); // Test copy
        ASSERT(C::created == 5 && C::assigned == 0 && C::deleted == 0);
        ASSERT_EQUAL(v2.size(), 5u);
        ASSERT_EQUAL(v2.capacity(), 5u);

        C::Reset();
        Vector<C> v3(move(v)); // Test move
        ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0);
        ASSERT_EQUAL(v3.size(), 5u);
        ASSERT_EQUAL(v3.capacity(), 5u);
        ASSERT_EQUAL(v.size(), 0u);
        ASSERT_EQUAL(v.capacity(), 0u);

        C::Reset();
        Vector<C> v4(v); // Test copy empty
        ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0);
        ASSERT_EQUAL(v4.size(), 0u);
        ASSERT_EQUAL(v4.capacity(), 0u);
    }
}

int main()
{
    TestRunner tr;
    RUN_TEST(tr, TestInit);
    RUN_TEST(tr, TestAssign);
    RUN_TEST(tr, TestPushBack);
    RUN_TEST(tr, TestEmplaceBack);
    RUN_TEST(tr, TestReserveEmpty);
    RUN_TEST(tr, TestReserveWithSomething);
    RUN_TEST(tr, TestResizeEmpty);
    RUN_TEST(tr, TestResizeWithSomething);
    RUN_TEST(tr, TestPopBack);
    RUN_TEST(tr, TestResizeToLess);
    RUN_TEST(tr, TestCtors);
    RUN_TEST(tr, TestInsert);
    RUN_TEST(tr, TestInsertWithReserve);
    RUN_TEST(tr, TestEmplace);
    RUN_TEST(tr, TestErase);

    return 0;
}
