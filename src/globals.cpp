#include "globals.h"

unsigned char image[IMAGE_SIZE][IMAGE_SIZE] = {0};
tile range_blocks[NUM_R_BLOCKS][NUM_R_BLOCKS] = {0};
tile domain_blocks[NUM_D_BLOCKS][NUM_D_BLOCKS] = {0};
tf_data compressed_data[NUM_R_BLOCKS][NUM_R_BLOCKS] = {0};
