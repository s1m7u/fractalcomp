#include "block.h"

block* block_create(int y, int x, int size, image* img) {
    block *ret = new block;
    ret->y = y;
    ret->x = x;
    ret->size = size;
    ret->img = img;
    return ret;
}

void block_destroy(block* b) {
    delete b;
}

