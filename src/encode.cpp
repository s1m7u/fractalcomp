#include "encode.h"
#include <stdlib.h>
#include <stdio.h>

void encode() {
    print_image("huh.txt", range_blocks);
    // for each range block:
    for (int i = 0; i < NUM_R_BLOCKS; i++) {
        for (int j = 0; j < NUM_R_BLOCKS; j++) {
            /* printf("PROCESSING BLOCK: %d %d\n", i, j); */
            // find the minimum residuals
            if(range_blocks[i][j].sum_sq == 0) {
                compressed_data[i][j] = {
                    .shrink_ratio = 2,
                    .d_block = {0, 0},
                    .orient = 0,
                    .contrast = 0,
                    .brightness = range_blocks[i][j].vals[0][0],
                    .residual = 0
                };
                continue;
            }

            tf_data minimum = regression(domain_blocks[0][0], range_blocks[i][j], 0);
            for (int k = 0; k < NUM_D_BLOCKS; k++) {
                for (int l = 0; l < NUM_D_BLOCKS; l++) {
                    for (int o = 0; o < 8; o++) {
                        tf_data temp = regression(domain_blocks[k][l], range_blocks[i][j], o);

                        /* printf("%f %f\n", range_blocks[i][j].mean, domain_blocks[k][l].mean); */
                        /* printf("%f %f\n", temp.contrast, temp.brightness); */
                        if(temp.brightness < -120 
                                || temp.brightness > 120
                                || temp.contrast < -8
                                || temp.contrast > 8) {
                            continue;
                        }
                        if(temp.residual < minimum.residual) {
                            minimum = temp;
                            minimum.d_block[0] = k;
                            minimum.d_block[1] = l;
                        }
                    }
                    /* printf("\n"); */
                }
            }
            compressed_data[i][j] = minimum;
        }
    }
}

