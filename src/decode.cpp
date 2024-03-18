#include "decode.h"
#include "tf.h"
#include <stdio.h>

void decode(tf_collection* compress) {
    unsigned int height = compress->height;
    unsigned int width = compress->width;
    image* buffers[2];
    buffers[0] = image_create(height, width);
    buffers[1] = image_create(height, width);

    for(unsigned int y = 0; y < height; y++) {
        for(unsigned int x = 0; x < width; x++) {
            /* yx_to_val(y, x, buffers[0]) = 120; */
            yx_to_val(y, x, buffers[0]) = rand() % 256;
        }
    }

    save_image("random.png", buffers[0]);
    print_image("random_image.txt", buffers[0]);

    for(unsigned int iter = 0; iter < NUM_ITERATIONS; iter++) {
        for(unsigned int y = 0; y < height/2; y++) {
            for(unsigned int x = 0; x < width/2; x++) {
                yx_to_val(y,x,buffers[iter%2]) = (
                        yx_to_val(2*y,2*x,buffers[iter%2]) +
                        yx_to_val(2*y+1,2*x,buffers[iter%2]) +
                        yx_to_val(2*y,2*x+1,buffers[iter%2]) +
                        yx_to_val(2*y+1,2*x+1,buffers[iter%2]))/4;
            }
        }
        for(tf_data t : compress->tfs) {
            for(unsigned int i = 0; i < t.size; i++) {
                for(unsigned int j = 0; j < t.size; j++) {
                    unsigned int y = i, x = j;
                    calc_pos(&y, &x, t.orient, t.size);

                    float temp = yx_to_val(t.d_block[0]+y,t.d_block[1]+x,buffers[iter%2]);
                    temp = t.contrast*temp + t.brightness;
                    yx_to_val(t.r_block[0]+i, t.r_block[1]+j, buffers[(iter+1)%2]) = (unsigned int)temp;
                }
            }
        }
    }

    save_image("decoded.png", buffers[NUM_ITERATIONS%2]);
    print_image("decoded_image.txt", buffers[NUM_ITERATIONS%2]);

    image_destroy(buffers[0]);
    image_destroy(buffers[1]);
}
