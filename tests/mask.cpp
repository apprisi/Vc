/*  This file is part of the Vc library. {{{

    Copyright (C) 2009-2013 Matthias Kretz <kretz@kde.org>

    Vc is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    Vc is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Vc.  If not, see <http://www.gnu.org/licenses/>.

}}}*/

#include "unittest.h"
#include <iostream>
#include "vectormemoryhelper.h"
#include <cmath>

using Vc::float_m;
using Vc::double_m;
using Vc::int_m;
using Vc::uint_m;
using Vc::short_m;
using Vc::ushort_m;

template<typename T> T two() { return T(2); }
template<typename T> T three() { return T(3); }

#define ALL_TYPES (ALL_VECTORS)
//, SIMD_ARRAYS(33), SIMD_ARRAYS(32), SIMD_ARRAYS(31), SIMD_ARRAYS(16), SIMD_ARRAYS(8), SIMD_ARRAYS(7), SIMD_ARRAYS(4), SIMD_ARRAYS(3), SIMD_ARRAYS(2), SIMD_ARRAYS(1))

TEST_TYPES(Vec, testInc, ALL_TYPES) /*{{{*/
{
    VectorMemoryHelper<Vec> mem(2);
    typedef typename Vec::EntryType T;
    typedef typename Vec::Mask Mask;
    T *data = mem;
    for (size_t borderI = 0; borderI < Vec::Size; ++borderI) {/*{{{*/
        const T border = static_cast<T>(borderI);
        for (size_t i = 0; i < Vec::Size; ++i) {
            data[i] = static_cast<T>(i);
            data[i + Vec::Size] = data[i] + static_cast<T>(data[i] < border ? 1 : 0);
        }
        Vec a(&data[0]);
        Vec b(&data[Vec::Size]);
        Mask m = a < border;
        Vec aa(a);
        COMPARE(aa(m)++, a) << ", border: " << border << ", m: " << m;
        COMPARE(aa, b) << ", border: " << border << ", m: " << m;
        COMPARE(++a(m), b) << ", border: " << border << ", m: " << m;
        COMPARE(a, b) << ", border: " << border << ", m: " << m;
    }
/*}}}*/
    for (size_t borderI = 0; borderI < Vec::Size; ++borderI) {
        const T border = static_cast<T>(borderI);
        for (size_t i = 0; i < Vec::Size; ++i) {
            data[i] = static_cast<T>(i);
            data[i + Vec::Size] = data[i] + static_cast<T>(data[i] < border ? 1 : 0);
        }
        Vec a(&data[0]);
        Vec b(&data[Vec::Size]);
        Mask m = a < border;
        Vec aa(a);
        where(m)(aa)++;
        COMPARE(aa, b) << ", border: " << border << ", m: " << m;
        ++where(m)(a);
        COMPARE(a, b) << ", border: " << border << ", m: " << m;
    }
}
/*}}}*/
TEST_TYPES(Vec, testDec, ALL_TYPES) /*{{{*/
{
    VectorMemoryHelper<Vec> mem(2);
    typedef typename Vec::EntryType T;
    typedef typename Vec::Mask Mask;
    T *data = mem;
    for (size_t borderI = 0; borderI < Vec::Size; ++borderI) {
        const T border = static_cast<T>(borderI);
        for (size_t i = 0; i < Vec::Size; ++i) {
            data[i] = static_cast<T>(i + 1);
            data[i + Vec::Size] = data[i] - static_cast<T>(data[i] < border ? 1 : 0);
        }
        Vec a(&data[0]);
        Vec b(&data[Vec::Size]);
        Mask m = a < border;
        Vec aa(a);
        COMPARE(aa(m)--, a);
        COMPARE(aa, b);

        aa = a;
        where(m)(aa)--;
        COMPARE(aa, b);

        aa = a;
        --where(m)(aa);
        COMPARE(aa, b);

        COMPARE(--a(m), b);
        COMPARE(a, b);
    }
}
/*}}}*/
TEST_TYPES(Vec, testPlusEq, ALL_TYPES) /*{{{*/
{
    VectorMemoryHelper<Vec> mem(2);
    typedef typename Vec::EntryType T;
    typedef typename Vec::Mask Mask;
    T *data = mem;
    for (size_t borderI = 0; borderI < Vec::Size; ++borderI) {
        const T border = static_cast<T>(borderI);
        for (size_t i = 0; i < Vec::Size; ++i) {
            data[i] = static_cast<T>(i + 1);
            data[i + Vec::Size] = data[i] + static_cast<T>(data[i] < border ? 2 : 0);
        }
        Vec a(&data[0]);
        Vec b(&data[Vec::Size]);
        Vec c = a;
        Mask m = a < border;
        COMPARE(a(m) += two<T>(), b);
        COMPARE(a, b);
        where(m) | c += two<T>();
        COMPARE(c, b);
    }
}
/*}}}*/
TEST_TYPES(Vec, testMinusEq, ALL_TYPES) /*{{{*/
{
    VectorMemoryHelper<Vec> mem(2);
    typedef typename Vec::EntryType T;
    typedef typename Vec::Mask Mask;
    T *data = mem;
    for (size_t borderI = 0; borderI < Vec::Size; ++borderI) {
        const T border = static_cast<T>(borderI);
        for (size_t i = 0; i < Vec::Size; ++i) {
            data[i] = static_cast<T>(i + 2);
            data[i + Vec::Size] = data[i] - static_cast<T>(data[i] < border ? 2 : 0);
        }
        Vec a(&data[0]);
        Vec b(&data[Vec::Size]);
        Mask m = a < border;

        Vec c = a;
        where(m) | c -= two<T>();
        COMPARE(c, b);

        COMPARE(a(m) -= two<T>(), b);
        COMPARE(a, b);
    }
}
/*}}}*/
TEST_TYPES(Vec, testTimesEq, ALL_TYPES) /*{{{*/
{
    VectorMemoryHelper<Vec> mem(2);
    typedef typename Vec::EntryType T;
    typedef typename Vec::Mask Mask;
    T *data = mem;
    for (size_t borderI = 0; borderI < Vec::Size; ++borderI) {
        const T border = static_cast<T>(borderI);
        for (size_t i = 0; i < Vec::Size; ++i) {
            data[i] = static_cast<T>(i);
            data[i + Vec::Size] = data[i] * static_cast<T>(data[i] < border ? 2 : 1);
        }
        Vec a(&data[0]);
        Vec b(&data[Vec::Size]);
        Mask m = a < border;

        Vec c = a;
        where(m) | c *= two<T>();
        COMPARE(c, b);

        COMPARE(a(m) *= two<T>(), b);
        COMPARE(a, b);
    }
}
/*}}}*/
TEST_TYPES(Vec, testDivEq, ALL_TYPES) /*{{{*/
{
    VectorMemoryHelper<Vec> mem(2);
    typedef typename Vec::EntryType T;
    typedef typename Vec::Mask Mask;
    T *data = mem;
    for (size_t borderI = 0; borderI < Vec::Size; ++borderI) {
        const T border = static_cast<T>(borderI);
        for (size_t i = 0; i < Vec::Size; ++i) {
            data[i] = static_cast<T>(5 * i);
            data[i + Vec::Size] = data[i] / static_cast<T>(data[i] < border ? 3 : 1);
        }
        Vec a(&data[0]);
        Vec b(&data[Vec::Size]);
        Mask m = a < border;

        Vec c = a;
        where(m) | c /= three<T>();
        COMPARE(c, b);

        COMPARE(a(m) /= three<T>(), b);
        COMPARE(a, b);
    }
}
/*}}}*/
TEST_TYPES(Vec, testAssign, ALL_TYPES) /*{{{*/
{
    VectorMemoryHelper<Vec> mem(2);
    typedef typename Vec::EntryType T;
    typedef typename Vec::Mask Mask;
    T *data = mem;
    for (size_t borderI = 0; borderI < Vec::Size; ++borderI) {
        const T border = static_cast<T>(borderI);
        for (size_t i = 0; i < Vec::Size; ++i) {
            data[i] = static_cast<T>(i);
            data[i + Vec::Size] = data[i] + static_cast<T>(data[i] < border ? 2 : 0);
        }
        Vec a(&data[0]);
        Vec b(&data[Vec::Size]);
        Mask m = a < border;

        Vec c = a;
        where(m) | c = b;
        COMPARE(c, b);

        COMPARE(a(m) = b, b);
        COMPARE(a, b);
    }
}
/*}}}*/
TEST_TYPES(Vec, testZero, ALL_TYPES) /*{{{*/
{
    typedef typename Vec::EntryType T;
    typedef typename Vec::Mask Mask;
    typedef typename Vec::IndexType I;

    for (size_t cut = 0; cut < Vec::Size; ++cut) {
        const Mask mask(I(Vc::IndexesFromZero) < cut);
        //std::cout << mask << std::endl;

        const T aa = 4;
        Vec a(aa);
        Vec b(Vc::Zero);

        where(!mask) | b = a;
        a.setZero(mask);

        COMPARE(a, b);
    }
}
/*}}}*/
TEST_TYPES(Vec, testCount, ALL_TYPES) /*{{{*/
{
    typedef typename Vec::Mask M;

    UnitTest::withRandomMask<Vec>([](M m) {
        int count = 0;
        for (size_t i = 0; i < Vec::Size; ++i) {
            if (m[i]) {
                ++count;
            }
        }
        COMPARE(m.count(), count) << ", m = " << m;
    });
}
/*}}}*/
TEST_TYPES(Vec, testFirstOne, ALL_TYPES) /*{{{*/
{
    typedef typename Vec::IndexType I;
    typedef typename Vec::Mask M;

    for (unsigned int i = 0; i < Vec::Size; ++i) {
        const M mask(I(Vc::IndexesFromZero) == i);
        COMPARE(mask.firstOne(), int(i));
    }
}
/*}}}*/
TEST_TYPES(V, shifted, (ALL_VECTORS, SIMD_ARRAYS(16), SIMD_ARRAYS(31)))/*{{{*/
{
    using M = typename V::Mask;
    constexpr int Size = V::Size;
    UnitTest::withRandomMask<V>([](const M &reference) {
        for (int shift = -2 * Size; shift <= 2 * Size; ++shift) {
            const M test = reference.shifted(shift);
            for (int i = 0; i < Size; ++i) {
                if (i + shift >= 0 && i + shift < Size) {
                    COMPARE(test[i], reference[i + shift])
                        << "shift: " << shift << ", i: " << i << ", test: " << test
                        << ", reference: " << reference;
                } else {
                    COMPARE(test[i], false) << "shift: " << shift << ", i: " << i
                                            << ", test: " << test
                                            << ", reference: " << reference;
                }
            }
        }
    });
}
/*}}}*/

template<typename M1, typename M2> void testLogicalOperatorsImpl()/*{{{*/
{
    VERIFY((M1(true) && M2(true)).isFull());
    VERIFY((M1(true) && M2(false)).isEmpty());
    VERIFY((M1(true) || M2(true)).isFull());
    VERIFY((M1(true) || M2(false)).isFull());
    VERIFY((M1(false) || M2(false)).isEmpty());
}
/*}}}*/
template<typename M1, typename M2> void testBinaryOperatorsImpl()/*{{{*/
{
    testLogicalOperatorsImpl<M1, M2>();

    VERIFY((M1(true) & M2(true)).isFull());
    VERIFY((M1(true) & M2(false)).isEmpty());
    VERIFY((M1(true) | M2(true)).isFull());
    VERIFY((M1(true) | M2(false)).isFull());
    VERIFY((M1(false) | M2(false)).isEmpty());
    VERIFY((M1(true) ^ M2(true)).isEmpty());
    VERIFY((M1(true) ^ M2(false)).isFull());
}
/*}}}*/
TEST(testBinaryOperators) /*{{{*/
{
    testBinaryOperatorsImpl< short_m,  short_m>();
    testBinaryOperatorsImpl< short_m, ushort_m>();
    testBinaryOperatorsImpl<ushort_m,  short_m>();
    testBinaryOperatorsImpl<ushort_m, ushort_m>();

    testBinaryOperatorsImpl<   int_m,    int_m>();
    testBinaryOperatorsImpl<   int_m,   uint_m>();
    testBinaryOperatorsImpl<   int_m,  float_m>();
    testBinaryOperatorsImpl<  uint_m,    int_m>();
    testBinaryOperatorsImpl<  uint_m,   uint_m>();
    testBinaryOperatorsImpl<  uint_m,  float_m>();
    testBinaryOperatorsImpl< float_m,    int_m>();
    testBinaryOperatorsImpl< float_m,   uint_m>();
    testBinaryOperatorsImpl< float_m,  float_m>();

    testBinaryOperatorsImpl<double_m, double_m>();
}
/*}}}*/

TEST_TYPES(V, maskReductions, ALL_TYPES) /*{{{*/
{
    UnitTest::withRandomMask<V>([](typename V::Mask mask) {
        constexpr decltype(mask.count()) size = V::Size;
        COMPARE(all_of(mask), mask.count() == size);
        if (mask.count() > 0) {
            VERIFY(any_of(mask));
            VERIFY(!none_of(mask));
            COMPARE(some_of(mask), mask.count() < size);
        } else {
            VERIFY(!any_of(mask));
            VERIFY(none_of(mask));
            VERIFY(!some_of(mask));
        }
    });
}/*}}}*/
TEST_TYPES(V, maskInit, ALL_TYPES) /*{{{*/
{
    typedef typename V::Mask M;
    COMPARE(M(Vc::One), M(true));
    COMPARE(M(Vc::Zero), M(false));
}
/*}}}*/
TEST_TYPES(V, maskScalarAccess, ALL_TYPES) /*{{{*/
{
    typedef typename V::Mask M;
    UnitTest::withRandomMask<V>([](M mask) {
        const auto &mask2 = mask;
        for (size_t i = 0; i < V::Size; ++i) {
            COMPARE(bool(mask[i]), mask2[i]);
        }

        const auto maskInv = !mask;
        for (size_t i = 0; i < V::Size; ++i) {
            mask[i] = !mask[i];
        }
        COMPARE(mask, maskInv);

        for (size_t i = 0; i < V::Size; ++i) {
            mask[i] = true;
        }
        COMPARE(mask, M(true));
    });
}/*}}}*/
template<typename MTo, typename MFrom> void testMaskConversion(const MFrom &m)/*{{{*/
{
    MTo test(m);
    size_t i = 0;
    for (; i < std::min(m.Size, test.Size); ++i) {
        COMPARE(test[i], m[i]) << i << " conversion from " << UnitTest::typeToString<MFrom>()
                               << " to " << UnitTest::typeToString<MTo>();
    }
    for (; i < test.Size; ++i) {
        COMPARE(test[i], false) << i << " conversion from " << UnitTest::typeToString<MFrom>()
                                << " to " << UnitTest::typeToString<MTo>();
    }
}/*}}}*/
TEST_TYPES(V, maskConversions, ALL_TYPES) /*{{{*/
{
    typedef typename V::Mask M;
    UnitTest::withRandomMask<V>([](M m) {
        testMaskConversion< float_m>(m);
        testMaskConversion<double_m>(m);
        testMaskConversion<   int_m>(m);
        testMaskConversion<  uint_m>(m);
        testMaskConversion< short_m>(m);
        testMaskConversion<ushort_m>(m);
    });
}
/*}}}*/
TEST_TYPES(V, testIntegerConversion, ALL_TYPES) /*{{{*/
{
    UnitTest::withRandomMask<V>([](typename V::Mask m) {
        auto bit = m.toInt();
        for (size_t i = 0; i < m.Size; ++i) {
            COMPARE(!!((bit >> i) & 1), m[i]);
        }
    });
}
/*}}}*/
TEST_TYPES(V, boolConversion, ALL_TYPES) /*{{{*/
{
    bool mem[V::Size + 64] __attribute__((aligned(16)));
    UnitTest::withRandomMask<V>([&](typename V::Mask m) {
        bool *ptr = mem;
        m.store(ptr);
        for (size_t i = 0; i < V::Size; ++i) {
            COMPARE(ptr[i], m[i]) << "offset: " << ptr - mem;
        }

        typename V::Mask m2(ptr);
        COMPARE(m2, m) << "offset: " << ptr - mem;
        for (++ptr; ptr < &mem[64]; ++ptr) {
            m.store(ptr, Vc::Unaligned);
            for (size_t i = 0; i < V::Size; ++i) {
                COMPARE(ptr[i], m[i]) << "offset: " << ptr - mem;
            }

            typename V::Mask m3(ptr, Vc::Unaligned);
            COMPARE(m3, m) << "offset: " << ptr - mem;
        }
    });
}
/*}}}*/
TEST_TYPES(V, testCompareOperators, ALL_TYPES) /*{{{*/
{
    typedef typename V::Mask M;
    const M a(true);
    const M b(false);
    VERIFY(!(a == b));
    VERIFY(!(b == a));
    VERIFY(a != b);
    VERIFY(b != a);

    for_all_masks(V, k)
    {
        M randomMask;
        do {
            randomMask = V::Random() < V::Random();
        } while (randomMask.isEmpty());
        const M k2 = k ^ randomMask;

        VERIFY( (k  == k )) << k;
        VERIFY(!(k2 == k )) << k << k2;
        VERIFY(!(k  == k2)) << k << k2;
        VERIFY( (k2 == k2)) << k << k2;

        VERIFY(!(k  != k )) << k;
        VERIFY( (k  != k2)) << k << k2;
        VERIFY( (k2 != k )) << k << k2;
        VERIFY(!(k2 != k2)) << k << k2;
    }
}
/*}}}*/

// vim: foldmethod=marker
