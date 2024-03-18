#include <stdio.h>

#include "tf.h"
#include "block.h"

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
    float ds = (float)blockSum(domain);
    float dsq = (float)blockSumOfSq(domain);

    float denom = domain->size * domain->size * dsq - ds * ds;

    if(denom == 0) {
        return 100000;
    }


    float xy = 0;
    for(unsigned int i = 0; i < domain->size; i++) {
        for(unsigned int j = 0; j < domain->size; j++) {
            unsigned int y = i, x = j;
            calc_pos(&y, &x, orient, domain->size);

            xy += yx_to_val(y,x,domain) * yx_to_val(i,j,range);
        }
    }

    float rs = (float)blockSum(range);

    tf.contrast = ((float)(domain->size * domain->size * xy) - ds * rs)/denom;
    /* tf.brightness = (float)(rs * dsq - ds * xy)/denom; */
    tf.brightness = (float)(rs - tf.contrast * ds)/(domain->size*domain->size);
    /* printf("%f %f\n", tf.contrast, tf.brightness); */
    tf.d_block[0] = domain->y;
    tf.d_block[1] = domain->x;
    tf.r_block[0] = range->y;
    tf.r_block[1] = range->x;
    tf.size = range->size;
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

void print_encoding(tf_collection* tfc) {
    FILE *encoding_file = fopen("encoding.txt", "w");
    for(tf_data t : tfc->tfs) {
        fprintf(
                encoding_file,
                "(%2d, %2d) -> (%2d %2d) "
                "s:%4d o:%2d c:%+2.2f b:%+2.2f\n", 
                t.d_block[0],
                t.d_block[1],
                t.r_block[0],
                t.r_block[1],
                t.size,
                t.orient,
                t.contrast,
                t.brightness
               );
    }        
    fclose(encoding_file);
    printf("Success: encoding.txt created.\n");
}
