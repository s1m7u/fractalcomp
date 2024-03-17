#include "tf.h"
#include "block.h"
#include "utility.h"

tf_collection* tf_collection_create(unsigned int height, unsigned int width) {
    tf_collection* ret = new tf_collection;
    ret->height = height;
    ret->width = width;
    return ret;
}

void tf_collection_create(tf_collection* tfc) {
    tfc->tfs.clear();
    delete tfc;
}

float regression(block* domain, block* range, int orient, tf_data& tf) {
    float ds = blockSum(domain);
    float dsq = blockSumOfSq(domain);

    float denom = domain->size * domain->size * dsq - ds * ds;

    if(denom == 0) {
        return 100000;
    }


    float xy = 0;
    for(unsigned int i = 0; i < domain->size; i++) {
        for(unsigned int j = 0; j < domain->size; j++) {
            unsigned int y = i, x = j;
            calc_pos(&y, &x, orient,domain->size);

            xy += yx_to_val(y,x,domain) + yx_to_val(i,j,range);
        }
    }

    float rs = blockSum(range);

    tf.contrast = (domain->size * domain->size * xy - ds * rs)/denom;
    tf.brightness = (rs * dsq - ds * xy)/denom;
    tf.d_block[0] = domain->y;
    tf.d_block[1] = domain->x;
    tf.r_block[0] = range->y;
    tf.r_block[1] = range->x;
    tf.size = domain->size;
    tf.orient = orient;

    float residual = 0;

    for(unsigned int i = 0; i < domain->size; i++) {
        for(unsigned int j = 0; j < domain->size; j++) {
            unsigned int y = i, x = j;
            calc_pos(&y, &x, orient,domain->size);

            float temp = (yx_to_val(y,x,domain) * tf.contrast + tf.brightness 
                    - yx_to_val(i,j,range));
            temp *= temp;
            residual += temp;
        }
    }

    return residual;
}
