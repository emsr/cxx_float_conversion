// Implementation of std::to_chars and std::from_chars -*- C++ -*-

// Copyright (C) 2017 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/bits/string_conv.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{utility}
 */

#ifndef _GLIBCXX_STRING_CONV_H
#define _GLIBCXX_STRING_CONV_H 1

#pragma GCC system_header

#if __cplusplus < 201402L
# include <bits/c++14_warning.h>
#else

#include <system_error>
#include <limits>
#include <cctype>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  struct to_chars_result {
    char* ptr;
    error_code ec;
  };

  struct from_chars_result {
    const char* ptr;
    error_code ec;
  };

_GLIBCXX_END_NAMESPACE_VERSION
namespace __detail{
_GLIBCXX_BEGIN_NAMESPACE_VERSION
  template<typename _Tp, typename... _Types>
    using __is_one_of = __or_<is_same<_Tp, _Types>...>;

  template<typename _Tp>
    using __is_to_chars_type = __and_<is_integral<_Tp>,
	  __not_<__is_one_of<_Tp, bool, char16_t, char32_t
#if _GLIBCXX_USE_WCHAR_T
	  , wchar_t
#endif
	    >>>;

  template<typename _Tp>
    using __to_chars_result_type
      = enable_if_t<__is_to_chars_type<_Tp>::value, to_chars_result>;

  template<typename _Tp>
    using __unsigned_least_t
      = conditional_t<(sizeof(_Tp) <= sizeof(int)), unsigned int,
	conditional_t<(sizeof(_Tp) <= sizeof(long)), unsigned long,
	conditional_t<(sizeof(_Tp) <= sizeof(long long)), unsigned long long,
#if _GLIBCXX_USE_INT128
	conditional_t<(sizeof(_Tp) <= sizeof(__int128)), unsigned __int128,
#endif
	void>>>>;

  template<typename _Tp>
    constexpr unsigned
    __to_chars_len(_Tp __value, int __base = 10)
    {
      static_assert(is_integral<_Tp>::value, "implementation bug");
      static_assert(is_unsigned<_Tp>::value, "implementation bug");

      unsigned __n = 1;
      const int __b2 = __base  * __base;
      const int __b3 = __b2 * __base;
      const int __b4 = __b3 * __base;
      for (;;)
	{
	  if (__value < __base) return __n;
	  if (__value < __b2) return __n + 1;
	  if (__value < __b3) return __n + 2;
	  if (__value < __b4) return __n + 3;
	  __value /= (unsigned)__b4;
	  __n += 4;
	}
    }

  template<typename _Tp>
    to_chars_result
    __to_chars(char* __first, char* __last, _Tp __val, int __base)
    {
      static_assert(is_integral<_Tp>::value, "implementation bug");
      static_assert(is_unsigned<_Tp>::value, "implementation bug");

      constexpr char __digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

      to_chars_result __res;

      const unsigned __len = __to_chars_len(__val, __base);

      if (__builtin_expect((__last - __first) < __len, 0))
	{
	  __res.ptr = __last;
	  __res.ec = std::make_error_code(errc::value_too_large);
	  return __res;
	}

      unsigned __pos = __len - 1;

      if (__base == 10)
	{
	  constexpr char __digits[201] =
	    "0001020304050607080910111213141516171819"
	    "2021222324252627282930313233343536373839"
	    "4041424344454647484950515253545556575859"
	    "6061626364656667686970717273747576777879"
	    "8081828384858687888990919293949596979899";
	  while (__val >= 100)
	    {
	      auto const __num = (__val % 100) * 2;
	      __val /= 100;
	      __first[__pos] = __digits[__num + 1];
	      __first[__pos - 1] = __digits[__num];
	      __pos -= 2;
	    }
	  if (__val >= 10)
	    {
	      auto const __num = __val * 2;
	      __first[__pos] = __digits[__num + 1];
	      __first[__pos - 1] = __digits[__num];
	    }
	  else
	    __first[__pos] = '0' + __val;
	}
      else if (__base == 8)
	{
	  constexpr char __digits[129] =
	    "00010203040506071011121314151617"
	    "20212223242526273031323334353637"
	    "40414243444546475051525354555657"
	    "60616263646566677071727374757677";
	  while (__val >= 64)
	    {
	      auto const __num = (__val % 64) * 2;
	      __val /= 64;
	      __first[__pos] = __digits[__num + 1];
	      __first[__pos - 1] = __digits[__num];
	      __pos -= 2;
	    }
	  if (__val >= 8)
	    {
	      auto const __num = __val * 2;
	      __first[__pos] = __digits[__num + 1];
	      __first[__pos - 1] = __digits[__num];
	    }
	  else
	    __first[__pos] = '0' + __val;
	}
      else if (__base == 2)
	{
	  while (__pos)
	    {
	      __first[__pos--] = '0' + (__val & 1);
	      __val >>= 1;
	    }
	  *__first = '0' + (__val & 1);
	}
      else
	{
	  while (__val >= __base)
	    {
	      auto const __quo = __val / __base;
	      auto const __rem = __val % __base;
	      __first[__pos--] = __digits[__rem];
	      __val = __quo;
	    }
	  *__first = __digits[__val];
	}
      __res.ptr = __first + __len;
      return __res;
    }
_GLIBCXX_END_NAMESPACE_VERSION
}
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<typename _Tp>
    __detail::__to_chars_result_type<_Tp>
    to_chars(char* __first, char* __last, _Tp __value, int __base = 10)
    {
      __glibcxx_assert(2 <= __base && __base <= 36);

      using _Up = __detail::__unsigned_least_t<_Tp>;
      _Up __unsigned_val = __value;

      if _GLIBCXX17_CONSTEXPR (std::is_signed<_Tp>::value)
	if (__value < 0)
	  {
	    if (__builtin_expect(__first != __last, 1))
	      *__first++ = '-';
	    __unsigned_val = _Up(~__value) + _Up(1);
	  }

      return __detail::__to_chars(__first, __last, __unsigned_val, __base);
    }

_GLIBCXX_END_NAMESPACE_VERSION
namespace __detail {
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<typename _Tp>
    bool
    __raise_and_add(_Tp& __val, int __base, unsigned char __c)
    {
      if (__builtin_mul_overflow(__val, __base, &__val)
	  || __builtin_add_overflow(__val, __c, &__val))
	return false;
      return true;
    }

  template<typename _Tp>
    bool
    __from_chars_binary(const char*& __first, const char* __last, _Tp& __val)
    {
      static_assert(is_integral<_Tp>::value, "implementation bug");
      static_assert(is_unsigned<_Tp>::value, "implementation bug");

      const ptrdiff_t __len = __last - __first;
      int __i = 0;
      while (__i < __len)
	{
	  if (__first[__i] == '0')
	      __val <<= 1;
	  else if (__first[__i] == '1')
	      (__val <<= 1) |= 1;
	  else
	    break;
	  __i++;
	}
      __first += __i;
      return __i <= (sizeof(_Tp) * __CHAR_BIT__);
    }

  template<typename _Tp>
    bool
    __from_chars_digit(const char*& __first, const char* __last, _Tp& __val,
		       int __base)
    {
      static_assert(is_integral<_Tp>::value, "implementation bug");
      static_assert(is_unsigned<_Tp>::value, "implementation bug");

      auto __matches = [__base](char __c) {
	  return '0' <= __c && __c <= ('0' + (__base - 1));
      };

      while (__first != __last)
	{
	  const char __c = *__first;
	  if (__matches(__c))
	  {
	    if (!__raise_and_add(__val, __base, __c - '0'))
	      {
		while (++__first != __last && __matches(*__first))
		  ;
		return false;
	      }
	    __first++;
	  }
	  else
	    return true;
	}
      return true;
    }

  constexpr bool __consecutive_chars(const char* __s, int __n)
  {
    for (int __i = 1; __i < __n; ++__i)
      if (__s[__i] != (__s[__i-1] + 1))
	return false;
    return true;
  }

  template<typename _Tp>
    bool
    __from_chars_alnum(const char*& __first, const char* __last, _Tp& __val,
		       int __base)
    {
      const int __b = __base - 10;
      bool __valid = true;
      while (__first != __last)
	{
	  unsigned char __c = *__first;
	  if (std::isdigit(__c))
	    __c -= '0';
	  else
	    {
	      constexpr char __abc[] = "abcdefghijklmnopqrstuvwxyz";
	      unsigned char __lc = std::tolower(__c);
	      constexpr bool __consecutive = __consecutive_chars(__abc, 26);
	      if _GLIBCXX17_CONSTEXPR (__consecutive)
		{
		  // Characters 'a'..'z' are consecutive
		  if (std::isalpha(__c) && (__lc - 'a') < __b)
		    __c = __lc - 'a' + 10;
		  else
		    break;
		}
	      else
		{
		  if (auto __p = __builtin_memchr(__abc, __lc, __b))
		    __c = static_cast<const char*>(__p) - __abc;
		  else
		    break;
		}
	    }

	  if (__builtin_expect(__valid, 1))
	    __valid = __raise_and_add(__val, __base, __c);
	  __first++;
	}
      return __valid;
    }

  template<typename _Tp>
    using __from_chars_result_type
      = enable_if_t<__is_to_chars_type<_Tp>::value, from_chars_result>;

_GLIBCXX_END_NAMESPACE_VERSION
}
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<typename _Tp>
    __detail::__from_chars_result_type<_Tp>
    from_chars(const char* __first, const char* __last, _Tp& __value,
	       int __base = 10)
    {
      __glibcxx_assert(2 <= __base && __base <= 36);

      from_chars_result __res{__first, {}};

      int __sign = 1;
      if _GLIBCXX17_CONSTEXPR (std::is_signed<_Tp>::value)
	if (__first != __last && *__first == '-')
	  {
	    __sign = -1;
	    ++__first;
	  }

      using _Up = __detail::__unsigned_least_t<_Tp>;
      _Up __val = 0;

      const auto __start = __first;
      bool __valid;
      if (__base == 2)
	__valid = __detail::__from_chars_binary(__first, __last, __val);
      else if (__base <= 10)
	__valid = __detail::__from_chars_digit(__first, __last, __val, __base);
      else
	__valid = __detail::__from_chars_alnum(__first, __last, __val, __base);

      if (__builtin_expect(__first == __start, 0))
	__res.ec = std::make_error_code(errc::invalid_argument);
      else
	{
	  __res.ptr = __first;
	  if (!__valid)
	    __res.ec = std::make_error_code(errc::result_out_of_range);
	  else
	    {
	      if _GLIBCXX17_CONSTEXPR (std::is_signed<_Tp>::value)
		{
		  _Tp __tmp;
		  if (__builtin_mul_overflow(__val, __sign, &__tmp))
		    __res.ec = make_error_code(errc::result_out_of_range);
		  else
		    __value = __tmp;
		}
	      else
		{
		  if _GLIBCXX17_CONSTEXPR
		    (numeric_limits<_Up>::max() > numeric_limits<_Tp>::max())
		    {
		      if (__val > numeric_limits<_Tp>::max())
			__res.ec = make_error_code(errc::result_out_of_range);
		      else
			__value = __val;
		    }
		  else
		    __value = __val;
		}
	    }
	}

      return __res;
    }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#endif // C++11

#endif // _GLIBCXX_STRING_CONV_H
