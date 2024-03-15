#include "decode.h"
#include <stdio.h>

void decode() {
    tile buffers[2][NUM_TILES][NUM_TILES] = {0};

    // initialise the noisy random image and image buffer
    for (int i = 0; i < NUM_TILES; i++) {
        for (int j = 0; j < NUM_TILES; j++) {
            for (int k = 0; k < TILE_SIZE; k++) {
                for (int l = 0; l < TILE_SIZE; l++) {
                    buffers[0][i][j].vals[k][l] = rand() >> 24;
                    /* buffers[0][i][j].vals[k][l] = ((rand() % 256) + 256) % 256; */
                }
            }
        }
    }

    save_image("random.png", buffers[0]);
    print_image("random_image.txt", buffers[0]);

    for(int iter = 0; iter < NUM_ITERATIONS; iter++) {
        for(int y = 0; y < NUM_D_BLOCKS; y++) {
            for(int x = 0; x < NUM_D_BLOCKS; x++) {
                for(int i = 0; i < R_BLOCK_SIZE; i++) {
                    for(int j = 0; j < R_BLOCK_SIZE; j++) {
                        buffers[iter%2][y][x]
                            .vals[i][j] = (
                                    yx_to_val(y*D_BLOCK_SIZE+2*i,x*D_BLOCK_SIZE+2*j,buffers[iter%2]) +
                                    yx_to_val(y*D_BLOCK_SIZE+2*i+1,x*D_BLOCK_SIZE+2*j,buffers[iter%2]) +
                                    yx_to_val(y*D_BLOCK_SIZE+2*i+1,x*D_BLOCK_SIZE+2*j+1,buffers[iter%2]) +
                                    yx_to_val(y*D_BLOCK_SIZE+2*i,x*D_BLOCK_SIZE+2*j+1,buffers[iter%2])
                                    )/4;
                    }
                }
            }
        }

        for(int y = 0; y < NUM_R_BLOCKS; y++) {
            for(int x = 0; x < NUM_R_BLOCKS; x++) {
                int (&d_block)[2] = compressed_data[y][x].d_block;
                float& contrast = compressed_data[y][x].contrast;
                float& brightness = compressed_data[y][x].brightness;
                for(int i = 0; i < TILE_SIZE; i++) {
                    for(int j = 0; j < TILE_SIZE; j++) {
                        int dy, dx;
                        calc_pos(i, j, &dy, &dx, compressed_data[y][x].orient);

                        float temp = buffers[iter%2][d_block[0]][d_block[1]].vals[dy][dx];
                        temp = contrast*temp + brightness;
                        buffers[(iter+1)%2][y][x].vals[i][j] = (int) temp;
                    }
                }
            }
        }
    }
    save_image("decoded.png", buffers[NUM_ITERATIONS%2]);
    print_image("decoded_image.txt", buffers[NUM_ITERATIONS%2]);
}
