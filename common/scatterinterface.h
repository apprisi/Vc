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

///////////////////////////////////////////////////////////////////////////////////////////
// scatters
// A scatter takes the following arguments:
// 1. A pointer to memory of any type that EntryType can convert to.
// 2. An indexes “vector”. The requirement is that the type implements the subscript operator,
//    stores «Size» valid index values, and each offset to the pointer above yields a valid
//    memory location for reading.
// 3. Optionally the third argument may be a mask. The mask disables several memory stores and
//    thus removes the requirements in (2.) for the disabled entries.

private:
    // enable_if<std::can_convert<MT, EntryType>::value && has_subscript_operator<IT>::value>
    template <typename MT, typename IT>
    inline void scatterImplementation(MT *mem, IT &&indexes) const;

    template <typename MT, typename IT>
    inline void scatterImplementation(MT *mem, IT &&indexes, MaskArgument mask) const;

public:
    #define VC_ASSERT_SCATTER_PARAMETER_TYPES__                                                    \
        static_assert(std::is_convertible<EntryType, MT>::value,                                   \
                      "The memory pointer needs to point to a type that the EntryType of this "    \
                      "SIMD vector type can be converted to.");                                    \
        static_assert(                                                                             \
            Vc::Traits::has_subscript_operator<IT>::value,                                         \
            "The indexes argument must be a type that implements the subscript operator.");        \
        static_assert(!Traits::is_simd_vector<IT>::value || Traits::VectorSize<IT>::value >= Size, \
                      "If you use a SIMD vector for the indexes parameter, the index vector must " \
                      "have at least as many entries as this SIMD vector.");                       \
        static_assert(!std::is_array<T>::value ||                                                  \
                          (std::rank<T>::value == 1 &&                                             \
                           (std::extent<T>::value == 0 || std::extent<T>::value >= Size)),         \
                      "If you use a simple array for the indexes parameter, the array must have "  \
                      "at least as many entries as this SIMD vector.")

    template <typename MT,
              typename IT,
              typename = enable_if<Vc::Traits::has_subscript_operator<IT>::value>>
    Vc_INTRINSIC void scatter(MT *mem, IT &&indexes) const
    {
        VC_ASSERT_SCATTER_PARAMETER_TYPES__;
        scatterImplementation(mem, std::forward<IT>(indexes));
    }

    template <typename MT,
              typename IT,
              typename = enable_if<Vc::Traits::has_subscript_operator<IT>::value>>
    Vc_INTRINSIC void scatter(MT *mem, IT &&indexes, MaskArgument mask) const
    {
        VC_ASSERT_SCATTER_PARAMETER_TYPES__;
        scatterImplementation(mem, std::forward<IT>(indexes), mask);
    }

    template <typename MT, typename IT>
    Vc_INTRINSIC void scatter(const Common::ScatterArguments<MT, IT> &args) const
    {
        scatter(args.address, args.indexes);
    }

    template <typename MT, typename IT>
    Vc_INTRINSIC void scatter(const Common::ScatterArguments<MT, IT> &args, MaskArgument mask) const
    {
        scatter(args.address, args.indexes, mask);
    }
#undef VC_ASSERT_SCATTER_PARAMETER_TYPES__
