#include <cmath>
#include <limits>

#include <iostream>
#include <iomanip>

template<typename _Float>
  struct _Float_thing
  {
    using _ExpTp = int;
    using _MantTp = unsigned long long;

    static constexpr auto    _S_prec = std::numeric_limits<_Float>::digits;
    static constexpr _MantTp _S_bits = std::exp2(_S_prec);

    bool    _M_sign = false;
    _ExpTp  _M_exp = 0;
    _MantTp _M_mant = 0;

    _Float_thing(_Float __x)
    : _M_sign(std::signbit(__x))
    {
      _M_mant = _S_bits * std::frexp(__x, &_M_exp);
    }

    _Float
    to_Float() const
    {
      return std::ldexp(_Float(_M_mant), -_S_prec);
    }
  };

int
main()
{
  auto pi = _Float_thing<double>(3.141592654);
  std::cout << " pi = " << pi._M_sign << " " << pi._M_exp << " 0x" << std::hex << pi._M_mant << '\n';
  std::cout << " pi = " << pi.to_Float() << '\n';
}

