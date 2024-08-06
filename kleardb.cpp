#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <fmt/core.h>
#include "kleardb/serialize.hpp"


void test_serialize_u32() {
    uint8_t buf[1024];

    uint32_t x = 100;
    uint8_t* p = (uint8_t*) &x;
    fmt::println("memory: {:b} {:b} {:b} {:b}", p[0], p[1], p[2], p[3]);

    kleardb::serialize_u32(x, buf);
    fmt::println("big endian serialized: {:b} {:b} {:b} {:b}", buf[0], buf[1], buf[2], buf[3]);

    fmt::println("big endian deserialized (slow): {:d}", kleardb::deserialize_slow(buf));
    
    kleardb::deserialize_u32(&x, buf);
    fmt::println("big endian deserialized: {:d}", x);

}

void test_serialize_u64() {
    uint8_t buf[1024];

    uint64_t x = 100;
    uint8_t* p = (uint8_t*) &x;
    fmt::println("memory: {:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

    kleardb::serialize_u64(x, buf);
    fmt::println("big endian serialized: {:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);

    uint64_t y;
    kleardb::deserialize_u64(&y, buf);
    fmt::println("big endian deserialized: {:d}", x);

}

void test_serialize_str() {
    uint8_t buf[1024];

    std::string s = "test that it works";
    
    kleardb::serialize_str(s, buf);
    fmt::println("serialized: {:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b} | {:c} {:c} {:c}", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[911]);

    std::string s_out;
    kleardb::deserialize_str(s_out, buf);
    fmt::println("deserialized: {}", s_out);
}


int main() {
    test_serialize_str();
}