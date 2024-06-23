#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <fmt/core.h>

namespace kleardb {

// serialize in big-endian
void serialize(uint32_t x, uint8_t* p_buf) {
    p_buf[0] = (x >> 24) & 0xFF;
    p_buf[1] = (x >> 16) & 0xFF;
    p_buf[2] = (x >>  8) & 0xFF;
    p_buf[3] = (x >>  0) & 0xFF;

    // memcpy(&x, p_buf, sizeof(x)); // this does not work because of endianness
}

// serialize string
void serialize(std::string s, uint8_t* p_buf) {
    int len = s.size();

    serialize(len, p_buf);
    p_buf += sizeof(len);

    for (int i = 0; i < len; i++) {
        p_buf[i] = s[i];
    }

    // memcpy(s, p_buf, len);
}

// deserialize in big-endian
uint32_t deserialize_slow(uint8_t* p_buf) {
    uint32_t x = 0;
    for (int i = 0; i < 4; i++) {
        x += (uint32_t)p_buf[i] * std::pow(2,4-i-1);
    }
    return x;

    // memcpy(&x, p_buf, sizeof(x)); // this does not work because of endianness
}

// deserialize in big endian
uint32_t deserialize(uint8_t* p_buf) {
    uint32_t x = 0;
    x |= ((uint32_t)p_buf[3] <<  0);
    x |= ((uint32_t)p_buf[2] <<  8);
    x |= ((uint32_t)p_buf[1] << 16);
    x |= ((uint32_t)p_buf[0] << 24);

    return x;

    // memcpy(&x, p_buf, sizeof(x)); // this does not work because of endianness
}

}

void test_serialization() {
    uint8_t buf[1024];

    int x = 100;
    uint8_t* p = (uint8_t*) &x;
    fmt::println("memory: {:b} {:b} {:b} {:b}", p[0], p[1], p[2], p[3]);

    kleardb::serialize(x, buf);
    fmt::println("big endian serialized: {:b} {:b} {:b} {:b}", buf[0], buf[1], buf[2], buf[3]);

    fmt::println("big endian deserialized (slow): {:d}", kleardb::deserialize_slow(buf));
    
    fmt::println("big endian deserialized: {:d}", kleardb::deserialize(buf));

}


int main(){
    test_serialization();
}