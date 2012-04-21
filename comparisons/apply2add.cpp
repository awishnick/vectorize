#include "vectorize.h"
using namespace vectorize;

void add2_byhand(unsigned n, const float* src1,
                 const float* src2, float* dst) {
  for (; n>=4; n-=4, src1+=4, src2+=4, dst+=4) {
    _mm_store_ps(dst, _mm_add_ps(_mm_load_ps(src1),
                                 _mm_load_ps(src2)));
  }
  for (; n; --n, ++src1, ++src2, ++dst) {
    *dst= *src1 + *src2;
  }
}

void add2_vectorize(unsigned n, const float* src1,
                    const float* src2, float* dst) {
  apply2(n, src1, src2, dst, _x + _y);
}
