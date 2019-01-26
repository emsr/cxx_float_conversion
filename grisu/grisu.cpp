
#include "diy_fp.h"

#include <cstdint>
#include <cstdio>
#include <cmath>

constexpr double C_1_LOG2_10 = 0.30102999566398114; // 1/lg(10)

int
k_comp(int e, int alpha, int gamma)
{
  return std::ceil((alpha - e + 63) * C_1_LOG2_10);
}

constexpr int
ten[9]
{
          1,
	 10,
	100,
       1000,
      10000,
    1000000,
   10000000,
  100000000,
 1000000000,
};

void
cut(diy_fp D, uint32_t parts[3])
{
  parts[2] = (D.f % (ten[7] >> D.e)) << D.e;
  uint64_t tmp = D.f / (ten[7] >> D.e);
  parts[1] = tmp % ten[7];
  parts[0] = tmp / ten[7];
}

void
grisu(double v, char* buffer)
{
  diy_fp w;
  uint32_t ps[3];
  int q = 64, alpha = 0, gamma = 3;
  w = normalize diy_fp(v);
  int mk = k_comp(w.e + q, alpha, gamma);
  diy_fp c_mk = cached_power(mk);
  diy_fp D = w * c_mk;
  cut(D, ps);
  sprintf(buffer, "%u%07u%07ue%d", ps[0], ps[1], ps[2], -mk);
}

int
digit_gen_no_div(diy_fp D, char* buffer)
{
  int i = 0, q = 64;
  diy_fp one;
  one.f = ((uint64_t) 1) << -D.e; one.e = D.e;
  buffer[i++] = '0' + (D.f >> -one.e); // division
  uint64_t f = D.f & (one.f - 1); // modulo
  buffer[i++] = '.';
  while (-one.e > q - 5)
    {
      uint64_t tmp = (f << 2) & (one.f - 1);
      int d = f >> (-one.e - 3);
      d &= 6;
      f = f + tmp;
      d += f >> (-one.e - 1);
      buffer[i++] = '0' + d;
      one.e++;
      one.f >>= 1;
      f &= one.f - 1;
    }
  while (i < 19)
    {
      f *= 10;
      buffer[i++] = '0' + (f >> -one.e);
      f &= one.f - 1;
    }
  return i;
}

int
digit_gen_mix(diy_fp D, char* buffer)
{
  diy_fp one;
  one.f = ((uint64_t)1) << -D.e;
  one.e = D.e;
  uint32_t part1 = D.f >> -one.e;
  uint64_t f = D.f & (one.f - 1);
  int i = sprintf(buffer, "%u", part1);
  buffer[i++] = '.';
  while (i < 19)
    {
      f *= 10;
      buffer[i++] = '0' + (f >> -one.e);
      f &= one.f - 1;
    }
  return i;
}

void
digit_gen(diy_fp Mp, diy_fp delta,
	  char* buffer, int* len, int* K)
{
  uint32_t div;
  int d;
  diy_fp one;
  one.f = ((uint64_t) 1) << -Mp.e;
  one.e = Mp.e;
  uint32_t p1 = Mp.f >> -one.e;
  uint64_t p2 = Mp.f & (one.f - 1);
  *len = 0;
  int kappa = 10;
  div = ten[9];
  while (kappa > 0)
    {
      d = p1 / div;
      if (d || *len)
	buffer[(*len)++] = '0' + d;
      p1 %= div;
      --kappa;
      div /= 10;
      if ((((uint64_t)p1) << -one.e) + p2 <= delta.f)
	{
	  *K += kappa;
	  return;
	}
    }
  do
    {
      p2 *= 10;
      d = p2 >> -one.e;
      if (d || *len)
	buffer[(*len)++] = '0' + d;
      p2 &= one.f - 1;
      --kappa;
      delta.f *= 10;
    }
  while (p2 > delta.f);
  *K += kappa;
}

bool
round_weed(char* buffer, int len,
	   uint64_t wp_W, uint64_t Delta,
	   uint64_t rest, uint64_t ten_kappa,
	   uint64_t ulp)
{
  uint64_t wp_Wup = wp_W - ulp;
  uint64_t wp_Wdown = wp_W + ulp;
  while (rest < wp_Wup
      && Delta - rest >= ten_kappa
      && (rest + ten_kappa < wp_Wup
       || wp_Wup - rest >= rest + ten_kappa - wp_Wup))
    {
      buffer[len-1]--;
      rest += ten_kappa;
    }
  if (rest < wp_Wdown
      && Delta - rest >= ten_kappa
      && (rest + ten_kappa < wp_Wdown
       || wp_Wdown - rest > rest + ten_kappa - wp_Wdown))
    return false;
  return 2 * ulp <= rest && rest <= Delta - 4 * ulp;
}
