#include "simde/x86/avx.h"

// https://github.com/mjmacleod/sse2neon/blob/95e80e13a96f66f38a083cce044e79e0ad11af1c/sse2neon.h#L2719-L2725
inline __m128i _mm_aesenc_si128(__m128i a, __m128i b) {
    return vreinterpretq_s64_u8(
        vaesmcq_u8(vaeseq_u8(vreinterpretq_u8_s64(a), uint8x16_t{})) ^
        vreinterpretq_u8_s64(b));
}

// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_aesenclast_si128
// https://developer.arm.com/architectures/instruction-sets/intrinsics/vaeseq_u8
inline __m128i _mm_aesenclast_si128(__m128i a, __m128i b) {
    return vreinterpretq_s64_u8(
        vaeseq_u8(vreinterpretq_u8_s64(a), uint8x16_t{}) ^
        vreinterpretq_u8_s64(b));
}

// https://github.com/microsoft/SymCrypt/blob/e875f1f957dcb1308f8e712e9f4a8edc6f4f6207/lib/aes-neon.c#L16-L41
inline uint32_t __neon_sbox_lookup(uint32_t in) {
    uint8x16_t x = vreinterpretq_u8_u32(vdupq_n_u32(in));
    x = vaeseq_u8(x, vdupq_n_u8(0));
    return vgetq_lane_u32(vreinterpretq_u32_u8(x), 0);
}

inline __m128i _mm_aeskeygenassist_si128(__m128i a, const int imm8) {
    /*
    X3[31:0] := a[127:96]
    X2[31:0] := a[95:64]
    X1[31:0] := a[63:32]
    X0[31:0] := a[31:0]
    RCON[31:0] := ZeroExtend32(imm8[7:0])
    dst[31:0] := SubWord(X1)
    dst[63:32] := RotWord(SubWord(X1)) XOR RCON
    dst[95:64] := SubWord(X3)
    dst[127:96] := RotWord(SubWord(X3)) XOR RCON
    */
    // zero extend
    uint32_t rcon = (uint32_t)((uint8_t)imm8);

    // S-box lookup
    uint32_t dst0 = __neon_sbox_lookup(vgetq_lane_u32(vreinterpretq_u32_s64(a), 1));
    uint32_t dst1 = dst0;
    // RotWord
    // https://en.wikipedia.org/wiki/AES_key_schedule
    dst1 = (dst1 & 0x000000ff) << 24 | ((dst1 >> 8) & (0x00ffffff));
    // xor
    dst1 ^= rcon;
    // S-box lookup
    uint32_t dst2 = __neon_sbox_lookup(vgetq_lane_u32(vreinterpretq_u32_s64(a), 3));
    uint32_t dst3 = dst2;
    // RotWord
    dst3 = (dst3 & 0x000000ff) << 24 | ((dst3 >> 8) & (0x00ffffff));
    // xor
    dst3 ^= rcon;

    return simde_mm_set_epi32(dst3, dst2, dst1, dst0);
}