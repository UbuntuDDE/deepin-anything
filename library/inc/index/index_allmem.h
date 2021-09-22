#pragma once

#include <stdint.h>

#include "index.h"
#include "index_base.h"

typedef struct __fs_allmem_index__ fs_allmem_index;

int load_allmem_index(fs_index** pfsi, int fd, uint32_t count);
fs_allmem_index* new_allmem_index(uint32_t count);
int save_allmem_index(fs_allmem_index* ami, const char* filename);

