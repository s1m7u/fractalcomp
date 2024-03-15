#ifndef GLOBALS_H
#define GLOBALS_H

#include "structs.h"
#include "constants.h"

extern unsigned char image[IMAGE_SIZE][IMAGE_SIZE];
extern tile range_blocks[NUM_R_BLOCKS][NUM_R_BLOCKS];
extern tile domain_blocks[NUM_D_BLOCKS][NUM_D_BLOCKS];
extern tf_data compressed_data[NUM_R_BLOCKS][NUM_R_BLOCKS];

#endif
