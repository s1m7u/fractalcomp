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
    ret->size = s;
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

double dpsSumFrom(dps* dr, int tlx, int tly, int brx, int bry) {
    // FROM tlx tly TO brx bry NOT INCLUDING brx line, bry line
    double ret = dr->pfxSum[bry * (dr->size + 1) + brx]
        - dr->pfxSum[tly * (dr->size + 1) + brx]
        - dr->pfxSum[bry * (dr->size + 1) + tlx]
        + dr->pfxSum[tly * (dr->size + 1) + tlx];
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
    // FROM tlx tly TO brx bry NOT INCLUDING brx line, bry line
    block* domain = comp.domain;
    block* range = comp.range;
    int orient = comp.orient;
    dps* dr = comp.dr;

    unsigned int dtlx = tlx, dtly = tly, dbrx = brx, dbry = bry;
    calc_pos(&dtly, &dtlx, orient, domain->size);
    calc_pos(&dbry, &dbrx, orient, domain->size);
    /*
    if (orient != 0) {
        printf("ORIENT %d\n", orient);
        printf("TL: %d %d, BR: %d %d\n", tlx, tlx, brx, bry);
        printf("D: TL: %d %d, BR: %d %d\n", dtlx, dtlx, dbrx, dbry);
        }*/
    if (dtlx > dbrx) {
        int s = dbrx;
        dbrx = dtlx + 1;
        dtlx = s - 1;
    }
    if (dtly > dbry) {
        int s = dbry;
        dbry = dtly + 1;
        dtly = s - 1;
    }
    /*
    if (orient != 0) {
        printf("but now ORIENT %d\n", orient);
        printf("TL: %d %d, BR: %d %d\n", tlx, tlx, brx, bry);
        printf("D: TL: %d %d, BR: %d %d\n", dtlx, dtlx, dbrx, dbry);
        }*/
    /*
    double ds = (double) blockSumFrom(domain, dtlx, dtly, dbrx, dbry);
    double dsq = (double) blockSumOfSqFrom(domain, dtlx, dtly, dbrx, dbry);
    */
    double ds = (double) blockSum (domain);
    double dsq = (double) blockSumOfSq(domain);
    
    double rs = (double) blockSumFrom(range, tlx, tly, brx, bry);
    double rsq = (double) blockSumOfSqFrom(range, tlx, tly, brx, bry);
    double xy = dpsSumFrom(dr, tlx, tly, brx, bry);

    int rect_area = (brx - tlx) * (bry - tly);

    double denom = rect_area * dsq - ds * ds;

    if(denom == 0) {
        return 100000;
    }


    tf.contrast = ((double)(rect_area * xy) - ds * rs)/denom;
    /* tf.brightness = (double)(rs * dsq - ds * xy)/denom; */
    tf.brightness = (double)(rs - tf.contrast * ds)/(rect_area);
    /* printf("%f %f\n", tf.contrast, tf.brightness); */

    // Unsure of how to make these jive with arbitrary rects.
    // not nec for calcing residual, but... ?
    tf.d_block[0] = domain->y;
    tf.d_block[1] = domain->x;
    tf.r_block[0] = range->y;
    tf.r_block[1] = range->x;
    tf.size = range->size;
    tf.orient = orient;

    double residual = 
        tf.contrast * tf.contrast * dsq 
        + 2*tf.contrast*tf.brightness*ds 
        - 2*tf.contrast * xy
        + tf.brightness * tf.brightness * rect_area
        - 2 * tf.brightness*rs 
        + rsq;
    
    return residual;
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
