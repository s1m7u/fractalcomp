#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "regression.h"
#include "constants.h"
#include "encode.h"
#include "structs.h"
#include "globals.h"
#include "decode.h"
#include "utility.h"

// main function
int main() {
    srand(SEED);

    import_image("test/bird.png");

    init_image();

    save_image("original.png", range_blocks);

    printf("INITIALISATION FINISHED\n");

    /* print_image("image.txt", image); */

    encode();

    printf("ENCODING FINISHED\n");

    print_encoding();

    decode();
}
