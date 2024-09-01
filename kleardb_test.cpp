#include <gtest/gtest.h>
#include "kleardb/serialize.hpp"
#include "fmt/core.h"
#include "kleardb/Schema.hpp"

// Demonstrate some basic assertions.
TEST(seralize_u32, basic) {
  uint8_t buf[1024];

  uint32_t x = 100;
  uint8_t* p = (uint8_t*) &x;
  fmt::println("memory: {:b} {:b} {:b} {:b}", p[0], p[1], p[2], p[3]);

  kleardb::serialize(x, buf);
  fmt::println("big endian serialized: {:b} {:b} {:b} {:b}", buf[0], buf[1], buf[2], buf[3]);

  fmt::println("big endian deserialized (slow): {:d}", kleardb::deserialize_slow(buf));

  kleardb::deserialize(x, buf);
  fmt::println("big endian deserialized: {:d}", x);
}

TEST(seralize_u64, basic) {
  uint8_t buf[1024];

  uint64_t x = 100;
  uint8_t* p = (uint8_t*) &x;
  fmt::println("memory: {:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

  kleardb::serialize(x, buf);
  fmt::println("big endian serialized: {:b} {:b} {:b} {:b} {:b} {:b} {:b} {:b}", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);

  uint64_t y;
  kleardb::deserialize(y, buf);
  fmt::println("big endian deserialized: {:d}", x);
}

TEST(seralize_str, basic) {
  // Expect two strings not to be equal.
  uint8_t buf[1024];

  std::string s = "test that it works";
  kleardb::serialize(s, buf);
  std::string s_out;
  kleardb::deserialize(s_out, buf);
  EXPECT_STREQ(s.c_str(), s_out.c_str());
}

TEST(schema, basic) {
  kleardb::Schema schema = kleardb::Schema::ParseSchema({{"A", "U64"}, {"B", "String"}, {"C", "U32"}});
  std::vector<std::vector<std::tuple<std::string, kleardb::SchemaVariant>>> data = {
    {{"A", {3ul}}, {"B", {"test"}}, {"C", {5u}}},
    {{"A", {(1ul)<<48}}, {"B", {"hello"}}, {"C", {4u}}}
  };

  std::vector<kleardb::Row> rows;
  for (auto& d : data) {
    rows.push_back(kleardb::Row::ParseRow(schema, d));
  }

  uint8_t buf[1024*1024];

  uint8_t* ptr = buf;
  for (auto row : rows) {
    ptr += row.serialize(ptr);
  }

  ptr = buf;
  for (size_t i = 0; i < rows.size(); i++) {
    kleardb::Row output;
    size_t offset = output.deserialize(schema, ptr);
    ptr += offset;

    output.print();
    EXPECT_EQ(output, rows[i]);
    EXPECT_EQ(output[0], std::get<1>(data[i][0]));
    EXPECT_EQ(output[1], std::get<1>(data[i][1]));
    EXPECT_EQ(output[2], std::get<1>(data[i][2]));
  }
}