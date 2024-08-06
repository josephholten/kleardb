#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <fmt/core.h>
#include <fcntl.h>
#include "kleardb/serialize.hpp"


int main() {
    std::string database_path = "basic.db";

    int db_fd = open(database_path.c_str(), O_CREAT | O_RDWR);
}