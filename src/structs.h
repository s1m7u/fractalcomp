#ifndef STRUCTS_H
#define STRUCTS_H

#include "constants.h"

typedef struct struct_tile {
    int vals[R_BLOCK_SIZE][R_BLOCK_SIZE]; // 4 x 4
    float mean;
    float sum_sq;
} tile;

typedef struct struct_tf_data {
    int shrink_ratio;
    int d_block[2]; // coordinates
    int orient;
    float contrast;
    float brightness;
    float residual;
} tf_data;


#endif

