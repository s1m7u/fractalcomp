#ifndef CONSTANTS_H
#define CONSTANTS_H

#define IMAGE_SIZE 256
#define TILE_SIZE 4
#define R_BLOCK_SIZE 4
#define D_BLOCK_SIZE 8

#define NUM_ITERATIONS 20

#define SEED 1243

#define NUM_TILES IMAGE_SIZE / TILE_SIZE
#define NUM_D_BLOCKS IMAGE_SIZE / D_BLOCK_SIZE
#define NUM_R_BLOCKS IMAGE_SIZE / R_BLOCK_SIZE

extern int orientations[8][2][2];

#endif
