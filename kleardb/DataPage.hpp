#include <cstdint>

struct Item {
    void* ptr;
    uint16_t size;
    uint64_t id;
};

class DataPage {
    public: 
        DataPage(uint32_t page_idx, int db_fd);
        void insert(const Item& item);
        Item read(uint64_t id);
    private:
        // index into list of pages
        uint32_t page_idx;
        // database file descriptor
        int db_fd;
};

// u64 str u64 | (10 "hello" 12) (200 "world is stupid" 500) ... | 20 4

// Startpage: schemalist, list of tuples with space and offset