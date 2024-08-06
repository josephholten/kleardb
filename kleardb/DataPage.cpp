#include<cstdint>
#include <unistd.h>
#include "DataPage.hpp"


DataPage::DataPage(uint32_t page_idx, int db_fd) {
    //initialising private variables
    this->page_idx = page_idx;
    this->db_fd = db_fd;

}

void DataPage::insert(const Item& item) {
    off_t startPos = 3; // dummy value start inserting at 3rd byte -> TOD calculate instead
    lseek(this->db_fd, startPos, SEEK_SET);
    write(this->db_fd, item.ptr, item.size);
    close(db_fd);
}