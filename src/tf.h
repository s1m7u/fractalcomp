#ifndef TF_H
#define TF_H

#include "block.h"
#include "config.h"
#include <vector>

typedef struct struct_tf_data {
    unsigned int d_block[2];
    unsigned int r_block[2];
    unsigned int size;
    unsigned int orient;
    float contrast;
    float brightness;
} tf_data;

typedef struct struct_tf_collection {
    unsigned int height, width;
    std::vector<tf_data> tfs;
} tf_collection;

tf_collection* tf_collection_create(unsigned int height, unsigned int width);
void tf_collection_destroy(tf_collection* tfc);

float regression(block* domain, block* range, int orient, tf_data& tf);

#endif

