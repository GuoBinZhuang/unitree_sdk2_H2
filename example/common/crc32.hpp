#pragma once

#include <cstdint>

namespace unitree::common {

// CRC-32 (polynomial 0x04c11db7) over `len` 32-bit words, as expected by the
// `crc` field of the low level DDS command messages.
inline uint32_t Crc32Core(const uint32_t *ptr, uint32_t len) {
  uint32_t CRC32 = 0xFFFFFFFF;
  const uint32_t dwPolynomial = 0x04c11db7;

  for (uint32_t i = 0; i < len; i++) {
    uint32_t xbit = 1 << 31;
    uint32_t data = ptr[i];
    for (uint32_t bits = 0; bits < 32; bits++) {
      if (CRC32 & 0x80000000) {
        CRC32 <<= 1;
        CRC32 ^= dwPolynomial;
      } else {
        CRC32 <<= 1;
      }
      if (data & xbit) {
        CRC32 ^= dwPolynomial;
      }
      xbit >>= 1;
    }
  }
  return CRC32;
}

}  // namespace unitree::common
