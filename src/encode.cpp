#include "encode.h"
#include "block.h"

tf_collection* encode(image* range, image* domain, unsigned int blockSize) {
    tf_collection* ret = tf_collection_create(range->height, range->width);

    block* dBlock = block_create(0,0,blockSize,domain);
    block* rBlock = block_create(0,0,blockSize,range);
    for(unsigned int ry = 0; ry < range->height; ry += blockSize) {
        for(unsigned int rx = 0; rx < range->width; rx += blockSize) {
            rBlock->y = ry;
            rBlock->x = rx;

            tf_data best = {
                .d_block = {0, 0},
                .r_block = {ry, rx},
                .size = blockSize,
                .orient = 0,
                .contrast = 0,
                .brightness = (float)yx_to_val(ry,rx,range),
            };

            tf_data cur = best;

            float minimum = 100000;
            for(unsigned int dy = 0; dy < domain->height; dy += blockSize) {
                for(unsigned int dx = 0; dx < domain->width; dx += blockSize) {
                    dBlock->y = dy;
                    dBlock->x = dx;
                    for(unsigned int o = 0; o < 8; o++) {
                        float res = regression(dBlock, rBlock, o, cur);
                        if(cur.brightness < 0 
                                || cur.brightness > 255
                                || cur.brightness + 255*cur.contrast < 0
                                || cur.brightness + 255*cur.contrast > 255) {
                            continue;
                        }
                        if(res < minimum) {
                            minimum = res;
                            best = cur;
                        }
                    }
                }
            }
            ret->tfs.push_back(best);
        }
    }

    return ret;
}

