#include "regression.h"
#include <stdio.h>
#include <stdlib.h>

tf_data regression(tile& domain, tile& range, int orient) {
    if(domain.sum_sq == 0) {
        tf_data ret = {
            .shrink_ratio = 2,
            .d_block = {0, 0},
            .orient = 0,
            .contrast = 0,
            .brightness = 0,
            .residual = 100000
        };
        return ret;
    }
    float contrast = 0;

    for(int i = 0; i < TILE_SIZE; i++) {
        for(int j = 0; j < TILE_SIZE; j++) {
            int y, x;
            calc_pos(i, j, &y, &x, orient);

            contrast += (domain.vals[y][x] - domain.mean) * (range.vals[i][j] - range.mean);
        }
    }

    contrast /= domain.sum_sq;
    float brightness = range.mean - contrast * domain.mean;

    float residual = 0;
    for(int i = 0; i < TILE_SIZE; i++) {
        for(int j = 0; j < TILE_SIZE; j++) {
            int y,x;
            calc_pos(i, j, &y, &x, orient);

            float temp = (domain.vals[y][x] * contrast + brightness - range.vals[i][j]);
            residual += temp * temp;
        }
    }

    tf_data ret = {
        .shrink_ratio = 2,
        .d_block = {0, 0},
        .orient = orient,
        .contrast = contrast,
        .brightness = brightness,
        .residual = residual
    };
    return ret;
}
