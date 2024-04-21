typedef struct precomp {
    block* domain;
    block* range;
    int orient;
    double* pfxsum;
} precomp;

precomp regression_precomp(block* domain, block* range, int orient, tf_data& tf) {
    //work towards a reusable regression
    //float ds = (float)blockSum(domain);
    //float dsq = (float)blockSumOfSq(domain);

    float denom = domain->size * domain->size * dsq - ds * ds;

    if(denom == 0) {
        return 100000;
    }

    double* pfxsum = malloc(sizeof(double)*domain->size*domain->size);
    
    float xy = 0;
    for(unsigned int i = 0; i < domain->size; i++) {
        for(unsigned int j = 0; j < domain->size; j++) {
            unsigned int y = i, x = j;
            calc_pos(&y, &x, orient, domain->size);

            pfxsum[i * domain->size + j] = yx_to_val(y,x,domain) * yx_to_val(i,j,range);
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

float regression_end(precomp comp, tf_data& tf, int i, int j) { // prob only nec to look at corresp i j in domain, range?
    float ds = (float) blockSum(domain);
    float dsq = (float) blockSumofSq(domain);

    // ... and so on

}

float regression_check(block* domain, block* range, int orient, tf_data& tf) {
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

    float sumdr = 0;

    for(unsigned int i = 0; i < domain->size; i++) {
        for(unsigned int j = 0; j < domain->size; j++) {
            unsigned int y = i, x = j;
            calc_pos(&y, &x, orient,domain->size);

            float temp = (yx_to_val(y,x,domain) * tf.contrast + tf.brightness 
                    - yx_to_val(i,j,range));
            temp *= temp;
            residual += temp;

            sumdr += yx_to_val(y,x,domain) * yx_to_val(i,j,range);
        }
    }

    float closed_form = tf.contrast * tf.contrast * dsq 
        + 2*tf.contrast*tf.brightness*ds 
        - 2*tf.contrast * sumdr 
        + tf.brightness * tf.brightness * (float)domain->size * (float)domain->size 
        - 2 * tf.brightness*rs 
        + ((float)blockSumOfSq(range));

    /* printf("%f, %f, makes %f\n", residual, closed_form, residual - closed_form); */

    return residual;
}
