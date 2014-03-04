/*  This file is part of the Vc library.

    Copyright (C) 2011-2012 Matthias Kretz <kretz@kde.org>

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

*/

#include "../common/data.h"
#include "where.h"
#include "iterators.h"
#include "macros.h"
namespace Vc_VERSIONED_NAMESPACE
{
namespace Scalar
{

// load member functions{{{1
template <typename T>
template <typename U, typename Flags, typename>
Vc_INTRINSIC void Vector<T>::load(const U *mem, Flags)
{
    m_data = mem[0];
}

// store member functions{{{1
template <typename T>
template <typename U, typename Flags, typename>
Vc_INTRINSIC void Vector<T>::store(U *mem, Flags) const
{
    mem[0] = m_data;
}
template <typename T>
template <typename U, typename Flags, typename>
Vc_INTRINSIC void Vector<T>::store(U *mem, Mask mask, Flags) const
{
    if (mask.data())
        mem[0] = m_data;
}

// gather {{{1
template <typename T>
template <typename MT, typename IT>
Vc_ALWAYS_INLINE void Vector<T>::gatherImplementation(const MT *mem, IT &&indexes)
{
    m_data = mem[indexes[0]];
}

template <typename T>
template <typename MT, typename IT>
Vc_ALWAYS_INLINE void Vector<T>::gatherImplementation(const MT *mem,
                                                      IT &&indexes,
                                                      MaskArgument mask)
{
    if (mask.data()) {
        m_data = mem[indexes[0]];
    }
}
// scatter {{{1
template <typename T>
template <typename MT, typename IT>
Vc_ALWAYS_INLINE void Vector<T>::scatterImplementation(MT *mem, IT &&indexes) const
{
    mem[indexes[0]] = m_data;
}

template <typename T>
template <typename MT, typename IT>
Vc_ALWAYS_INLINE void Vector<T>::scatterImplementation(MT *mem, IT &&indexes, MaskArgument mask) const
{
    if (mask.data()) {
        mem[indexes[0]] = m_data;
    }
}

// copySign {{{1
template <>
Vc_INTRINSIC Vector<float> Vector<float>::copySign(Vector<float> reference) const
{
    union {
        float f;
        unsigned int i;
    } value, sign;
    value.f = data();
    sign.f = reference.data();
    value.i = (sign.i & 0x80000000u) | (value.i & 0x7fffffffu);
    return float_v(value.f);
}
template<> Vc_INTRINSIC Vector<double> Vector<double>::copySign(Vector<double> reference) const
{
    union {
        double f;
        unsigned long long i;
    } value, sign;
    value.f = data();
    sign.f = reference.data();
    value.i = (sign.i & 0x8000000000000000ull) | (value.i & 0x7fffffffffffffffull);
    return double_v(value.f);
} // }}}1
// bitwise operators {{{1
#define VC_CAST_OPERATOR_FORWARD(op, IntT, VecT) \
template<> Vc_ALWAYS_INLINE VecT &VecT::operator op##=(const VecT &x) { \
    typedef IntT uinta Vc_MAY_ALIAS; \
    uinta *left = reinterpret_cast<uinta *>(&m_data); \
    const uinta *right = reinterpret_cast<const uinta *>(&x.m_data); \
    *left op##= *right; \
    return *this; \
} \
template<> Vc_ALWAYS_INLINE Vc_PURE VecT VecT::operator op(const VecT &x) const { \
    VecT ret = *this; \
    return VecT(ret op##= x); \
}
#define VC_CAST_OPERATOR_FORWARD_FLOAT(op)  VC_CAST_OPERATOR_FORWARD(op, unsigned int, Vector<float>)
#define VC_CAST_OPERATOR_FORWARD_DOUBLE(op) VC_CAST_OPERATOR_FORWARD(op, unsigned long long, Vector<double>)
VC_ALL_BINARY(VC_CAST_OPERATOR_FORWARD_FLOAT)
VC_ALL_BINARY(VC_CAST_OPERATOR_FORWARD_DOUBLE)
#undef VC_CAST_OPERATOR_FORWARD
#undef VC_CAST_OPERATOR_FORWARD_FLOAT
#undef VC_CAST_OPERATOR_FORWARD_DOUBLE
// }}}1
// operators {{{1
#include "../common/operators.h"
// }}}1
// exponent {{{1
template<> Vc_INTRINSIC Vector<float> Vector<float>::exponent() const
{
    VC_ASSERT(m_data >= 0.f);
    union { float f; int i; } value;
    value.f = m_data;
    return float_v(static_cast<float>((value.i >> 23) - 0x7f));
}
template<> Vc_INTRINSIC Vector<double> Vector<double>::exponent() const
{
    VC_ASSERT(m_data >= 0.);
    union { double f; long long i; } value;
    value.f = m_data;
    return double_v(static_cast<double>((value.i >> 52) - 0x3ff));
}
// }}}1
// FMA {{{1
static Vc_ALWAYS_INLINE float highBits(float x)
{
    union {
        float f;
        unsigned int i;
    } y;
    y.f = x;
    y.i &= 0xfffff000u;
    return y.f;
}
static Vc_ALWAYS_INLINE double highBits(double x)
{
    union {
        double f;
        unsigned long long i;
    } y;
    y.f = x;
    y.i &= 0xfffffffff8000000ull;
    return y.f;
}
template<typename T> Vc_ALWAYS_INLINE T _fusedMultiplyAdd(T a, T b, T c)
{
    const T h1 = highBits(a);
    const T l1 = a - h1;
    const T h2 = highBits(b);
    const T l2 = b - h2;
    const T ll = l1 * l2;
    const T lh = l1 * h2 + h1 * l2;
    const T hh = h1 * h2;
    if (std::abs(c) < std::abs(lh)) {
        return (ll + c) + (lh + hh);
    } else {
        return (ll + lh) + (c + hh);
    }
}
template<> Vc_ALWAYS_INLINE void float_v::fusedMultiplyAdd(const float_v &f, const float_v &s)
{
    data() = _fusedMultiplyAdd(data(), f.data(), s.data());
}
template<> Vc_ALWAYS_INLINE void double_v::fusedMultiplyAdd(const double_v &f, const double_v &s)
{
    data() = _fusedMultiplyAdd(data(), f.data(), s.data());
}
// Random {{{1
static Vc_ALWAYS_INLINE void _doRandomStep(Vector<unsigned int> &state0,
        Vector<unsigned int> &state1)
{
    state0.load(&Common::RandomState[0]);
    state1.load(&Common::RandomState[uint_v::Size]);
    (state1 * 0xdeece66du + 11).store(&Common::RandomState[uint_v::Size]);
    uint_v((state0 * 0xdeece66du + 11).data() ^ (state1.data() >> 16)).store(&Common::RandomState[0]);
}

template<typename T> Vc_INTRINSIC Vector<T> Vector<T>::Random()
{
    Vector<unsigned int> state0, state1;
    _doRandomStep(state0, state1);
    return Vector<T>(static_cast<EntryType>(state0.data()));
}
template<> Vc_INTRINSIC Vector<float> Vector<float>::Random()
{
    Vector<unsigned int> state0, state1;
    _doRandomStep(state0, state1);
    union { unsigned int i; float f; } x;
    x.i = (state0.data() & 0x0fffffffu) | 0x3f800000u;
    return float_v(x.f - 1.f);
}
template<> Vc_INTRINSIC Vector<double> Vector<double>::Random()
{
    typedef unsigned long long uint64 Vc_MAY_ALIAS;
    uint64 state0 = *reinterpret_cast<const uint64 *>(&Common::RandomState[8]);
    state0 = (state0 * 0x5deece66dull + 11) & 0x000fffffffffffffull;
    *reinterpret_cast<uint64 *>(&Common::RandomState[8]) = state0;
    union { unsigned long long i; double f; } x;
    x.i = state0 | 0x3ff0000000000000ull;
    return double_v(x.f - 1.);
}
// isNegative {{{1
template<typename T> Vc_INTRINSIC Vc_PURE typename Vector<T>::Mask Vector<T>::isNegative() const
{
    union { float f; unsigned int i; } u;
    u.f = m_data;
    return Mask(0u != (u.i & 0x80000000u));
}
template<> Vc_INTRINSIC Vc_PURE double_m double_v::isNegative() const
{
    union { double d; unsigned long long l; } u;
    u.d = m_data;
    return double_m(0ull != (u.l & 0x8000000000000000ull));
}
// setQnan {{{1
template<typename T> Vc_INTRINSIC void Vector<T>::setQnan()
{
    union { float f; unsigned int i; } u;
    u.i = 0xffffffffu;
    m_data = u.f;
}
template<> Vc_INTRINSIC void double_v::setQnan()
{
    union { double d; unsigned long long l; } u;
    u.l = 0xffffffffffffffffull;
    m_data = u.d;
}
template<typename T> Vc_INTRINSIC void Vector<T>::setQnan(Mask m)
{
    if (m) {
        setQnan();
    }
}
template<> Vc_INTRINSIC void double_v::setQnan(Mask m)
{
    if (m) {
        setQnan();
    }
}
// }}}1
}
// simd_cast {{{1
template <typename Return, typename T>
inline Return simd_cast(Scalar::Vector<T> x)
{
    return static_cast<Return>(x);
}
// }}}1
}
#include "undomacros.h"
// vim: foldmethod=marker
