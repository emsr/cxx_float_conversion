#include <cmath>
#include <limits>

#include <iostream>
#include <iomanip>

template<typename _Float>
  void
  test_nexttoward()
  {
    std::cout << std::setprecision(std::numeric_limits<_Float>::max_digits10);
    const auto w = 6 + std::cout.precision();

    _Float minf = -std::numeric_limits<_Float>::infinity();
    _Float pinf = +std::numeric_limits<_Float>::infinity();
    _Float mmax = -std::numeric_limits<_Float>::max();
    _Float pmax = +std::numeric_limits<_Float>::max();
    _Float mzero = -_Float(0);
    _Float pzero = +_Float(0);
    _Float m42 = -_Float(42);
    _Float p42 = +_Float(42);

    _Float next;

    next = std::nexttoward(mmax, minf);
    std::cout << " next = " << std::setw(w) << next << '\n';

    next = std::nexttoward(pmax, pinf);
    std::cout << " next = " << std::setw(w) << next << '\n';

    next = std::nexttoward(mzero, minf);
    std::cout << " next = " << std::setw(w) << next << '\n';

    next = std::nexttoward(pzero, pinf);
    std::cout << " next = " << std::setw(w) << next << '\n';

    next = std::nexttoward(m42, minf);
    std::cout << " next = " << std::setw(w) << next << '\n';

    next = std::nexttoward(p42, pinf);
    std::cout << " next = " << std::setw(w) << next << '\n';
  }

int
main()
{
  test_nexttoward<float>();
  test_nexttoward<double>();
  test_nexttoward<long double>();
}
