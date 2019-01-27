#include <cmath>
#include <limits>

#include <iostream>
#include <iomanip>

template<typename _Float>
  void
  test_frexp()
  {
    std::cout << std::setprecision(std::numeric_limits<_Float>::max_digits10);
    const auto w = 6 + std::cout.precision();

    _Float mmax = -std::numeric_limits<_Float>::max();
    _Float pmax = +std::numeric_limits<_Float>::max();
    _Float mzero = -_Float(0);
    _Float pzero = +_Float(0);
    _Float m42 = -_Float(42);
    _Float p42 = +_Float(42);

    int exp;
    _Float frac;

    frac = std::frexp(mmax, &exp);
    std::cout << " frac = " << std::setw(w) << frac
              << " exp = " << std::setw(3) << exp << '\n';

    frac = std::frexp(pmax, &exp);
    std::cout << " frac = " << std::setw(w) << frac
              << " exp = " << std::setw(3) << exp << '\n';

    frac = std::frexp(mzero, &exp);
    std::cout << " frac = " << std::setw(w) << frac
              << " exp = " << std::setw(3) << exp << '\n';

    frac = std::frexp(pzero, &exp);
    std::cout << " frac = " << std::setw(w) << frac
              << " exp = " << std::setw(3) << exp << '\n';

    frac = std::frexp(m42, &exp);
    std::cout << " frac = " << std::setw(w) << frac
              << " exp = " << std::setw(3) << exp << '\n';

    frac = std::frexp(p42, &exp);
    std::cout << " frac = " << std::setw(w) << frac
              << " exp = " << std::setw(3) << exp << '\n';
  }

int
main()
{
  test_frexp<float>();
  test_frexp<double>();
  test_frexp<long double>();
}
