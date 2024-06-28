#include "docopt.cpp/docopt.h"
#include "nlohmann/json.hpp"
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <chrono>

template<typename Clock = std::chrono::high_resolution_clock>
class Timer {
public:
    Timer() {
        start();
    }

    void start() {
        m_start = Clock::now();
    }

    size_t stop() {
        m_duration = Clock::now() - m_start;
        return this->ns();
    }

    size_t ns() {
        return m_duration.count();
    }

    size_t us() {
        return std::chrono::duration_cast<std::chrono::microseconds>(m_duration).count();
    }

    size_t ms() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(m_duration).count();
    }

    size_t s() {
        return std::chrono::duration_cast<std::chrono::seconds>(m_duration).count();
    }

private:
    Clock::time_point m_start;
    Clock::duration m_duration;
};

using json = nlohmann::json;
using namespace nlohmann::literals;

// serialize in big-endian
size_t serialize(uint64_t x, uint8_t* p_buf) {
    size_t number_of_bytes_written = sizeof(x);
    p_buf[0] = (x >> 56) & 0xFF;
    p_buf[1] = (x >> 48) & 0xFF;
    p_buf[2] = (x >> 40) & 0xFF;
    p_buf[3] = (x >> 32) & 0xFF;

    p_buf[4] = (x >> 24) & 0xFF;
    p_buf[5] = (x >> 16) & 0xFF;
    p_buf[6] = (x >>  8) & 0xFF;
    p_buf[7] = (x >>  0) & 0xFF;
    return number_of_bytes_written;
}

// deserialize in big endian
uint64_t deserialize(uint8_t* p_buf) {
    uint64_t x = 0;
    x |= ((uint64_t)p_buf[7] <<  0);
    x |= ((uint64_t)p_buf[6] <<  8);
    x |= ((uint64_t)p_buf[5] << 16);
    x |= ((uint64_t)p_buf[4] << 24);

    x |= ((uint64_t)p_buf[3] << 32);
    x |= ((uint64_t)p_buf[2] << 40);
    x |= ((uint64_t)p_buf[1] << 48);
    x |= ((uint64_t)p_buf[0] << 56);
    return x;
}

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
            close(fd);
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

void profile_cache(std::string data_path, bool fill, std::string tag) {
    static const size_t block_size = 8192;
    static const size_t initial_size = 4 * 1024 *  128 * block_size;
    static const size_t max_size = initial_size;
    static const size_t size_factor = 8;

    static const size_t iteration_factor = 1;

    json data = json::array();

    int fd = open(data_path.c_str(), O_RDWR | O_CREAT);
    if (fd < 0) {
        perror("Error: open");
        return;
    }

    for (size_t size = initial_size; size <= max_size; size *= size_factor) {
        fprintf(stderr, "size: %ld\n", size);

        if (fill) {
            printf("filling...\n");
            for (size_t off = lseek64(fd, 0, SEEK_END); off < size; off += block_size) {
                uint8_t block[block_size] = {0};
                ssize_t bytes = write(fd, block, block_size);
                if (bytes < 0 || (size_t)bytes < block_size) {
                    perror("Error: fill");
                    goto cleanup;
                }
            }
        }

        lseek64(fd, 0, SEEK_SET);
        for (size_t stride = block_size; stride < size; stride *= 2) {
            fprintf(stderr, "  stride: %ld\n", stride);
            // setup closed loop
            for (size_t idx = 0; idx < size; idx += stride) {
                uint8_t buf[sizeof(idx)];
                lseek64(fd, idx, SEEK_SET);
                serialize((idx + stride) % size, buf);
                write(fd, buf, sizeof(idx));
            }

            // in ns
            size_t seek = 0;
            size_t readt = 0;

            // want to keep the work load for every size roughly the same
            // so if we double the size, we half the number of iterations
            const size_t iterations = iteration_factor * max_size / size;

            for (size_t iteration = 0; iteration < iterations; iteration++) {
                for (size_t round = 0; round < stride / block_size; round++) {
                    volatile size_t idx = 0;
                    do {
                        // fprintf(stderr, "    %ld\n", idx/block_size);

                        uint8_t block[block_size];

                        Timer t;
                        lseek64(fd, idx, SEEK_SET);
                        seek += t.stop();
                        t.start();
                        ssize_t bytes = read(fd, block, block_size);
                        readt += t.stop();

                        if (bytes < 0) {
                            perror("Error: reading block");
                            goto cleanup;
                        } else if ((size_t)bytes < sizeof(idx)) {
                            fprintf(stderr, "Error: could only read %ld bytes, not enough to deserialize next idx\n", bytes);
                            goto cleanup;
                        }
                        idx = deserialize(block);
                    } while (idx != 0);
                }
            }

            size_t blocks = size / block_size;
            size_t avg_seek = seek / (blocks*iterations);
            size_t avg_readt = readt / (blocks*iterations);
            size_t bytes_read = (size / stride) * block_size;
            // fprintf(stderr, "    seek: %ld ns\n", avg_seek);
                // fprintf(stderr, "    read: %ld ns\n", avg_readt);

            data.push_back({
                {"tag", tag},
                {"size", size},
                {"bytes", bytes_read},
                {"seek", avg_seek},
                {"read", avg_readt},
                {"total", avg_seek + avg_readt}
            });
        }
    }

    printf("%s\n", data.dump().c_str());

cleanup:
    close(fd);
    remove(data_path.c_str());
}

static const char USAGE[] =
R"(profile

Usage:
    profile read <data_path> [--json]
    profile cache <data_path> [--fill] [--tag=<string>]
)";

int main(int argc, char** argv) {
    std::map<std::string, docopt::value> args = docopt::docopt(
        USAGE,
        {argv + 1, argv + argc}
    );

    if (args["read"].asBool()) {
        profile_read(args["<data_path>"].asString(), args["--json"].asBool());
    }

    else if (args["cache"].asBool()) {
        profile_cache(args["<data_path>"].asString(), args["--fill"].asBool(), args["--tag"] ? args["--tag"].asString() : "");
    }
}