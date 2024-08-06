#include <cstdint>
#include <iostream>
#include <stdio.h>

namespace kleardb {

// serialize in big-endian
size_t serialize_u32(uint32_t x, uint8_t* p_buf) {
    p_buf[0] = (x >> 24) & 0xFF; //0x: we can use hexadecimal notation, 0xFF = 0b1111 1111 
    p_buf[1] = (x >> 16) & 0xFF;
    p_buf[2] = (x >>  8) & 0xFF;
    p_buf[3] = (x >>  0) & 0xFF;

    return 4;
    // memcpy(&x, p_buf, sizeof(x)); // this does not work because of endianness
}

// deserialize in big endian
size_t deserialize_u32(uint32_t* x, uint8_t* p_buf) {
    *x = 0;
    *x |= ((uint32_t)p_buf[3] <<  0);
    *x |= ((uint32_t)p_buf[2] <<  8);
    *x |= ((uint32_t)p_buf[1] << 16);
    *x |= ((uint32_t)p_buf[0] << 24);

    return 4;

    // memcpy(&x, p_buf, sizeof(x)); // this does not work because of endianness
}

size_t serialize_u64(uint64_t x, uint8_t* p_buf) {
    p_buf[0] = (x >> 56) & 0xFF; //0x: we can use hexadecimal notation, 0xFF = 0b1111 1111 
    p_buf[1] = (x >> 48) & 0xFF;
    p_buf[2] = (x >> 40) & 0xFF;
    p_buf[3] = (x >> 32) & 0xFF;
    p_buf[4] = (x >> 24) & 0xFF; //0x: we can use hexadecimal notation, 0xFF = 0b1111 1111 
    p_buf[5] = (x >> 16) & 0xFF;
    p_buf[6] = (x >>  8) & 0xFF;
    p_buf[7] = (x >>  0) & 0xFF;

    return 8;
    // memcpy(&x, p_buf, sizeof(x)); // this does not work because of endianness
}

size_t deserialize_u64(uint64_t* x, uint8_t* p_buf) {
    *x = 0;
    *x |= ((uint64_t)p_buf[7] <<  0);
    *x |= ((uint64_t)p_buf[6] <<  8);
    *x |= ((uint64_t)p_buf[5] << 16);
    *x |= ((uint64_t)p_buf[4] << 24);
    *x |= ((uint64_t)p_buf[3] << 32);
    *x |= ((uint64_t)p_buf[2] << 40);
    *x |= ((uint64_t)p_buf[1] << 48);
    *x |= ((uint64_t)p_buf[0] << 56);

    return 8;

    // memcpy(&x, p_buf, sizeof(x)); // this does not work because of endianness
}

// serialize string
size_t serialize_str(const std::string& s, uint8_t* p_buf) {
    uint64_t len = s.size();
    uint64_t len_of_len = serialize_u64(len, p_buf);
    p_buf += len_of_len;

    for (int i = 0; i < len; i++) {
        p_buf[i] = s[i];
    }

    return len + len_of_len;
}

size_t deserialize_str(std::string& out, uint8_t* p_buf) {
    uint64_t len;
    uint64_t len_of_len = deserialize_u64(&len, p_buf);
    p_buf += len_of_len;

    out.resize(len);

    for (int i = 0; i < len; i++) {
        out[i] = (char)p_buf[i];
    }

    return len + len_of_len;
}

// deserialize in big-endian
uint32_t deserialize_slow(uint8_t* p_buf) {
    uint32_t x = 0;
    for (int i = 0; i < 4; i++) {
        x += (uint32_t)p_buf[i] * std::pow(2,4-i-1);
    }
    return x;
}

}