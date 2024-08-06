#include <gtest/gtest.h>
#include "kleardb/serialize.hpp"

// Demonstrate some basic assertions.
TEST(seralize_u32, basic) {
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

TEST(seralize_u32, basic) {

TEST(seralize_str, basic) {
  // Expect two strings not to be equal.
  uint8_t buf[1024];

  std::string s = "test that it works";
  kleardb::serialize_str(s, buf);
  std::string s_out;
  kleardb::deserialize_str(s_out, buf);
  EXPECT_STREQ(s.c_str(), s_out.c_str());
}