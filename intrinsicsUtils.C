#ifdef __SSE2__
static int hor_m128i(__m128i mask4) {
    __m128i tmp = _mm_shuffle_epi32(mask4, _MM_SHUFFLE(2, 3, 0, 1));
    tmp = _mm_or_si128(mask4, tmp);
    mask4 = tmp;
    tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(1, 0, 3, 2));
    mask4 = _mm_or_si128(mask4, tmp);   // all elements now equal to the hor

    int mask = _mm_cvtsi128_si32(mask4); // return zeroth element
    return mask;
}
#endif

#ifdef __AVX__
//adapted from function found in StackOverflow article "How to sum __m256 horizontally"
static bool hor_m256(__m256 mask8) {
    __m128 half0 = _mm256_extractf128_ps(mask8, 0);
    __m128 half1 = _mm256_extractf128_ps(mask8, 1);
    __m128 orQuad = _mm_or_ps(half0, half1);
    // sum holds 3+7, 2+6, 1+5, 0+4
    __m128 loDual = orQuad;
    __m128 hiDual = _mm_movehl_ps(orQuad, orQuad);
    __m128 orDual = _mm_or_ps(loDual, hiDual);
    __m128 lo = orDual;
    __m128 hi = _mm_shuffle_ps(orDual, orDual, 0x1);
    __m128 hor = _mm_or_ps(lo, hi);
    return !(_mm_cvtss_f32(hor) == 0);
}
#endif

#ifdef __SSE2__
void debugPrint_m128i(__m128i reg) {
    printf("%i %i %i %i\n", ((int*)(&reg))[3], ((int*)(&reg))[2], ((int*)(&reg))[1], ((int*)(&reg))[0]);
}

void debugPrint_m128(__m128 reg) {
    printf("%f %f %f %f\n", ((float*)(&reg))[3],  ((float*)(&reg))[2],  ((float*)(&reg))[1],  ((float*)(&reg))[0]); 
}
#endif

#ifdef __AVX__
void debugPrint_m256(__m256 reg) {
    printf("%f %f %f %f %f %f %f %f\n", ((float*)(&reg))[7], ((float*)(&reg))[6], ((float*)(&reg))[5], ((float*)(&reg))[4], ((float*)(&reg))[3], ((float*)(&reg))[2], ((float*)(&reg))[1], ((float*)(&reg))[0]);
} 
#endif


