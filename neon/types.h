/*  This file is part of the Vc library. {{{
Copyright © 2014 Matthias Kretz <kretz@kde.org>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the names of contributing organizations nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

}}}*/

#ifndef VC_NEON_TYPES_H_
#define VC_NEON_TYPES_H_

#include "intrinsics.h"
#include "macros.h"

#ifdef VC_DEFAULT_IMPL_NEON
#define VC_DOUBLE_V_SIZE 1
#define VC_FLOAT_V_SIZE 4
#define VC_INT_V_SIZE 4
#define VC_UINT_V_SIZE 4
#define VC_SHORT_V_SIZE 8
#define VC_USHORT_V_SIZE 8
#endif

namespace Vc_VERSIONED_NAMESPACE
{
namespace NEON
{
constexpr std::size_t VectorAlignment = 16;

template <typename T> class Vector;
typedef Vector<double>         double_v;
typedef Vector<float>           float_v;
typedef Vector<int>               int_v;
typedef Vector<unsigned int>     uint_v;
typedef Vector<short>           short_v;
typedef Vector<unsigned short> ushort_v;

template <typename T> class Mask;
typedef Mask<double>         double_m;
typedef Mask<float>           float_m;
typedef Mask<int>               int_m;
typedef Mask<unsigned int>     uint_m;
typedef Mask<short>           short_m;
typedef Mask<unsigned short> ushort_m;

template <typename V = Vector<float>> class alignas(alignof(V)) VectorAlignedBaseT;

template <typename T> struct is_vector : public std::false_type {};
template <typename T> struct is_vector<Vector<T>> : public std::true_type {};
template <typename T> struct is_mask : public std::false_type {};
template <typename T> struct is_mask<Mask<T>> : public std::true_type {};

template <typename T> struct VectorTraits;
template <> struct VectorTraits<int> { typedef int32x4_t Type; };
template <> struct VectorTraits<unsigned int> { typedef uint32x4_t Type; };
template <> struct VectorTraits<short> { typedef int16x8_t Type; };
template <> struct VectorTraits<unsigned short> { typedef uint16x8_t Type; };
template <> struct VectorTraits<float> { typedef float32x4_t Type; };
template <> struct VectorTraits<double> { typedef double Type; };
}

namespace Traits
{
template<typename T> struct is_simd_mask_internal<NEON::Mask<T>> : public std::true_type {};
template<typename T> struct is_simd_vector_internal<NEON::Vector<T>> : public std::true_type {};
}
}

#include "undomacros.h"

#endif  // VC_NEON_TYPES_H_

// vim: foldmethod=marker
