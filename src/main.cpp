#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "constants.h"
#include "encode.h"
#include "decode.h"
#include "image.h"
#include "jpeg.h"

// main function
int main() {
    srand(SEED);

    /* import_image("test/bird.png"); */
    /* import_image("test/hs.png"); */
    /* import_image("test/hybrid.png"); */
    /* import_image("test/birdinhouse.png"); */

    // debug("READING IMAGE\n");

    image *to_jpeg = image_from_png("test/bird.png");
    image_to_jpeg("bird.jpeg", to_jpeg, JPEG_COMPRESSION_QUALITY);
    encode_stats_pj("test/bird.png", "bird.jpeg");

    image* range_blocks = image_from_png("test/bird.png");
    /* image* range_blocks = image_from_png("test/hs.png"); */
    /* image* range_blocks = image_from_png("test/hybrid.png"); */
    /* image* range_blocks = image_from_png("test/birdinhouse.png"); */

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

    decode(compress, "test/bird.png"); 
    // we only need the name of the original file to compute the rmse at each iteration

    debug("DECODING FINISHED\n");

    image_destroy(range_blocks);
    image_destroy(domain_blocks);

    encode_stats_pp("test/bird.png", "decoded.png");
}
