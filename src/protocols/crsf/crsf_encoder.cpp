// Location: src/protocols/crsf/crsf_encoder.cpp

#include <algorithm>
#include <cstdint>
namespace CRSF {
void encode_channels(const int16_t *input_16bit, uint8_t *output_22byte) {
    uint32_t val = 0;
    int bits = 0, pos = 0;
    for (int i = 0; i < 16; i++) {
        float norm = (input_16bit[i] + 32768.0f) / 65535.0f;
        uint32_t c = static_cast<uint32_t>(172 + (norm * 1639));
        c = std::clamp(c, 172u, 1811u) & 0x07FF;
        val |= (c << bits);
        bits += 11;
        while (bits >= 8) {
            output_22byte[pos++] = val & 0xFF;
            val >>= 8;
            bits -= 8;
        }
    }
}
} // namespace CRSF