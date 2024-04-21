#include <stdio.h>
#include <stdlib.h>

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

float regression_check(block* domain, block* range, int orient, tf_data& tf) {

    double ds = (double)blockSum(domain);
    double dsq = (double)blockSumOfSq(domain);

    double rs = (double)blockSum(range);
    double rsq = (double)blockSumOfSq(range);

    int ds_check = 0;
    int dsq_check = 0;
    int rs_check = 0;
    int rsq_check = 0;
    for(unsigned int i = 0; i < domain->size; i++) {
        for(unsigned int j = 0; j < domain->size; j++) {
            ds_check += yx_to_val(i,j,domain);
            dsq_check += yx_to_val(i,j,domain) * yx_to_val(i,j,domain);
            rs_check += yx_to_val(i,j,range);
            rsq_check += yx_to_val(i,j,range) * yx_to_val(i,j,range);
        }
    }
    if(ds_check != blockSum(domain) || dsq_check != blockSumOfSq(domain)
    || rs_check != blockSum(range) || rsq_check != blockSumOfSq(range)) {
        printf("%d %d %d %d\n", rs_check, rsq_check, ds_check, dsq_check);
        printf("%f %f %f %f\n", rs, rsq, ds, dsq);
        printf("NANI\n");
        exit(0);
    }

    double denom = domain->size * domain->size * dsq - ds * ds;

    if(denom == 0) {
        return 100000;
    }

    double xy = 0;
    for(unsigned int i = 0; i < domain->size; i++) {
        for(unsigned int j = 0; j < domain->size; j++) {
            unsigned int y = i, x = j;
            calc_pos(&y, &x, orient, domain->size);

            xy += yx_to_val(y,x,domain) * yx_to_val(i,j,range);
        }
    }

    tf.contrast = ((double)(domain->size * domain->size * xy) - ds * rs)/denom;
    /* tf.brightness = (double)(rs * dsq - ds * xy)/denom; */
    tf.brightness = (double)(rs - tf.contrast * ds)/(domain->size*domain->size);
    /* printf("%f %f\n", tf.contrast, tf.brightness); */
    tf.d_block[0] = domain->y;
    tf.d_block[1] = domain->x;
    tf.r_block[0] = range->y;
    tf.r_block[1] = range->x;
    tf.size = range->size;
    tf.orient = orient;

    double residual = 0;

    double sumdr = 0;

    for(unsigned int i = 0; i < domain->size; i++) {
        for(unsigned int j = 0; j < domain->size; j++) {
            unsigned int y = i, x = j;
            calc_pos(&y, &x, orient,domain->size);

            double temp = (yx_to_val(y,x,domain) * tf.contrast + tf.brightness 
                    - yx_to_val(i,j,range));
            /* double temp = (yx_to_val(y,x,domain) * tf.contrast + tf.brightness ); */
            /* double temp = (yx_to_val(y,x,domain) * tf.contrast); */
            /* double temp = ((double)yx_to_val(i,j,domain) ) * ((double)yx_to_val(i,j,domain) ); */
            /* temp *= temp; */
            residual += temp;

            sumdr += yx_to_val(y,x,domain) * yx_to_val(i,j,range);
        }
    }

    double closed_form = 
        tf.contrast * tf.contrast * dsq 
        + 2*tf.contrast*tf.brightness*ds 
        - 2*tf.contrast * xy
        + tf.brightness * tf.brightness * domain->size * domain->size 
        - 2 * tf.brightness*rs 
        + ((double)blockSumOfSq(range));


    if (residual - closed_form > 1000) {
        /* printf("%d %d %d %d\n", rs_check, rsq_check, ds_check, dsq_check); */
        /* printf("%f %f %f %f\n", rs, rsq, ds, dsq); */
        printf("%f, %f, makes %f\n", residual, closed_form, residual - closed_form);
    }

    return residual;
}


float regression(block* domain, block* range, int orient, tf_data& tf) {
    return regression_check(domain, range, orient, tf);
    //...
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
    debug("Success: encoding.txt created.\n");
}
