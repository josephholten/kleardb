#include "docopt.cpp/docopt.h"
#include <chrono>
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdint.h"

static const char USAGE[] =
R"(profile

Usage:
    profile read <data_path> [--read_size=<int>]
)";

void profile_read(std::string data_path, size_t block_size) {
    int fd = open(data_path.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    std::vector<uint8_t> block(block_size);
    volatile size_t total = 0;

    loff_t file_size = lseek64(fd, 0, SEEK_END);
    if (file_size < 0) {
        perror("file_size");
        return;
    }
    // reset to start
    lseek64(fd, 0, SEEK_SET);

    size_t blocks = file_size / block_size;

    double total_latency = 0;
    size_t max_latency = 0;

    for (size_t offset = 0; offset < (size_t)file_size; offset += file_size / block_size) {
        auto start = std::chrono::high_resolution_clock::now();
        ssize_t bytes = read(fd, block.data(), block_size);
        size_t latency = (std::chrono::high_resolution_clock::now() - start).count();
        if (bytes < 0) {
            perror("read");
            break;
        } else if (bytes == 0) {
            printf("zero bytes read -> end of file?\n");
            break;
        }
        // only for the read not to be optimized away
        for (uint8_t x : block)
            total += x;

        total_latency += latency;
        max_latency = std::max(latency, max_latency);

        offset += bytes;
    }
    printf("block_size %ld:\n", block_size);
    printf("  avg_latency: %lf\n", (double) total_latency / blocks);
    printf("  max_latency: %ld\n", max_latency);
    printf("  throughput:  %lf\n", (double) total_latency / file_size);
    close(fd);
}

int main(int argc, char** argv) {
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE, {argv + 1, argv + argc});

    if (args["read"]) {
        const uint64_t read_size = (args["--read_size"] ? args["--read_size"].asLong() : 512);
        profile_read(args["<data_path>"].asString(), read_size);
    }
}