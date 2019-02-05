#include <cmath>
#include <limits>
#include <bit>

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
      _M_mant = _S_bits * std::abs(std::frexp(__x, &_M_exp));
    }

    _Float
    to_Float() const
    {
      return (_M_sign ? -1 : +1)
	   * std::ldexp(_Float(_M_mant), _M_exp - _S_prec);
    }

    void
    zoom()
    {
      _M_exp -= 2; // FIXME: Watch subnorm.
      _M_mant *= 4;
    }

    bool
    is_zoomed()
    { return std::log2p1(_M_mant) > _S_prec; }
  };

template<typename _Float>
  void
  test_ryu()
  {
    std::cout << std::setprecision(std::numeric_limits<_Float>::digits10);
    const auto w = 6 + std::cout.precision();
    _Float pi_d = 3.141592654;
    auto pi = _Float_thing<_Float>(pi_d);
    std::cout << " pi = " << std::setw(w) << pi_d << '\n';
    std::cout << " pi = " << pi._M_sign << " " << pi._M_exp << " 0x" << std::hex << pi._M_mant << '\n';
    std::cout << " pi = " << std::setw(w) << pi.to_Float() << '\n';

    pi.zoom();
    std::cout << " pi.is_zoomed() : " << pi.is_zoomed() << '\n';
    std::cout << " pi = " << pi._M_sign << " " << pi._M_exp << " 0x" << std::hex << pi._M_mant << '\n';
  }

int
main()
{
  test_ryu<float>();
  test_ryu<double>();
  //test_ryu<long double>();
}

