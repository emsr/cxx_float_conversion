//

#include <cstdint>
#include <cassert>

  struct diy_fp
  {
    uint64_t f;
    int e;

    diy_fp&
    operator+=(diy_fp a);

    diy_fp&
    operator-=(diy_fp a);

    diy_fp&
    operator*=(diy_fp a);

    diy_fp&
    operator/=(diy_fp a);
  };

  diy_fp
  operator-(diy_fp x, diy_fp y)
  {
    assert(x.e == y.e && x.f >= y.f);
    diy_fp r = {.f = x.f - y.f, .e = x.e};
    return r;
  }

  diy_fp
  operator*(diy_fp x, diy_fp y)
  {
    //uint64_t a, b, c, d, ac, bc, ad, bd, tmp;
    constexpr uint64_t M32 = 0xFFFFFFFF;
    auto a = x.f >> 32;
    auto b = x.f & M32;
    auto c = y.f >> 32;
    auto d = y.f & M32;
    auto ac = a * c;
    auto bc = b * c;
    auto ad = a * d;
    auto bd = b * d;
    auto tmp = (bd >> 32) + (ad & M32) + (bc & M32);
    tmp += 1U << 31; // Round
    diy_fp r;
    r.f = ac + (ad >> 32) + (bc >> 32) + (tmp >> 32);
    r.e = x.e + y.e + 64;
    return r;
  }
