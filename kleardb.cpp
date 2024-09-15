#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <fmt/core.h>
#include <fcntl.h>
#include <unistd.h>
#include "kleardb/serialize.hpp"
#include <array>
#include <sys/types.h>
#include <errno.h>

using byte_t = uint8_t;

static constexpr size_t KiB = 1024;
//static constexpr size_t MiB = KiB * KiB;
static constexpr size_t BlockSize = 8 * KiB;

void write_block(int fd, off_t offset, byte_t* block) {
    off_t pos = lseek(fd, offset, SEEK_SET);
    if (pos < 0) {
        fmt::println("Error in write_block lseek: {}", strerror(errno));
        exit(1);
    }
    assert(pos == offset);
    ssize_t count = write(fd, block, BlockSize);
    if (count < 0) {
        fmt::println("Error in write_block write: {}", strerror(errno));
        exit(1);
    }
    if ((size_t)count < BlockSize) {
        // TODO: try again to write remaining bytes
        fmt::println("Error in write_block: could only write {}/{}", count, BlockSize);
        exit(1);
    }
};

void read_block(int fd, size_t offset, byte_t* block) {
    off_t pos = lseek(fd, offset, SEEK_SET);
    if (pos < 0) {
        fmt::println("Error in read_block lseek: {}", strerror(errno));
        exit(1);
    }
    assert((size_t)pos == offset);
    ssize_t count = read(fd, block, BlockSize);
    if (count < 0) {
        fmt::println("Error in read_block read: {}", strerror(errno));
        exit(1);
    }
    if ((size_t)count < BlockSize) {
        // TODO: try again to read remaining bytes
        fmt::println("Error in read_block: could only read {}/{}", count, BlockSize);
        exit(1);
    }
};

int main() {
    std::string database_path = "basic.db";

    std::array<byte_t, BlockSize> block = {0};

    // create file if it doesn't exist and open with read/write access
    int flags = O_CREAT | O_RDWR;
    // read write for user, read for group
    int mode = S_IRUSR | S_IWUSR | S_IRGRP;
    int db_fd = open(database_path.c_str(), flags, mode);

    // parse schema
    // parse some rows
    // serialize rows into block

    write_block(db_fd, 0, block.data());
    read_block(db_fd, 0, block.data());

    // deserialize rows
    // output rows

    close(db_fd);
}