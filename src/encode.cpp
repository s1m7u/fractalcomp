#include "encode.h"
#include <stdlib.h>
#include <stdio.h>

void encode() {
    // for each range block:
    for (int i = 0; i < NUM_R_BLOCKS; i++) {
        for (int j = 0; j < NUM_R_BLOCKS; j++) {
            /* printf("PROCESSING BLOCK: %d %d\n", i, j); */
            // find the minimum residuals
            tf_data minimum = regression(domain_blocks[0][0], range_blocks[i][j], 0);
            for (int k = 0; k < NUM_D_BLOCKS; k++) {
                for (int l = 0; l < NUM_D_BLOCKS; l++) {
                    for (int o = 0; o < 8; o++) {
                        tf_data temp = regression(domain_blocks[k][l], range_blocks[i][j], o);
                        if(temp.residual < minimum.residual) {
                            minimum = temp;
                            minimum.d_block[0] = k;
                            minimum.d_block[1] = l;
                        }
                    }
                }
            }
            compressed_data[i][j] = minimum;
        }
    }
}

