#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "constants.h"
#include "encode.h"
#include "decode.h"
#include "image.h"

// main function
int main() {
    srand(SEED);

    /* import_image("test/bird.png"); */
    /* import_image("test/hs.png"); */
    /* import_image("test/hybrid.png"); */
    /* import_image("test/birdinhouse.png"); */

    debug("READING IMAGE\n");

    /* image* range_blocks = image_from_png("test/bird.png"); */
    /* image* range_blocks = image_from_png("test/hs.png"); */
    /* image* range_blocks = image_from_png("test/hybrid.png"); */
    /* image* range_blocks = image_from_png("test/birdinhouse.png"); */
    image* range_blocks = image_from_png("test/peppers.png");
    image* range_blocks = image_from_png("test/airport.png");

    debug("READING IMAGE\n");


    debug("DOWNSAMPLING IMAGE\n");

    image* domain_blocks = downsample_by_two(range_blocks);

    debug("DOWNSAMPLING COMPLETE\n");


    /* print_image("image.txt", image); */

    debug("ENCODING IMAGE\n");

    tf_collection* compress = encode(range_blocks, domain_blocks, BLOCK_SIZE);

    debug("ENCODING FINISHED\n");

    print_encoding(compress);

    debug("DECODING IMAGE\n");

    decode(compress);

    debug("DECODING FINISHED\n");

    image_destroy(range_blocks);

    image_destroy(domain_blocks);
}
