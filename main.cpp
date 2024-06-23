#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <fmt/core.h>



int main(){
    // serialization
    // int is 32bits
    int x = 100; // how is int represented? 3bytes*0 and 01100100
    uint8_t b1 = x & (0b11111111);
    fmt::println("byte 1: {:b}", b1);
    uint8_t b2 = (x >> 8) & (0b11111111);
    fmt::println("byte 2: {:b}", b2);

    uint8_t* p = (uint8_t*) &x;
    fmt::println("memory: {:b} {:b} {:b} {:b}", p[0], p[1], p[2], p[3]);
    // fmt::println("memory: {:b} {:b} {:b} {:b}", *(p+0), *(p+1), p[2], p[3]);

    char buffer[1024]; // char = byte

    // save x as binary into buffer

    // print buffer as hex to console

    const char* s = "hello world";

    printf("%s\n", s);

    return 0;
}