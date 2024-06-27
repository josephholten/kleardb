#include "docopt.cpp/docopt.h"
#include "nlohmann/json.hpp"
#include <chrono>
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdint.h"

using json = nlohmann::json;

static const char USAGE[] =
R"(profile

Usage:
    profile read <data_path> [--json]
)";

void profile_read(std::string data_path, bool json_logging) {
    int fd = open(data_path.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    json data = json::array();

    for (size_t k = 8; k <= 16; k++) {
        lseek64(fd, 0, SEEK_SET);
        size_t block_size = 1 << k;
        std::vector<uint8_t> block(block_size);
        volatile size_t total = 0;

        loff_t file_size = lseek64(fd, 0, SEEK_END);
        if (file_size < 0)
        {
            perror("file_size");
            return;
        }
        // reset to start
        lseek64(fd, 0, SEEK_SET);

        size_t blocks = file_size / block_size;

        size_t total_latency = 0;
        size_t max_latency = 0;

        for (size_t offset = 0; offset < (size_t)file_size; offset += block_size) {
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

        double avg_latency = (double)total_latency / blocks;
        double throughput = (double)file_size / total_latency;
        if (json_logging) {
            data.push_back({
                {"block_size", block_size},
                {"avg_latency", avg_latency},
                {"max_latency", max_latency},
                {"throughput", throughput}
            });
        } else {
            printf("block_size %ld bytes\n", block_size);
            printf("  avg_latency: %lf ns\n", avg_latency);
            printf("  max_latency: %ld ns\n", max_latency);
            printf("  throughput:  %lf bytes/ns\n", throughput);
        }
    }

    if (json_logging) {
        printf("%s", data.dump().c_str());
    }

    close(fd);
}

int main(int argc, char** argv) {
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE, {argv + 1, argv + argc});

    if (args["read"]) {
        profile_read(args["<data_path>"].asString(), args["--json"].asBool());
    }
}