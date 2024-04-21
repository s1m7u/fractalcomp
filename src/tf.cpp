#include <stdio.h>
#include <stdlib.h>

#include <math.h>

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

/// start new shit

typedef struct doublepfxsum {
    double* orig;
    double* pfxSum;
    int size; 
} dps;
    

typedef struct precomp {
    block* domain;
    block* range;
    int orient;
    dps* dr;
} precomp;

dps* dps_from_doublep(double* p, int s) {
    dps* ret = new dps;
    ret->pfxSum = new double[(s + 1) * (s + 1)];
    ret->orig = p;
    // now make the pfxsum
    for (unsigned int y = 1; y <= s; y++) {
        for (unsigned int x = 1; x <= s; x++) {
            ret->pfxSum[y*(s+1)+x] =
                + ret->pfxSum[(y - 1)*(s+1)+x]
                + ret->pfxSum[y*(s+1)+x-1]
                - ret->pfxSum[(y-1)*(s+1)+x-1]
                + ret->orig[(y-1)*s+x-1];
        }
    }
    return ret;
}

precomp regression_precomp(block* domain, block* range, int orient) {
    double* orig = new double[(domain->size) * (domain->size)];
    
    float xy = 0;
    for(unsigned int i = 0; i < domain->size; i++) {
        for(unsigned int j = 0; j < domain->size; j++) {
            unsigned int y = i, x = j;
            calc_pos(&y, &x, orient, domain->size);

            orig[i * domain->size + j] = yx_to_val(y,x,domain) * yx_to_val(i,j,range);
        }
    }

    dps* dr = dps_from_doublep(orig, domain->size);

    precomp ret;
    ret.domain = domain;
    ret.range = range;
    ret.orient = orient;
    ret.dr = dr;
    
    return ret;
}

float regression_pc(precomp comp, tf_data& tf, int tlx, int tly, int brx, int bry) { // prob only nec to look at corresp i j in domain, range?
    // for now we ignore the rect given, it should be fairly easy to remedy this
    block* domain = comp.domain;
    block* range = comp.range;
    int orient = comp.orient;
    dps* dr = comp.dr;
    
    double ds = (double) blockSum(domain);
    double dsq = (double) blockSumOfSq(domain);

    double rs = (double) blockSum(range);
    double rsq = (double) blockSumOfSq(range);

    double denom = domain->size * domain->size * dsq - ds * ds;

    if(denom == 0) {
        return 100000;
    }

    double xy = (dr->pfxSum[domain->size * (domain->size + 1) + domain->size]);

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

    double closed_form = 
        tf.contrast * tf.contrast * dsq 
        + 2*tf.contrast*tf.brightness*ds 
        - 2*tf.contrast * xy
        + tf.brightness * tf.brightness * domain->size * domain->size 
        - 2 * tf.brightness*rs 
        + rsq;
    
    return closed_form;
}


float regression_check(block* domain, block* range, int orient, tf_data& tf) {
    precomp comp = regression_precomp(domain, range, orient);
    return regression_pc(comp, tf, 0, 0, domain->size, domain->size);
}

/// end new shit


float regression(block* domain, block* range, int orient, tf_data& tf) {
    return regression_check(domain, range, orient, tf);
    
    double ds = (double)blockSum(domain);
    double dsq = (double)blockSumOfSq(domain);

    double rs = (double)blockSum(range);
    double rsq = (double)blockSumOfSq(range);


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

    double closed_form = 
        tf.contrast * tf.contrast * dsq 
        + 2*tf.contrast*tf.brightness*ds 
        - 2*tf.contrast * xy
        + tf.brightness * tf.brightness * domain->size * domain->size 
        - 2 * tf.brightness*rs 
        + rsq;

    return closed_form;
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
