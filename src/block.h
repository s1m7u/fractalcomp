#ifndef BLOCK_H
#define BLOCK_H

#include "config.h"
#include "image.h"

typedef struct struct_block {
    //Top left
    unsigned int y, x;
    unsigned int size;
    image* img;
} block;

block* block_create(int y, int x, int size, image* img);
void block_destroy(block* b);

inline int blockSum(block* b) {
    return 
        b->img->pfxSum[(b->y+b->size)*(b->img->width+1) + (b->x+b->size)]
        - b->img->pfxSum[(b->y)*(b->img->width+1) + (b->x + b->size)]
        - b->img->pfxSum[(b->y+b->size)*(b->img->width+1) + (b->x)]
        + b->img->pfxSum[(b->y)*(b->img->width+1) + (b->x)];
}

inline long int blockSumOfSq(block* b) {
    return
        b->img->pfxSumOfSq[(b->y+b->size)*(b->img->width+1) + (b->x+b->size)]
        - b->img->pfxSumOfSq[(b->y)*(b->img->width+1) + (b->x + b->size)]
        - b->img->pfxSumOfSq[(b->y+b->size)*(b->img->width+1) + (b->x)]
        + b->img->pfxSumOfSq[(b->y)*(b->img->width+1) + (b->x)];
}

//assumes y,x < size
inline unsigned int& yx_to_val(int y, int x, block* b) {
    return yx_to_val(b->y + y, b->x + x,b->img);
}

#endif
